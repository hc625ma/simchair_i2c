#include <Wire.h>
uint16_t t1,t2,t3,t4,t5,t6;

byte data[6];
//uint16_t data[6]; // if 6 axes are active
uint8_t filter_counter_rx = 8;
uint8_t filter_counter_ry = 8;
uint8_t filter_counter_rz = 8;

void setup() {
  Wire.begin(18);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void loop() 
{
  t1 = filteredRead(A0,filter_counter_rx);
  t2 = filteredRead(A1,filter_counter_ry);
  t3 = filteredRead(A2,filter_counter_rz);
//  t4 = filteredRead(A3,filter_counter_rx);
//  t5 = filteredRead(A6,filter_counter_ry);
//  t6 = filteredRead(A7,filter_counter_rz);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void requestEvent() {
  data[0] = (t1 >> 8) & 0xFF;
  data[1] = t1 & 0xFF;
  data[2] = (t2 >> 8) & 0xFF;
  data[3] = t2 & 0xFF;
  data[4] = (t3 >> 8) & 0xFF;
  data[5] = t3 & 0xFF;
//  data[6] = (t4 >> 8) & 0xFF;
//  data[7] = t4 & 0xFF;
//  data[8] = (t5 >> 8) & 0xFF;
//  data[9] = t5 & 0xFF;
//  data[10] = (t6 >> 8) & 0xFF;
//  data[11] = t6 & 0xFF;

  Wire.write(data,6); //if 3 axes are active
  //  Wire.write(data,12); //if 6 axes are active
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
