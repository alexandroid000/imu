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

See  for more details. You will need:

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

### SD Card

Next we'll need to set up our microcontroller to read and write from an SD card.
There are a couple different Arduino libraries for interacting with SD cards,
and the default one I've found to be slow and somewhat buggy. Instead I am using
[this SdFat library](https://github.com/greiman/SdFat) since it has binary
read/write functionality, as opposed to the SD library which writes strings.
Clone that repo and put the "SdFat" directory in your `~/Arduino/libraries/`
folder. The `html` folder contains nice documentation for this library.

The SD card uses the I2C protocol to communicate with the microcontroller. This
is why we will have to include the `Wire` library in our Arduino code.

Wiring the SD card reader and Arduino:

 SD  |   Arduino
----    ---------
GND      GND
VCC      VCC
SDA      2
SCL      3
DO       14
DI       16

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

