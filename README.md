IMU
===

Code for collecting, reading and analyzing data from an IMU mounted in a rolling
ball robot

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

Hardware
========


