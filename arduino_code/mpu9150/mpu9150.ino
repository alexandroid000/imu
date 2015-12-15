// I2C device class (I2Cdev) demonstration Arduino sketch for MPU9150
// 1/4/2013 original by Jeff Rowberg <jeff@rowberg.net> at https://github.com/jrowberg/i2cdevlib
//          modified by Aaron Weiss <aaron@sparkfun.com>
//
// Changelog:
//     2011-10-07 - initial release
//     2013-1-4 - added raw magnetometer output

/* ============================================
I2Cdev device library code is placed under the MIT license

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

/*
  SD card read/write taken from example in Arduino library, in public domain

 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 */

#include <SPI.h>
#include "SdFat.h"
SdFat SD;

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "MPU9150.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU9150 accelgyro;

int16_t ax, ay, az;
int16_t ax_avg, ay_avg, az_avg;
int16_t gx, gy, gz;
int16_t gx_avg, gy_avg, gz_avg;

File myFile;

const int chipSelect = 10;

int i;
const int window_size = 1;
unsigned long time;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    TWBR = 12;

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
//    Serial.begin(38400);
//    while (!Serial) {
//      ;
//    }

    // initialize device
    accelgyro.initialize();


    //  check IMU initialization
    if (!accelgyro.testConnection()) {
        return;
    }

    //  check SD initialization
    if (!SD.begin(chipSelect)) {
        return;
    }


}

void loop(void) {

    ax_avg = 0;
    ay_avg = 0;
    az_avg = 0;
    gx_avg = 0;
    gy_avg = 0;
    gz_avg = 0;

    time = millis();

    // read raw accel/gyro measurements from device
    for (i = 0; i < window_size; i++) {
        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        ax_avg += ax;
        ay_avg += ay;
        az_avg += az;
        gx_avg += gx;
        gy_avg += gy;
        gz_avg += gz;
    }

    // straight up average over window
    // could make adaptive window size - don't average high delta data
    ax_avg = ax_avg/window_size;
    ay_avg = ay_avg/window_size;
    az_avg = az_avg/window_size;
    gx_avg = gx_avg/window_size;
    gy_avg = gy_avg/window_size;
    gz_avg = gz_avg/window_size;

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    myFile = SD.open("data.txt", O_CREAT | O_WRITE | O_APPEND);

    // if the file opened okay, write to it:
    if (myFile) {

        // tab-separated accel/gyro x/y/z values
        myFile.print(time); myFile.print("\t");
        myFile.print(ax_avg); myFile.print("\t");
        myFile.print(ay_avg); myFile.print("\t");
        myFile.print(az_avg); myFile.print("\t");
        myFile.print(gx_avg); myFile.print("\t");
        myFile.print(gy_avg); myFile.print("\t");
        myFile.println(gz_avg); myFile.print("\t");
        // close the file:
    }

    myFile.close();

}
