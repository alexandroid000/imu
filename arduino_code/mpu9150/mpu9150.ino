// read data from MPU9150 IMU and write to SD card using arduino pro micro
// Alexandra Nilles, fall 2015
// beerware

// sdfat library from: https://github.com/jbeynon/sdfatlib
// used (instead of default arduino SD libs) for binary read/write capabilities
#include <SPI.h>
#include "SdFat.h"
SdFat SD;
SdFile myFile;

// I2C device class (I2Cdev) demonstration Arduino sketch for MPU9150
// 1/4/2013 original by Jeff Rowberg <jeff@rowberg.net> at https://github.com/jrowberg/i2cdevlib
//          modified by Aaron Weiss <aaron@sparkfun.com>
//
// Creative Commons / MIT licenses
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "MPU9150.h"
MPU9150 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

const int chipSelect = 10;

int i;
unsigned long time;

// create buffer for reading/writing data
// 2*buf_size bytes currently, 512 max
// each set of data is seven 16-bit floating-point numbers (time + gyr + acc)
const size_t buf_size = 252;
uint16_t data[buf_size];


void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    TWBR = 12;

    // serial communication for debugging
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

    //  check SD initialization - chipSelect depends on board
    if (!SD.begin(chipSelect)) {
        return;
    }

    // open file - it's never actually closed but just using "sync" seems stable
    if (!myFile.open("data.txt", O_CREAT | O_WRITE | O_APPEND)) {
        return;
    }

    // initialize buffer with zeros
    for (i = 0; i < buf_size; i++) {
        data[i] = 0;
    }
}

void loop(void) {

    // read raw accel/gyro measurements from device
    // getMotion6 returns int16_t values
    for (i = 0; i < buf_size; i=i+7) {

        // record time data collection begins
        // time is 32-bits (means 65 second max data collection until rollover)
        time = millis();
        data[i] = time;

        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        data[i+1] = ax;
        data[i+2] = ay;
        data[i+3] = az;
        data[i+4] = gx;
        data[i+5] = gy;
        data[i+6] = gz;
    }

    myFile.write(data, sizeof(data));
    myFile.sync();

}
