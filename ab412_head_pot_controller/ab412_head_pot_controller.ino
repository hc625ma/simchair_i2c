//Simchair MKIII AB412 collective head analog controller firmware. Reads 6 8-bit axes.

// CONNECT POTENTIOMETERS AS FOLLOWS:
// POT BOARD 1 VCC -> POT BOARD 2 VCC -> PIN 10
// POT BOARD 1 GND -> POT BOARD 2 GND -> GND
// POT BOARD 1 VRx -> A0
// POT BOARD 1 VRy -> A1
// POT BOARD 2 VRx -> A2
// POT BOARD 2 VRy -> A3
// POT BOARD 1 SW -> PIN 11
// POT BOARD 2 SW -> PIN 12
// ROTARY POT 1 VCC -> ROTARY POT 2 VCC -> PIN9
// ROTARY POT 1 GND -> ROTARY POT 2 GND -> GND
// ROTARY POT 1 SIGNAL -> A6
// ROTARY POT 2 SIGNAL -> A7

#define AB412_HEAD_POT_I2C_ADDRESS 14

#include <Wire.h>

uint8_t x1,y1,x2,y2,r1,r2;
uint8_t b = 0b00000000; //digital pins 2 to 9; x ^= (1 << n); - toggles nth bit of x.  all other bits left alone.

//MODE SWITCH GOES THERE!
byte pins[] = {11,12};

byte data[7]; //6 8 bit axes + 2 buttons in the 7th byte

void setup()
{
  pinMode(10, OUTPUT);           // power up pot boards
  digitalWrite(10, HIGH);
  pinMode(9, OUTPUT);           // power up pot boards
  digitalWrite(9, HIGH);
  Wire.begin(AB412_HEAD_POT_I2C_ADDRESS);                // join i2c bus with address #20
  Wire.onRequest(requestEvent); // register event
  for (int i = 0; i < sizeof(pins); i++)
  {
    pinMode(pins[i], INPUT_PULLUP);
  }
}

void loop()
{
  // Convert 10 bit ADC reading to 8 bit value to speed things up - see i2c_preipheral.ino if you want up to 16 bit values.
  //Keep in mind, smaller amount of data to transfer = smaller input lag, I would recommend using 10+ bit values for main controls only
  x1 = analogRead(A0) >> 2;
  y1 = analogRead(A1) >> 2;
  x2 = analogRead(A2) >> 2;
  y2 = analogRead(A3) >> 2;
  r1 = analogRead(A6) >> 2;
  r2 = analogRead(A7) >> 2;


  uint8_t buf = 0b00000000; // fill the buffer first so all our button values will update simultaneously and wont be affected by interrupts
  for (int i = 0; i < sizeof(pins); i++)
  {
    bool pin = !digitalRead(pins[i]);
    if (pin == 1)
    {
      buf |= (1 << i);       // forces ith bit of b to be 1.  all other bits left alone.
    }
    else
    {
      buf &= ~(1 << i);      // forces ith bit of b to be 0.  all other bits left alone.
    }
  }

  b = buf;
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void requestEvent()
{
  data[0] = x1;
  data[1] = y1;
  data[2] = x2;
  data[3] = y2;
  data[4] = r1;
  data[5] = r2;
  data[6] = b;
  Wire.write(data,7);
}
