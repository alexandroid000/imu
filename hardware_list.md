Current Hardware
================

IMU
---
- [MPU9150 sparkfun breakout board ($30)](https://www.sparkfun.com/products/11486)
- [datasheet](
  https://cdn.sparkfun.com/datasheets/Sensors/IMU/MPU-9150-Datasheet.pdf)
- We are not using the magnetometer so this board is a bit overkill. There are
  cheaper boards out there that would be better suited for this project, but the
  MPU9150 has lots of documentation online so it's the one I tried first.

SD Card Reader/Writer
---------------------

- [source ($10)](https://www.sparkfun.com/products/544)
- this is just the reader/writer, you will also need a microSD card

Microcontroller
---------------

- [Pro Micro 3.3V/8MHz ($20)](https://www.sparkfun.com/products/12587)
- [Datasheet](https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/ProMicro8MHzv1.pdf)
- [Hookup
  guide](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide)

Battery
-------

The system needs at least 3.3V, and is rated up to about ~12 V. The
microcontroller has a voltage regulator on it, but it's safer to use a
lower-voltage source. I'm using [these](https://www.sparkfun.com/products/10718)
and they work great. The system is very low-powered, hopefully will get exact
numbers on power consumption soon.

Helpful guides
--------------

- [General blog for integrating IMU and microcontroller](
  https://github.com/kriswiner/MPU-6050/wiki/Affordable-9-DoF-Sensor-Fusion)

