//WARNING! REMOVE LED ON PIN 13!
//CONNECT SWITCHES FROM A TOP LEFT CORNER, FROM LEFT TO RIGHT IN ROWS
//FIRST ROW: 1st: PIN0,PIN1 2nd: PIN2,PIN3 3rd:  PIN4,PIN5
//SECOND ROW: 1st PIN6,PIN7 etc

#define HUEY_HEAD_I2C_ADDRESS 17

#include <Wire.h>

//#define DEBUG

uint8_t x,y;
uint8_t b = 0b00000000; //digital pins 0 to 7; x ^= (1 << n); - toggles nth bit of x.  all other bits left alone.
uint8_t b1 = 0b00000000; //digital pins 8 to 15
byte pins = 13;



byte data[4];

void setup() {
  Wire.begin(HUEY_HEAD_I2C_ADDRESS); // do not change this!
  Wire.onRequest(requestEvent); // register event
  #if defined(DEBUG)
    Serial.begin(9600);           // start serial for output
  #endif

  for (int i = 0; i <= pins; i++)
  {
    pinMode(i, INPUT_PULLUP);
  }

}

void loop()
{
  x = analogRead(A0) >> 2;
  y = analogRead(A1) >> 2;


  for (int i = 0; i <= pins; i++)
  {
    bool pin = !digitalRead(i);
    if (pin == 1)
    {
     if (i < 8)
     {
        b |= (1 << i);       // forces ith bit of b to be 1.  all other bits left alone.
     }
     else
     {
        b1 |= (1 <<  (i - 8));
     }
    }
    else
    {
      if ( i < 8)
      {
        b &= ~(1 << i);      // forces ith bit of b to be 0.  all other bits left alone.
      }
      else
      {
        b1 &= ~(1 << (i - 8));
      }
    }


    #if defined(DEBUG)
      printBits(b);
      Serial.print(" ");
      printBits(b1);
      Serial.print(" ");
      Serial.print(x);
      Serial.print(" ");
      Serial.print(y);
      Serial.println();
    #endif
 }

}

void requestEvent() {
  data[0] = x;
  data[1] = y;
  data[2] = b;
  data[3] = b1;
  Wire.write(data,4);
}

void printBits(byte myByte){
 for(byte mask = 0x80; mask; mask >>= 1){
   if(mask  & myByte)
       Serial.print('1');
   else
       Serial.print('0');
 }
}
