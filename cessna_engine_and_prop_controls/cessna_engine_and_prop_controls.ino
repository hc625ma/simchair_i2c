#define GA_CONTROLS_I2C_ADDRESS 11

#include <Wire.h>
uint16_t rx,ry,rz;

byte data[6];

uint8_t filter_counter_rx = 8;
uint8_t filter_counter_ry = 8;
uint8_t filter_counter_rz = 8;

void setup() {
  pinMode(10, OUTPUT);           // power up pots
  digitalWrite(10, HIGH);
  Wire.begin(GA_CONTROLS_I2C_ADDRESS);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void loop()
{
  rx = filteredRead(A0,filter_counter_rx);
  ry = filteredRead(A1,filter_counter_ry);
  rz = filteredRead(A2,filter_counter_rz);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void requestEvent() {
  data[0] = (rx >> 8) & 0xFF;
  data[1] = rx & 0xFF;
  data[2] = (ry >> 8) & 0xFF;
  data[3] = ry & 0xFF;
  data[4] = (rz >> 8) & 0xFF;
  data[5] = rz & 0xFF;

  Wire.write(data,6);
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
