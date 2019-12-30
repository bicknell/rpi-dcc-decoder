/***
BSD 2-Clause License

Copyright (c) 2018, Leo Bicknell
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include <pigpio.h>

#define MAX_GPIOS 32
#define MAX_BITS 64

/* Timing values found via trial and error */
/* Remember, zeros s t r e t c h */
#define TICKS_MIN_DCC_ONE 12
#define TICKS_MAX_DCC_ONE 38
#define TICKS_MIN_DCC_ZERO 68
#define TICKS_MAX_DCC_ZERO 150

#define OPT_R_MIN 1
#define OPT_R_MAX 300
#define OPT_R_DEF 10

#define OPT_S_MIN 1
#define OPT_S_MAX 10
#define OPT_S_DEF 5

#define STATE_PREAMBLE 0
#define STATE_DATA 1

typedef struct
{
   uint32_t first_tick;
   uint32_t last_high;
   uint32_t curbit;
   uint32_t state;
   uint32_t preamble;
   uint32_t dcc_one;
   uint32_t dcc_zero;
   uint32_t dcc_bad;
   uint8_t bits[MAX_BITS];
} gpioData_t;

const char * two_eight_speeds[] = {
    "STOP",
    "E-STOP",
    "Step 1",
    "Step 3",
    "Step 5",
    "Step 7",
    "Step 9",
    "Step 11",
    "Step 13",
    "Step 15",
    "Step 17",
    "Step 19",
    "Step 21",
    "Step 23",
    "Step 25",
    "Step 27",
    "STOP",
    "E-STOP",
    "Step 2",
    "Step 4",
    "Step 6",
    "Step 8",
    "Step 10",
    "Step 12",
    "Step 14",
    "Step 16",
    "Step 18",
    "Step 20",
    "Step 22",
    "Step 24",
    "Step 26",
    "Step 28"
};


static int timing_data[512];

static volatile gpioData_t g_gpio_data[MAX_GPIOS];
static volatile gpioData_t l_gpio_data[MAX_GPIOS];

static volatile int g_reset_counts[MAX_GPIOS];

static uint32_t g_mask;

static int g_num_gpios;
static int g_gpio[MAX_GPIOS];

static int g_opt_r = OPT_R_DEF;
static int g_opt_s = OPT_S_DEF;

void usage()
{
   fprintf
   (stderr,
      "\n" \
      "Usage: sudo ./freq_count_1 gpio ... [OPTION] ...\n" \
      "   -r value, sets refresh period in deciseconds, %d-%d, default %d\n" \
      "   -s value, sets sampling rate in micros, %d-%d, default %d\n" \
      "\nEXAMPLE\n" \
      "sudo ./freq_count_1 4 7 -r2 -s2\n" \
      "Monitor gpios 4 and 7.  Refresh every 0.2 seconds.  Sample rate 2 micros.\n" \
      "\n",
      OPT_R_MIN, OPT_R_MAX, OPT_R_DEF,
      OPT_S_MIN, OPT_S_MAX, OPT_S_DEF
   );
}

void fatal(int show_usage, char *fmt, ...)
{
   char buf[128];
   va_list ap;

   va_start(ap, fmt);
   vsnprintf(buf, sizeof(buf), fmt, ap);
   va_end(ap);

   fprintf(stderr, "%s\n", buf);

   if (show_usage) usage();

   fflush(stderr);

   exit(EXIT_FAILURE);
}

static int initOpts(int argc, char *argv[])
{
   int i, opt;

   while ((opt = getopt(argc, argv, "p:r:s:")) != -1)
   {
      i = -1;

      switch (opt)
      {
         case 'r':
            i = atoi(optarg);
            if ((i >= OPT_R_MIN) && (i <= OPT_R_MAX))
               g_opt_r = i;
            else fatal(1, "invalid -r option (%d)", i);
            break;

         case 's':
            i = atoi(optarg);
            if ((i >= OPT_S_MIN) && (i <= OPT_S_MAX))
               g_opt_s = i;
            else fatal(1, "invalid -s option (%d)", i);
            break;

        default: /* '?' */
           usage();
           exit(-1);
        }
    }
   return optind;
}

void edges(int gpio, int level, uint32_t tick)
{
   if (level == 1) {
       l_gpio_data[gpio].last_high = tick;
       return;
   }

   if (level == 0) {
       /* Calculate time from the last rise. */
       uint32_t difference = tick - l_gpio_data[gpio].last_high;
       timing_data[difference]++;

       if (l_gpio_data[gpio].curbit >= MAX_BITS) {
           printf("\nOverrun\n");
           /* Reset everything */
           l_gpio_data[gpio].state = STATE_PREAMBLE;
           l_gpio_data[gpio].preamble = 0;
           l_gpio_data[gpio].curbit = 0;
           for (uint32_t x = 0;x < MAX_BITS;x++) {
               l_gpio_data[gpio].bits[x] = 0;
           }
           return;
       }   

       /* Is it in range to be a DCC ONE? */
       if (difference >= TICKS_MIN_DCC_ONE && difference <= TICKS_MAX_DCC_ONE) {
           l_gpio_data[gpio].dcc_one++;

           /* If we're in the PREAMBLE state, count ones */
           if (l_gpio_data[gpio].state == STATE_PREAMBLE) {
               l_gpio_data[gpio].preamble++;

           /* If we're in the data state, and we've received bits, and the current bit is a 9th bit, and it's a 1, EOM */
           } else if (l_gpio_data[gpio].state == STATE_DATA && (l_gpio_data[gpio].curbit > 0) && ((l_gpio_data[gpio].curbit % 9) == 0)) {

               /* Now decode packet */
               switch (l_gpio_data[gpio].curbit / 9) {

               case 3:
                   if ((l_gpio_data[gpio].bits[0] ^ l_gpio_data[gpio].bits[1]) != l_gpio_data[gpio].bits[2]) {
                       printf("Checksum fails.\n");
                   }
                   if (l_gpio_data[gpio].bits[0] == 0x00 && l_gpio_data[gpio].bits[1] == 0x00) {
                       printf("RESET PACKET\n");
                   } else if (l_gpio_data[gpio].bits[0] == 0x00) {
                       printf("BROADCAST, direction %c, speed %s\n", l_gpio_data[gpio].bits[1] & 0x20 ? 'F' : 'B',
                              two_eight_speeds[l_gpio_data[gpio].bits[1] & 0x1F]);
                   } else if (l_gpio_data[gpio].bits[0] == 0xFF && l_gpio_data[gpio].bits[1] == 0x00) {
                       printf("IDLE PACKET\n");
                   } else {
                       printf("Baseline   : 7-bit Address %4d, direction %c, speed (28ss) %s\n", l_gpio_data[gpio].bits[0], 
                               l_gpio_data[gpio].bits[1] & 0x20 ? 'F' : 'B',
                              two_eight_speeds[l_gpio_data[gpio].bits[1] & 0x1F]);
                   }
                   break;

               case 4:
                   if ((l_gpio_data[gpio].bits[0] ^ l_gpio_data[gpio].bits[1] ^ l_gpio_data[gpio].bits[2]) != l_gpio_data[gpio].bits[3]) {
                       printf("4-byte Checksum fails.\n");
                       break;
                   }

               case 5:
                   if ((l_gpio_data[gpio].bits[0] ^ l_gpio_data[gpio].bits[1] ^ l_gpio_data[gpio].bits[2] ^ l_gpio_data[gpio].bits[3]) != l_gpio_data[gpio].bits[4]) {
                       printf("5-byte Checksum fails.\n");
                       break;
                   }

               case 6:
                   if ((l_gpio_data[gpio].bits[0] ^ l_gpio_data[gpio].bits[1] ^ l_gpio_data[gpio].bits[2] ^ l_gpio_data[gpio].bits[3] ^ l_gpio_data[gpio].bits[4]) != l_gpio_data[gpio].bits[5]) {
                       printf("6-byte Checksum fails.\n");
                       break;
                   }


                   uint16_t address;
                   uint16_t address_bits;
                   uint16_t instructions;

                   /* Decode the address */
                   if ((l_gpio_data[gpio].bits[0] & 0xC0) == 0xC0) {
                     address = l_gpio_data[gpio].bits[1] | ((l_gpio_data[gpio].bits[0] & 0x3F) << 8);
                     address_bits = 14;
                     instructions = 2;
                   } else if ((l_gpio_data[gpio].bits[0] & 0xC0) == 0x80) {
                     address = l_gpio_data[gpio].bits[1] | ((l_gpio_data[gpio].bits[0] & 0x3F) << 8);
                     address_bits = 9;
                     instructions = 2;
                   } else if ((l_gpio_data[gpio].bits[0] & 0xC0) == 0x00) {
                     address = l_gpio_data[gpio].bits[0];
                     address_bits = 7;
                     instructions = 1;
                   }
                   printf("Extended(%d): %d-bit Address %4d:", l_gpio_data[gpio].curbit / 9, address_bits, address);

      
                   /* Rest of the packet is instructions, except for the last byte which is checksum */
                   for (int i = instructions;i < ((l_gpio_data[gpio].curbit / 9) - 1);i++) {
                       switch (l_gpio_data[gpio].bits[i] & 0xE0) { 
                       case 0x00: /* 000 Decoder and Consisit Control Instruction */
                           switch (l_gpio_data[gpio].bits[i] & 0x10) {
                           case 0x00: /* Decoder Control */
                               printf(" Decoder Control");
                               break;
                           case 0x10: /* Consist Control */   
                               switch (l_gpio_data[gpio].bits[i] & 0x0F) {
                               case 0x00: /* Deactivate */
                                   printf(" deactivate consist %d", l_gpio_data[gpio].bits[i+1]);
                                   break;
                               case 0x02: /* Activate Normal Direction */
                                   printf(" %sactivate consist normal direction %d", l_gpio_data[gpio].bits[i+1] == 0 ? "de" : "",
                                          l_gpio_data[gpio].bits[i+1]);
                                   break;
                               case 0x03: /* Activate Reverse Direction */
                                   printf(" %sactivate consist reverse direction %d", l_gpio_data[gpio].bits[i+1] == 0 ? "de" : "",
                                          l_gpio_data[gpio].bits[i+1]);
                                   break;
                               }
                               i++;
                               break;
                           }
                           break;
                       case 0x20: /* 001 Advanced Operations Instructions */
                           switch (l_gpio_data[gpio].bits[i] & 0x1F) {
                           case 0x1F: /* 128 Speed Step Control */
                               if ((l_gpio_data[gpio].bits[i+1] & 0x7F) == 0) {
                                   printf(" direction %c, speed (128ss) STOP", l_gpio_data[gpio].bits[i+1] & 0x80 ? 'F' : 'R');
                               } else if ((l_gpio_data[gpio].bits[i+1] & 0x7F) == 1) {
                                   printf(" direction %c, speed (128ss) E-STOP", l_gpio_data[gpio].bits[i+1] & 0x80 ? 'F' : 'R');
                               } else {
                                   printf(" direction %c, speed (128ss) %d", 
                                          l_gpio_data[gpio].bits[i+1] & 0x80 ? 'F' : 'R', l_gpio_data[gpio].bits[i+1] & 0x7F);
                               }
                               i++;
                               break; 
                           case 0x1E: /* Restricted Speed Step Control */
                               printf(" restricted speed step");
                               i++;
                               break; 
                           case 0x1D: /* Analog Function Group */
                               printf(" analog function group");
                               i++;
                               break; 
                           default:
                               printf(" THIS SHOULD NEVER HAPPEN\n");
                           }
                           break;
                       case 0x40: /* 010 Speed and Direction Instruction for Reverse Operation */
                           printf(" direction R, speed (28ss) %s", two_eight_speeds[l_gpio_data[gpio].bits[i] & 0x1F]);
                           break;
                       case 0x60: /* 011 Speed and Direction Instruction for Forward Operation */
                           printf(" direction F, speed (28ss) %s", two_eight_speeds[l_gpio_data[gpio].bits[i] & 0x1F]);
                           break;
                       case 0x80: /* 100 Function Group One Instruction */
                           printf(" F0=%c, F1=%c, F2=%c, F3=%c, F4=%c",
                                  (l_gpio_data[gpio].bits[i] & 0x08) == 0x08 ? '1' : '0',
                                  (l_gpio_data[gpio].bits[i] & 0x01) == 0x01 ? '1' : '0',
                                  (l_gpio_data[gpio].bits[i] & 0x02) == 0x02 ? '1' : '0',
                                  (l_gpio_data[gpio].bits[i] & 0x03) == 0x03 ? '1' : '0',
                                  (l_gpio_data[gpio].bits[i] & 0x04) == 0x04 ? '1' : '0');
                           break;
                       case 0xA0: /* 101 Function Group Two Instruction */
                           printf(" F5=%c, F6=%c, F7=%c, F8=%c, F9=%c, F10=%c, F11=%c, F12=%c",
                                  (l_gpio_data[gpio].bits[i] & 0x11) == 0x11 ? '1' : '0',
                                  (l_gpio_data[gpio].bits[i] & 0x12) == 0x12 ? '1' : '0',
                                  (l_gpio_data[gpio].bits[i] & 0x14) == 0x14 ? '1' : '0',
                                  (l_gpio_data[gpio].bits[i] & 0x18) == 0x18 ? '1' : '0',
                                  (l_gpio_data[gpio].bits[i] & 0x01) == 0x01 ? '1' : '0',
                                  (l_gpio_data[gpio].bits[i] & 0x02) == 0x02 ? '1' : '0',
                                  (l_gpio_data[gpio].bits[i] & 0x04) == 0x04 ? '1' : '0',
                                  (l_gpio_data[gpio].bits[i] & 0x08) == 0x08 ? '1' : '0');
                           break;
                       case 0xC0: /* 110 Future Expansion */
                           printf(" Future Expansion");
                           break;
                       case 0xE0: /* 111 Configuration Variable Access Instruction */
                           printf(" Config Variable");
                           break;
                       default:
                           printf(" THIS SHOULD NEVER HAPPEN\n");
                       }
                   }
                   printf("\n");
                   break;

               default:
                   /* Print packet in hex */
                   printf("%d bytes:", l_gpio_data[gpio].curbit / 9);
                   /* 9th byte 1 means end packet */
                   for (uint32_t x = 0;x < l_gpio_data[gpio].curbit / 9;x++) {
                       printf(" %02X", l_gpio_data[gpio].bits[x]);
                   }
                   putchar('\n');

               }

               /* Reset everything */
               l_gpio_data[gpio].state = STATE_PREAMBLE;
               l_gpio_data[gpio].preamble = 0;
               l_gpio_data[gpio].curbit = 0;
               for (uint32_t x = 0;x < MAX_BITS;x++) {
                   l_gpio_data[gpio].bits[x] = 0;
               }

           /* If we're in the data state, record the data. */
           } else if (l_gpio_data[gpio].state == STATE_DATA) {
               int byte = l_gpio_data[gpio].curbit / 9;
               int shift = 8 - (l_gpio_data[gpio].curbit % 9);

               if (l_gpio_data[gpio].curbit % 9) {
/*                   printf("%d, Setting byte %d, shift %d = 1\n", l_gpio_data[gpio].curbit, byte, shift); */
                   l_gpio_data[gpio].bits[byte] |= 0x1 << shift;
               } else {
                   printf("RECEIVED ONE IN ILLEGAL SPACE\n");
               }
               l_gpio_data[gpio].curbit++;
           } else {
               printf("THIS SHOULD NEVER HAPPEN\n");

               /* Reset everything */
               l_gpio_data[gpio].state = STATE_PREAMBLE;
               l_gpio_data[gpio].preamble = 0;
               l_gpio_data[gpio].curbit = 0;
               for (uint32_t x = 0;x < MAX_BITS;x++) {
                   l_gpio_data[gpio].bits[x] = 0;
               }
           }

       /* Is it in range to be a DCC ZERO? */
       } else if (difference >= TICKS_MIN_DCC_ZERO && difference <= TICKS_MAX_DCC_ZERO) {
           l_gpio_data[gpio].dcc_zero++;

           /* Preamble is 12 or more 1's followed by a zero, change state to STATE_DATA */
           if (l_gpio_data[gpio].state == STATE_PREAMBLE && l_gpio_data[gpio].preamble >= 12) {
               l_gpio_data[gpio].state = STATE_DATA;
/*               printf("Packet start bit after %d preamble. (%d)\n", l_gpio_data[gpio].preamble, l_gpio_data[gpio].curbit); */
               l_gpio_data[gpio].curbit++;

           /* Not enough preamble, but we got a zero?  That's an error, start over */
           } else if (l_gpio_data[gpio].state == STATE_PREAMBLE) {
/*               printf("\nZero after %d premable\n", l_gpio_data[gpio].preamble);  */
               /* Reset everything */
               l_gpio_data[gpio].state = STATE_PREAMBLE;
               l_gpio_data[gpio].preamble = 0;
               l_gpio_data[gpio].curbit = 0;
               for (int x = 0;x < MAX_BITS;x++) {
                   l_gpio_data[gpio].bits[x] = 0;
               }

           /* If we're in the data state, record the zero. */
           } else if (l_gpio_data[gpio].state == STATE_DATA) {
/*               int byte = l_gpio_data[gpio].curbit / 9;
               int shift = 8 - (l_gpio_data[gpio].curbit % 9); */

               if (l_gpio_data[gpio].curbit % 9 == 0) { 
/*                   printf("Received data byte start bit.\n"); */
               } else {
/*                   printf("%d, Setting byte %d, shift %d = 0\n", l_gpio_data[gpio].curbit, byte, shift); */
               }
               l_gpio_data[gpio].curbit++;

           } else {
               printf("THIS SHOULD NEVER HAPPEN\n");
               /* Reset everything */
               l_gpio_data[gpio].state = STATE_PREAMBLE;
               l_gpio_data[gpio].preamble = 0;
               l_gpio_data[gpio].curbit = 0;
               for (int x = 0;x < MAX_BITS;x++) {
                   l_gpio_data[gpio].bits[x] = 0;
               }
           }

       /* Does it seem to be not in any valid range? */
       } else {
           l_gpio_data[gpio].dcc_bad++;

           printf("Ticks not in valid range, got %d.\n", difference);

           /* Reset everything */
           l_gpio_data[gpio].state = STATE_PREAMBLE;
           l_gpio_data[gpio].preamble = 0;
           l_gpio_data[gpio].curbit = 0;
           for (int x = 0;x < MAX_BITS;x++) {
               l_gpio_data[gpio].bits[x] = 0;
           }
       }
   }
}

int main(int argc, char *argv[])
{
   int i, rest, g, mode;

   /* command line parameters */

   rest = initOpts(argc, argv);

   /* get the gpios to monitor */

   g_num_gpios = 0;

   for (i=rest; i<argc; i++)
   {
      g = atoi(argv[i]);
      if ((g>=0) && (g<32))
      {
         g_gpio[g_num_gpios++] = g;
         g_mask |= (1<<g);
      }
      else fatal(1, "%d is not a valid g_gpio number\n", g);
   }

   if (!g_num_gpios) fatal(1, "At least one gpio must be specified");

   printf("Monitoring gpios");
   for (i=0; i<g_num_gpios; i++) printf(" %d", g_gpio[i]);
   printf("\nSample rate %d micros, refresh rate %d deciseconds\n",
      g_opt_s, g_opt_r);

   gpioCfgClock(g_opt_s, 1, 1);

   if (gpioInitialise()<0) return 1;

   /* monitor g_gpio level changes */
   for (i=0; i<g_num_gpios; i++) gpioSetAlertFunc(g_gpio[i], edges);

   mode = PI_INPUT;

   for (i=0; i<g_num_gpios; i++) gpioSetMode(g_gpio[i], mode);

   while (1)
   {
      gpioDelay(g_opt_r * 1000000);

      for (i=0; i<g_num_gpios; i++)
      {
         g = g_gpio[i];
         g_gpio_data[g] = l_gpio_data[g];

/*
         printf("Timing Data Dump Follows\n");
         for (int x = 0;x < 512;x++) {
             printf("t=%d\t%d\n", x, timing_data[x]);
         }
 */
         if (g_gpio_data[g].dcc_one > 0 || g_gpio_data[g].dcc_zero > 0) {
             printf("g=%d %8d %8d %8d\n", g, g_gpio_data[g].dcc_one, 
                    g_gpio_data[g].dcc_zero, g_gpio_data[g].dcc_bad);
         }
         l_gpio_data[g].dcc_one = 0;
         l_gpio_data[g].dcc_zero = 0;
         l_gpio_data[g].dcc_bad = 0;
      }
   }

   gpioTerminate();
}

