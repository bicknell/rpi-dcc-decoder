# rpi-dcc-decoder
Raspberry Pi NMRA DCC Decoder Hardware and Software

A hardware and software design to connect a Raspberry Pi to a NMRA 
compliant DCC system, decode the DCC signal and display the output
on the screen.  As-is it is a "DCC Sniffer", but with minor
modification could be used to turn the Raspberry Pi into a stationary
decoder.

See [HARDWARE.md](HARDWARE.md) for the circuit design to connect a Raspberry Pi to 
the tracks of an NMRA compliant DCC system.

The software uses the [pigpio](http://abyz.me.uk/rpi/pigpio/) library.
Inspired by [Frequency Counter 1](http://abyz.me.uk/rpi/pigpio/code/freq_count_1.zip).

This software is under the BSD 2-clause license, see LICENSE.
