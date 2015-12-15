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
SdFile myFile;

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


const int chipSelect = 10;

int i;
const int window_size = 1;
unsigned long time;

// create buffer for reading/writing data
// 7*16 bits currently, 512 max
uint16_t data[7];
uint16_t one = 1;

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

    for (uint16_t i = 0; i < 7; i++) {
        data[i] = 0;
    }

    // record time data collection begins
    // bitshift 32-bit time (means 65 second-max data collection until rollover)
    time = millis();
    data[0] = time;

    // read raw accel/gyro measurements from device
    // getMotion6 returns int16_t values
    for (i = 0; i < window_size; i++) {

        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        data[1] += ax;
        data[2] += ay;
        data[3] += az;
        data[4] += gx;
        data[5] += gy;
        data[6] += gz;
    }

    // straight up average over window
    // could make adaptive window size - don't average high delta data

    // tab-separated accel/gyro x/y/z values
    data[1] = data[1]/window_size;
    data[2] = data[2]/window_size;
    data[3] = data[3]/window_size;
    data[4] = data[4]/window_size;
    data[5] = data[5]/window_size;
    data[6] = data[6]/window_size;

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.

    // if the file opened okay, write to it:
    if (myFile.open("data.txt", O_CREAT | O_WRITE | O_APPEND)) {
        myFile.write(data, sizeof(data));
    }

    myFile.close();

}
