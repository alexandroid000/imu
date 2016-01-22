---
title: IMU Setup Guide
...

Do You Want to Build a IMU?
---------------------------

This is a guide to creating a stand-alone, battery-powered IMU unit. It details
how to set up a microcontroller to read and log data to an SD card.

Hardware
--------

This guide is written for the MPU9150 IMU, with an Arduino Pro Micro
microcontroller.

Things you will need:

-   the hardware in [this list](hardware_list.md)
-   [Arduino software](https://www.arduino.cc/)
-   A USB-to-microUSB cable
-   wires, wirecutters, breadboard, and/or soldering iron
-   It will be helpful to have clamps and tweezers for soldering.

I HIGHLY recommend setting everything up in a breadboard before soldering
anything.

### Microcontroller

The first thing to set up is the microcontroller. This is what will initialize
the IMU, read data, and write to the SD card. Many electronics run at 5 Volts
but all of ours use 3.3V, so we can power the microcontroller from a battery
(possibly providing more than 3.3V), and use the onboard voltage regulator to
guarantee that power to all our other devices will be at the required 3.3V.

[Read this Pro Micro Hookup Guide](https://www.sparkfun.com/products/10718).
Ignore the stuff about the Fio. It will walk you through setting up your
software environment (an extra library is necessary for the Pro Micro). Follow
the steps through Example 1, "Blinkies!".

[Here is the datasheet for this
board](https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/ProMicro8MHzv1.pdf).
Warning, there are different versions of this board out there so stick with this
datasheet.

I use [this makefile] (https://github.com/sudar/Arduino-Makefile) for to compile
and upload Arduino programs, instead of the IDE, but that is personal
preference.

### SD Card

Next we'll need to set up our microcontroller to read and write from an SD card.
There are a couple different Arduino libraries for interacting with SD cards,
and the default one I've found to be slow and somewhat buggy. Instead I am using
[this SdFat library](https://github.com/greiman/SdFat) since it has binary
read/write functionality, as opposed to the SD library which writes strings.
Clone that repo and put the "SdFat" directory in your `~/Arduino/libraries/`
folder. The `html` folder contains nice documentation for this library.

The SD card uses the SPI protocol to communicate with the microcontroller.
[Blog on how SPI
works](http://nerdclub-uk.blogspot.com/2012/11/how-spi-works-with-sd-card.html),
if you're curious.

Connect these pins on the SD card reader and Arduino:

 SD             |   Arduino
----            |  ---------
GND             |   GND
VCC             |   VCC
SCK (clock)     |   15
DO (digital out)|   14
DI (digital in) |   16

There are example programs in the SdFat library. Try running
`ReadWriteSdFat.ino`, located in `SdFat/examples/ReadWriteSdFat/`. You will have
to change the first line of that program to `const int chipSelect = 10;`, as
this is chip specific. TODO: Edit this wiki with any other changes you have to
make to get it working.

### IMU

Connect these pins on the IMU and Arduino:

 IMU   |   Arduino
-----  |  ---------
GND    |   GND
VCC    |   VCC
SDA    |   2
SCL    |   3

The communication protocol between these is I2C. We'll use the `i2cdevlib`
library, found [here](http://github.com/jrowberg/i2cdevlib). Clone and put in
your Arduino libraries folder. This uses the Arduino `Wire` library so we'll
have to include both libraries in our code.

Usage
=====

-   compile and upload `arduino_code/mpu9150/mpu9150.ino`
    -   I use [this makefile] (https://github.com/sudar/Arduino-Makefile) for
        Arduino
-   run `binary_to_str.py data.txt` where `data.txt` is the binary data file
    written by the arduino to SD card
-   run functions in `path_gen.py` to plot IMU data and run low-pass filter

path_gen
========

