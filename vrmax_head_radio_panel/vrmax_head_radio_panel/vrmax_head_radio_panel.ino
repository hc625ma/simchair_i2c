#include <Wire.h>

byte data[5];

int encoder0PinA = 1;
int encoder0PinB = 2;
int encoder1PinA = 6;
int encoder1PinB = 7;
int encoder2PinA = 12;
int encoder2PinB = 13;
int encoder3PinA = 16;
int encoder3PinB = 17;

uint8_t b = 0b00000000;
uint8_t b1 = 0b00000000;
uint8_t b2 = 0b00000000;

int encoder0Pos = 0;
int encoder0PinALast = LOW;
int encoder1Pos = 0;
int encoder1PinALast = LOW;
int encoder2Pos = 0;
int encoder2PinALast = LOW;
int encoder3Pos = 0;
int encoder3PinALast = LOW;
int n0,n1,n2,n3 = LOW;
byte button_pins[] = {0,3,4,5,8,9,10,11,14,15};//button on pin 18 (A6) is connected to analog only pin!

void setup() {
  Wire.begin(19);                // join i2c bus with address #8
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
  
  Serial.begin (9600);
}

void loop() 
{
  for (int i = 0; i < sizeof(button_pins); i++)
  {
    bool pin = !digitalRead(button_pins[i]);
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
  }
  bool enc3_button = !analogRead(A6);
  if (enc3_button == 1)
  {
    b1 |= (1 << 2);
  }
  else
  {
    b1 &= ~(1 << 2); 
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


// DEBUG
//    printBits(b);
//    Serial.print(" ");
//    printBits(b1);
//    Serial.println(" ");
//  WARNING! TEST BUTTONS AND ENCODERS SEPARATELY!
//    Serial.print (encoder0Pos);
//    Serial.print(" ");
//    Serial.print (encoder1Pos);
//    Serial.print(" ");
//    Serial.print (encoder2Pos);
//    Serial.print(" ");
//    Serial.print (encoder3Pos);
//    Serial.println();
  
}

void requestEvent() 
{
  data[0] = b;
  data[1] = b1;
  data[2] = encoder0Pos;
  data[3] = encoder1Pos;
  data[4] = encoder2Pos;
  data[5] = encoder3Pos;
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





