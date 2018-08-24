#include <Wire.h>
uint16_t z,rz,ry;
byte data[6];

uint8_t filter_counter_z = 6;
uint8_t filter_counter_rz = 6;
uint8_t filter_counter_ry = 6;

void setup() 
{
  Wire.begin(15);                // join i2c bus with address #11
  Wire.onRequest(requestEvent); // register event
 // Serial.begin(9600);           // start serial for output
}

void loop() 
{
  // read axis, invert if needed
  z = filteredRead(A0,filter_counter_z);
  rz = filteredRead(A1,filter_counter_rz);
  ry = filteredRead(A2,filter_counter_ry);
  // we have 200 degree turn instead of 300, so a little adjustment is necessary
  // uncomment Serial.print statements and change 2nd and 3rd values to physical min and max
  // of your lever

// comment this to calibrate cyclic, then uncomment Serial.prints below and enter min and max values as 2 and 3 numbers
// you can swap 2 last numbers to invert the axis if needed
  z = map(z,0,1002,1023,0);
  rz = map(rz,125,1023,1023,0);
  ry = map(ry,114,1023,1023,0);
//
//  Serial.print(z); 
//  Serial.print(" ");
//  Serial.print(rz);
//  Serial.print(" ");
//  Serial.println(ry);
//  

}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void requestEvent() {
  data[0] = (z >> 8) & 0xFF;
  data[1] = z & 0xFF;
  data[2] = (rz >> 8) & 0xFF;
  data[3] = rz & 0xFF;
  data[4] = (ry >> 8) & 0xFF;
  data[5] = ry & 0xFF;
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

int invert (int val)
{
  val = 1023 - val;
  return val;
}

