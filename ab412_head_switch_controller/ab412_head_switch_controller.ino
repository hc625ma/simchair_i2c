/*
 * Connect switches from top left corner, from left to right, in rows:
 * First row: 1st: PIN0, PIN1 - 2nd: PIN2, PIN3 - 3rd: PIN4, PIN5
 * Second row: 1st PIN6, PIN7 - etc.
 *
 * Total 16 pins
 */

#include <Wire.h>

//#define DEBUG

#define AB412_HEAD_I2C_ADDRESS 13 // do not change this!

uint32_t buttons;

void setup()
{
  #if defined(DEBUG)
    Serial.begin(9600);
  #endif

  Wire.begin(AB412_HEAD_I2C_ADDRESS);
  Wire.onRequest(requestEvent);

  // Set all the digital pins as input pullup
  DDRB = 0x00;  // set pins as inputs
  DDRC = 0x00;
  DDRD = 0x00;
  PORTB = 0xFF; // enable interal pullup resistors
  PORTC = 0xFF;
  PORTD = 0xFF;
}

void loop()
{
  // The buttons are connected to pins with internal pullup resistors, so we
  // have to invert the logic, so that a pin 0 means a logical 1 (button pressed).
  uint32_t pins_0_7 = ~PIND;
  uint32_t pins_8_13 = ~PINB & 0b00111111;
  uint32_t pins_14_16 = ~PINC & 0b00000111;
  buttons = (pins_14_16 << 14) | (pins_8_13 << 8) | pins_0_7;

  #if defined(DEBUG)
    printBits(buttons);
    Serial.println();
  #endif
}

void requestEvent()
{
  uint8_t data[3];
  data[0] = buttons & 0xFF;
  data[1] = (buttons >> 8) & 0xFF;
  data[2] = (buttons >> 16) & 0xFF;
  Wire.write(data,3);
}

#if defined(DEBUG)
void printBits(uint8_t bits) {
  for(uint8_t mask = 0x8000; mask; mask >>= 1) {
    if(mask & bits)
      Serial.print('1');
    else
      Serial.print('0');
  }
}
#endif
