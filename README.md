# rpi-dcc-decoder
Raspberry Pi NMRA DCC Decoder Hardware and Software

A hardware and software design to connect a Raspberry Pi to a NMRA 
compliant DCC system, decode the DCC signal and display the output
on the screen.  As-is it is a "DCC Sniffer", but with minor
modification could be used to turn the Raspberry Pi into a stationary
decoder.

See [HARDWARE.md](HARDWARE.md) for the circuit design to connect a
Raspberry Pi to the tracks of an NMRA compliant DCC system.

The software uses the [pigpio](http://abyz.me.uk/rpi/pigpio/) library.
Inspired by [Frequency Counter 1](http://abyz.me.uk/rpi/pigpio/code/freq_count_1.zip).

This software is under the BSD 2-clause license, see LICENSE.

## Building

`make`

## Running

`./dcc_decode 5 -s1`

Runs on GPIO 5, with a 1us poller.  Use 1us for the best performance.

## DCC Reference Documents

* [S9.2 DCC Packet Format](https://www.nmra.org/sites/default/files/s-92-2004-07.pdf)
* [S9.2.1 DCC Extended Packet Formats](https://www.nmra.org/sites/default/files/s-9.2.1_2012_07.pdf)
* [S9.2.2 DCC Configuration Variables](https://www.nmra.org/sites/default/files/standards/sandrp/pdf/s-9.2.2_decoder_cvs_2012.07.pdf)
* [S9.2.2 Appendix A DCC Manufacturer ID Codes](https://www.nmra.org/sites/default/files/standards/sandrp/pdf/appendix_a_s-9.2.2_0.pdf)
* [S9.2.3 DCC Service Mode](https://www.nmra.org/sites/default/files/s-9.2.3_2012_07.pdf)
* [S9.2.4 DCC Fail Safe](https://www.nmra.org/sites/default/files/s-9.2.4_2012_07.pdf)
