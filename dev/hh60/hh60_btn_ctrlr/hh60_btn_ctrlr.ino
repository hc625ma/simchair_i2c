// WARNING! REMOVE LED ON PIN 13!
// CONNECT SWITCHES FROM A TOP LEFT CORNER (SLIDER), FROM LEFT TO RIGHT
// ORDER: 2 UPPER WIRES, 2 LOWER WIRES, BUTTON, TIE GNDs


#define HH60_BTN_CTRLR_I2C_ADDRESS 31 // do not change this!

#include <Wire.h>

#define INTERCOM_BUTTON_PIN   5
#define PTT_BUTTON_PIN        6
#define INTERCOM_PRESS_DURATION 130  // adjust to your liking, values below 100 are not recommended

unsigned long intercom_press_ts = 0;
bool ptt_pressed = false;

//#define DEBUG

uint16_t x,y;
uint8_t b = 0b00000000; //digital pins 0 to 7; x ^= (1 << n); - toggles nth bit of x.  all other bits left alone.
uint8_t b1 = 0b00000000;
uint8_t b2 = 0b00000000;
uint8_t buf = 0x00; 

int pins = 9;


byte data[2];

void setup() {
  Wire.begin(HH60_BTN_CTRLR_I2C_ADDRESS);                // join i2c bus with address #8
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
    b1 = read_btn(2);
 //   b2 = read_hat(7);

    #if defined(DEBUG)
      printBits(b1);
      Serial.print(" ");
//      printBits(b2);
//      Serial.print(" ");
//      printBits(b3);
      Serial.println();
    #endif
}

uint8_t read_btn (uint8_t start_pin)
{
  uint8_t b = 0b00000000;
  for (int i = start_pin; i < (start_pin + 8); i++) // hat takes 5 pins
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
  
  buf = b;
  buf &= ~(1 << (INTERCOM_BUTTON_PIN));

  if (b & (1 << INTERCOM_BUTTON_PIN)) // INTERCOM pressed
  {
    if (ptt_pressed == false) // PTT has not been pressed yet
    {
      if (intercom_press_ts == 0) // if we are pressing INTERCOM for the 1st time
      {
        intercom_press_ts = millis();
      }
      else
      {
        if ((millis() - intercom_press_ts) > INTERCOM_PRESS_DURATION) // if intercom is pressed for long enough we are sure we want it rather than PTT
        {
          if (!(b & (1 << PTT_BUTTON_PIN))) // and the PTT button is not pressed
          {
            buf |= (1 << (INTERCOM_BUTTON_PIN));  // press INTERCOM button

          }
          else
          {
            ptt_pressed = true;
          }
        }
      }
    }
  }
  else 
  {
    intercom_press_ts = 0;
    ptt_pressed = false;
  }

  return buf;
}

void requestEvent() {
  data[0] = b1;
//  data[1] = b2;
//  data[2] = b2;
  Wire.write(data,1);
}

void printBits(byte myByte){
 for(byte mask = 0x80; mask; mask >>= 1){
   if(mask  & myByte)
       Serial.print('1');
   else
       Serial.print('0');
 }
}
