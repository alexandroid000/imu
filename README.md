---
title: IMU Setup Guide
...

This is a guide to creating a stand-alone, battery-powered IMU unit. It details
how to set up a microcontroller to read and log data to an SD card.

Hardware
--------

This guide is written for the MPU9150 IMU, with an Arduino Pro Micro
microcontroller.

Things you will need:

-   the hardware in [this list](hardware_list.md)
-   [Arduino software](https://www.arduino.cc/en/Main/Software)
-   A USB-to-microUSB cable
-   wires, wirecutters, breadboard, and/or soldering iron
-   It will be helpful to have clamps and tweezers for soldering.

I HIGHLY recommend testing with a breadboard before soldering anything.

### Managing Arduino Libraries

Several of the steps below involve downloading an external library. When you run
the Arduino IDE for the first time, it creates a directory (on Linux, it is
`~/Arduino`). Inside this directory should be a directory called `libraries`. If
it is not there, create it. Put all external libraries in `~/Arduino/libraries`.

Several of the libraries we use are in the form of git repositories with
multiple libraries included. For these, we clone the repo, then copy the
sub-folder that is our actual library into `~/Arduino/libraries`.

We will also need to copy the header files (`*.h`) from the libraries that we
want to use into the folder containing our code that we want to upload to the
Arduino. This is frustrating but necessary and you'll get an error like: `No
rule to make target 'MPU9150.h', needed by 'build-promicro8/mpu9150.o'.  Stop`
if you don't do this.

### Microcontroller

The first thing to set up is the microcontroller. This is what will initialize
the IMU, read data, and write to the SD card. Many electronics run at 5 Volts
but all of ours use 3.3V, so we can power the microcontroller from a battery
(possibly providing more than 3.3V), and use the onboard voltage regulator to
guarantee that power to all our other devices will be at the required 3.3V.

[Read this Arduino Pro Micro Hookup
Guide](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide).
Ignore the stuff about the Fio. It will walk you through setting up your
software environment (an extra library is necessary for the Pro Micro). Follow
the steps through Example 1, "Blinkies!".

[Here is the datasheet for the Arduino Pro
Micro](https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/ProMicro8MHzv1.pdf).
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
Clone the SdFat repo and put the subfolder `SdFat` in your
`~/Arduino/libraries/` folder. The `html` subfolder in the SdFat repo contains
nice documentation for this library.

The SD card uses the SPI protocol to communicate with the microcontroller.
[Blog on how SPI
works](http://nerdclub-uk.blogspot.com/2012/11/how-spi-works-with-sd-card.html),
if you're curious.

Connect these pins on the SD card reader and Arduino Pro Micro:

 SD             |   Arduino
----            |  ---------
GND             |   GND
VCC             |   VCC
SCK (clock)     |   15
DO (digital out)|   14
DI (digital in) |   16
CS              |   10

There are example programs in the SdFat library. Try running
`ReadWriteSdFat.ino`, located in `SdFat/examples/ReadWriteSdFat/`. You will have
to change the first line of that program to `const int chipSelect = 10;`, as
this is chip specific. TODO: Edit this wiki with any other changes you have to
make to get it working.

### IMU

Connect these pins on the IMU and Arduino Pro Micro:

 IMU   |   Arduino
-----  |  ---------
GND    |   GND
VCC    |   VCC
SDA    |   2
SCL    |   3

The communication protocol between these is I2C. It uses the `i2cdevlib`
library, found [here](http://github.com/jrowberg/i2cdevlib). Clone and put in
your Arduino libraries folder. This uses the Arduino `Wire` library so we'll
have to include both libraries in our code.

The stuff we need is in `i2cdevlib/Arduino/MPU9150`. You can look around in the
`i2cdevlib` library at all the other devices that are supported. Many sensors
use I2C so this is a nice interface.

Move the `i2cdevlib/Arduino/MPU9150` folder into your `Arduino/libraries`
directory.

### Printing IMU Data to Serial

Compile and upload the file `Examples/MPU9150_raw.ino`. If you open the Serial
interface in the Arduino IDE (looks like a magnifying glass in the top right
corner), you should see data from the IMU in real time. Try moving the sensor
around, keeping it still, etc and see if the values seem sensible. Look at the
code to see which printouts are gyrometer, accelerometer, and magnetometer data.

These values have no units yet - look at [the IMU datasheet, on pages 11, 12,
and 13](https://cdn.sparkfun.com/datasheets/Sensors/IMU/MPU-9150-Datasheet.pdf).
The line that says "ADC word length" and the corresponding value "16" indicate
that the ADC (Analog to Digital Converter) is a 16 bit number. The IMU uses all
these bits, regardless of what range it is set to (you can see the range
settings on the data sheet as well). We have to manually divide this number by
the correct scaling factor later to get real units.

But, the values should be signed and will be proportional to the real values.
What should the values look like when the IMU isn't moving? What should the sign
of the values do when the IMU changes direction?

### Saving IMU Data on SD Card

Now, take a look at the file `arduino_code/mpu9150.ino` in this repo. The file
imports the relevant libraries at the top, and initializes the SD card
reader/writer and the IMU in the `setup` function. Luckily we have really high
level functions for this. If you want to change the data collection resolution
of the IMU, you will have to edit the `initialize` function in the file
`MPU9150.cpp` in the same `arduino_code` folder.

If you compile and upload `mpu9150.ino` to the Arduino, with an SD card in the
reader, it should collect data from the MPU9150 and write it to the SD card.

It writes binary data to the SD card which needs to be unpacked. The
`binary_to_str.py` file in this repo will do that for you. Just run it from the
command line with the name of the data file as the first and only argument.

The data file should then contain lines with 7 space separated values. The first
value is the timestamp (in milliseconds since the program started) of data
collection. The next three numbers are the accelerometer measurements in the x,
y, and z directions, and the next three numbers are the gyrometer measurements
around the x, y and z axis.

These numbers have no units. According to the [MPU9150
datasheet](https://cdn.sparkfun.com/datasheets/Sensors/IMU/MPU-9150-Datasheet.pdf),
the accelerometer and gyrometer data are recorded as 16 bit binary numbers,
which must be scaled by the sensitivity scale factor provided in the datasheet
for each range setting. Check out `path_gen.md` for how that is done and other
useful things to do with the data.

Good luck!

### Troubleshooting

-   If you are having difficulty opening Arduino on your computer, check to see if your anti-virius software is blocking it


### Arduino Interface

-   Arduino makes an IDE that will handle libraries, compiling and uploading
    programs to the board. It's fine but kind of buggy and written in Java.
    Also, I prefer using one text editor for everything.
-   I use [this makefile] (https://github.com/sudar/Arduino-Makefile) for
    Arduino. [There is a tutorial here]
    (http://hardwarefun.com/tutorials/compiling-arduino-sketches-using-makefile)
    on how to use it. It's pretty straightforward and makes using Arduino a lot
    less painful.
