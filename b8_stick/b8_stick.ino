//Simchair MKIII B8 stick firmware. The stick has 6 buttons and 2 8 bit analog axes.

// CONNECT BUTTONS AS FOLLOWS:
// POT BUTTON - PIN 2 TO GND
// TOP (TRIMMER) - PIN 3 TO GND
// MIDDLE (FIRE) - PIN 4 TO GND
// BOTTOM - PIN 5 TO GND
// TRIGGER INTERCOM - PIN 6 TO GND
// TRIGGER PTT (full press) - PIN 7 TO GND

#include <Wire.h>

//INTERCOM (trigger half-press)
#define INTERCOM_PIN 6
#define INTERCOM_PRESS_DURATION 130 //adjust to your liking, values below 100 are not recommended
long intercom_pressed = -1;
#define PTT_PIN 7

uint8_t x,y;
uint8_t b = 0b00000000; //digital pins 2 to 9; x ^= (1 << n); - toggles nth bit of x.  all other bits left alone.

//INTERCOM TRIGGER PIN IS DEFINED SEPARATELY!
byte pins[] = {2,3,4,5};

byte data[3]; //2 8 bit axes + 5 buttons in the third byte

void setup() 
{
  pinMode(10, OUTPUT);           // power up the pot board
  digitalWrite(10, HIGH);
  Wire.begin(20);                // join i2c bus with address #20
  Wire.onRequest(requestEvent); // register event
  for (int i = 0; i < sizeof(pins); i++)
  {
    pinMode(pins[i], INPUT_PULLUP);
  }
  pinMode(INTERCOM_PIN, INPUT_PULLUP);
  pinMode(PTT_PIN, INPUT_PULLUP);
}

void loop() 
{
  // Convert 10 bit ADC reading to 8 bit value to speed things up - see i2c_preipheral.ino if you want up to 16 bit values.
  //Keep in mind, smaller amount of data to transfer = smaller input lag, I would recommend using 10+ bit values for main controls only
  x = analogRead(A1) >> 2; 
  y = analogRead(A0) >> 2;
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
  
  bool intercom = !digitalRead(INTERCOM_PIN);
  bool ptt = !digitalRead(PTT_PIN);
  if (ptt == 1)
  {
    buf |= (1 << 4);
  }
  else
  {
    buf &= ~(1 << 4);
  }
  
  if (intercom == 1)
  {
    if (intercom_pressed == -1)
    {
      intercom_pressed = millis();
    }
    else
    {
      if ((millis() - intercom_pressed) > INTERCOM_PRESS_DURATION)
      {
        if (ptt == 0)
        {
          buf |= (1 << 5); // press button 6
        }
        else
        {
          intercom_pressed = -1;
          buf &= ~(1 << 5); //release button 6
        }
      }
    }
  }
  else if (intercom == 0)
  {
    intercom_pressed = -1;
    buf &= ~(1 << 5); 
  }

  b = buf;
} 

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void requestEvent() {
  data[0] = x;
  data[1] = y;
  data[2] = b;
  Wire.write(data,3);
}
