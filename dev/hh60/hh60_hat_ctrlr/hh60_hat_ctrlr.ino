// WARNING! REMOVE LED ON PIN 13!
// CONNECT SWITCHES FROM A TOP LEFT CORNER (SLIDER), FROM LEFT TO RIGHT
// ORDER: 2 UPPER WIRES, 2 LOWER WIRES, BUTTON, TIE GNDs


#define HH60_HAT_CTRLR_I2C_ADDRESS 30 // do not change this!

#include <Wire.h>

//#define DEBUG

uint16_t x,y;
uint8_t b = 0b00000000; //digital pins 0 to 7; x ^= (1 << n); - toggles nth bit of x.  all other bits left alone.
uint8_t b1 = 0b00000000;
uint8_t b2 = 0b00000000;
uint8_t b3 = 0b00000000;

int pins = 16;


byte data[2];

void setup() {
  Wire.begin(HH60_HAT_CTRLR_I2C_ADDRESS);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  #if defined(DEBUG)
    Serial.begin(115200);           // start serial for output
  #endif
  for (int i = 2; i <= pins; i++)
  {
    pinMode(i, INPUT_PULLUP);
  }

}

void loop()
{
    b1 = read_hat(2);
    b2 = read_hat(7);
    b3 = read_hat(12);

    #if defined(DEBUG)
      printBits(b1);
      Serial.print(" ");
      printBits(b2);
      Serial.print(" ");
      printBits(b3);
      Serial.println();
    #endif
}

uint8_t read_hat (uint8_t start_pin)
{
  uint8_t b = 0b00000000;
  for (int i = start_pin; i < (start_pin + 5); i++) // hat takes 5 pins
  {
    bool pin = !digitalRead(i);
    if (pin == 1)
    {
      b |= (1 << (i % 8)); // forces ith bit of b to be 1.  all other bits left alone.
    }
    else
    {
      b &= ~(1 << (i % 8)); // forces ith bit of b to be 0.  all other bits left alone.
    }
  }
  return b;
}

void requestEvent() {
  data[0] = b1;
  data[1] = b2;
  data[2] = b3;
  Wire.write(data,3);
}

void printBits(byte myByte){
 for(byte mask = 0x80; mask; mask >>= 1){
   if(mask  & myByte)
       Serial.print('1');
   else
       Serial.print('0');
 }
}
