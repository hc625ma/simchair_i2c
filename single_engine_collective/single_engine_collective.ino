// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

#define SINGLE_COLLECTIVE_I2C_ADDRESS 12

#include <Wire.h>
uint16_t z,rz;
byte data[4];

uint8_t filter_counter_z = 6;
uint8_t filter_counter_rz = 6;

void setup()
{
  pinMode(10, OUTPUT);           // power up the pot board
  digitalWrite(10, HIGH);
  Wire.begin(SINGLE_COLLECTIVE_I2C_ADDRESS);                // join i2c bus with address #11
  Wire.onRequest(requestEvent); // register event
//  Serial.begin(9600);           // start serial for output
}

void loop()
{
  z = filteredRead(A0,filter_counter_z);
  rz = filteredRead(A1,filter_counter_rz);
  // we have 200 degree turn instead of 300, so a little adjustment is necessary
  // uncomment Serial.print statements and change 2nd and 3rd values to physical min and max
  // of your lever


  z = map(z,14,1003,1023,0);
  rz = map(rz,0,992,1023,0);
//
//  Serial.print(z);
//  Serial.print(" ");
//  Serial.println(rz);


}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void requestEvent() {
  data[0] = (z >> 8) & 0xFF;
  data[1] = z & 0xFF;
  data[2] = (rz >> 8) & 0xFF;
  data[3] = rz & 0xFF;
  Wire.write(data,4);
}

uint16_t filteredRead (uint16_t input,uint8_t filter_counter)
{
  uint32_t filter = 0;
  for (uint8_t i=0;i<filter_counter;i++)
  {
      filter+= analogRead(input);
      delay(1);
  }

  uint16_t val = filter/filter_counter;
  return val;

}
