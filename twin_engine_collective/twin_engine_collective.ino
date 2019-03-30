#include <Wire.h>

//#define CALIBRATE

uint16_t z,rz,ry;
byte data[6];

uint8_t filter_counter_z = 6;
uint8_t filter_counter_rz = 6;
uint8_t filter_counter_ry = 6;

void setup()
{
  Wire.begin(15);                // join i2c bus with address #11
  Wire.onRequest(requestEvent); // register event
  #if defined(CALIBRATE)
    Serial.begin(9600);           // start serial for output
  #endif
}

void loop()
{
  // read axis, invert if needed
  z = filteredRead(A0,filter_counter_z);
  rz = filteredRead(A1,filter_counter_rz);
  ry = filteredRead(A2,filter_counter_ry);
  // we have 200 degree turn instead of 300, so a little adjustment is necessary
  // uncomment #define CALIBRATE statement and change 2nd and 3rd values to physical min and max
  // of your lever.
  // You can swap 2 last numbers to invert the axis if needed
  #if !defined(CALIBRATE)
    z = map(z,0,1002,1023,0);
    rz = map(rz,125,1023,1023,0);
    ry = map(ry,114,1023,1023,0);
  #endif

  #if defined(CALIBRATE)
    Serial.print(z);
    Serial.print(" ");
    Serial.print(rz);
    Serial.print(" ");
    Serial.println(ry);
  #endif
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void requestEvent() {
  // copy values so they don't get modified while we're writing them down the wire!
  uint16_t z_copy,rz_copy,ry_copy;
  z_copy = z;
  //z_copy = filteredRead(A0,filter_counter_z); // send actual value instead of mapped value, uncomment and comment above line to calibrate
  rz_copy = rz;
  ry_copy = ry;
  data[0] = (z_copy >> 8) & 0xFF;
  data[1] = z_copy & 0xFF;
  data[2] = (rz_copy >> 8) & 0xFF;
  data[3] = rz_copy & 0xFF;
  data[4] = (ry_copy >> 8) & 0xFF;
  data[5] = ry_copy & 0xFF;
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
