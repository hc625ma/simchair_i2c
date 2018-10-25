// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads1115;
int16_t x,y;


byte data[4];

void setup() {
  //Wire.begin(10);                // join i2c bus with address #8
 // Wire.onRequest(requestEvent); // register event
//  Serial.begin(9600);           // start serial for output
 // ads1115.begin();
  //ads1115.setGain(GAIN_ONE);
  
}

void loop() 
{  
 // x = ads1115.readADC_SingleEnded(0) >> 3;
 // y = ads1115.readADC_SingleEnded(1) >> 3;

  
//Serial.println(x);
//Serial.println(y);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void requestEvent() {
  data[0] = (x >> 8) & 0xFF;
  data[1] = x & 0xFF;
  data[2] = (y >> 8) & 0xFF;
  data[3] = y & 0xFF;
  Wire.write(data,4);
}
