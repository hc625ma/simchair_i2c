#include <Wire.h>

//#define DEBUG // uncomment to test - set serial baud rate to 115200

byte data[5];

int encoder0PinA = 2;
int encoder0PinB = 3;
int encoder1PinA = 5;
int encoder1PinB = 6;
int encoder2PinA = 10;
int encoder2PinB = 11;
int encoder3PinA = 15;
int encoder3PinB = 16;

uint8_t b = 0b00000000;

uint8_t encoder0Pos = 0;
uint8_t encoder0PinALast = LOW;
uint8_t encoder1Pos = 0;
uint8_t encoder1PinALast = LOW;
uint8_t encoder2Pos = 0;
uint8_t encoder2PinALast = LOW;
uint8_t encoder3Pos = 0;
uint8_t encoder3PinALast = LOW;

int n0,n1,n2,n3 = LOW;
byte button_pins[] = {4,7,8,9,12,13,14,17};//button on pin 18 (A6) is connected to analog only pin!

void setup() {
  Wire.begin(21);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event

  
  for (int i = 0; i < sizeof(button_pins); i++)
  {
    pinMode(button_pins[i], INPUT_PULLUP);
  }

  pinMode (encoder0PinA,INPUT_PULLUP);
  pinMode (encoder0PinB, INPUT_PULLUP);

  pinMode (encoder1PinA,INPUT_PULLUP);
  pinMode (encoder1PinB, INPUT_PULLUP);

  pinMode (encoder2PinA,INPUT_PULLUP);
  pinMode (encoder2PinB, INPUT_PULLUP);

  pinMode (encoder3PinA,INPUT_PULLUP);
  pinMode (encoder3PinB, INPUT_PULLUP);

  #if defined(DEBUG)
    Serial.begin (115200);
  #endif
}

void loop() 
{
  for (int i = 0; i < sizeof(button_pins); i++)
  {
    bool pin = !digitalRead(button_pins[i]);
    if (pin == 1)
    {
      b |= (1 << i);       // forces ith bit of b to be 1.  all other bits left alone.
    }
    else
    {
      b &= ~(1 << i);      // forces ith bit of b to be 0.  all other bits left alone.
    }
  }
  
  
  n0 = !digitalRead(encoder0PinA);
  if ((encoder0PinALast == LOW) && (n0 == HIGH)) 
  {
    if (!digitalRead(encoder0PinB) == LOW) 
    {
      encoder0Pos--;
    } 
    else 
    {
      encoder0Pos++;
    }
   }
  encoder0PinALast = n0;

  n1 = !digitalRead(encoder1PinA);
  if ((encoder1PinALast == LOW) && (n1 == HIGH)) 
  {
    if (!digitalRead(encoder1PinB) == LOW) 
    {
      encoder1Pos--;
    } 
    else 
    {
      encoder1Pos++;
    }
   }
  encoder1PinALast = n1;
  
  n2 = !digitalRead(encoder2PinA);
  if ((encoder2PinALast == LOW) && (n2 == HIGH)) 
  {
    if (!digitalRead(encoder2PinB) == LOW) 
    {
      encoder2Pos--;
    } 
    else 
    {
      encoder2Pos++;
    }
   }
  encoder2PinALast = n2;
  
  n3 = !digitalRead(encoder3PinA);
  if ((encoder3PinALast == LOW) && (n3 == HIGH)) 
  {
    if (!digitalRead(encoder3PinB) == LOW) 
    {
      encoder3Pos--;
    } 
    else 
    {
      encoder3Pos++;
    }
   }
  encoder3PinALast = n3;
  
 

#if defined(DEBUG)
    printBits(b);
    Serial.print(" ");

    Serial.print (encoder0Pos);
    Serial.print(" ");
    Serial.print (encoder1Pos);
    Serial.print(" ");
    Serial.println(encoder2Pos);
#endif


}

void requestEvent() 
{
  data[0] = b;
  data[1] = encoder0Pos;
  data[2] = encoder1Pos;
  data[3] = encoder2Pos;
  data[4] = encoder3Pos;
  Wire.write(data,5);
}

void printBits(byte myByte)
{
   for(byte mask = 0x80; mask; mask >>= 1)
   {
   if(mask  & myByte)
       Serial.print('1');
   else
       Serial.print('0');
   }
}





