/**
 * Simchair MKIII B8 stick firmware. The stick has 6 buttons and 2 8-bit analog
 * axes.
 *
 * Connect buttons as follow:
 *     POT BOARD VCC to PIN 10
 *     POT BOARD GND -> Arduino GND
 *     POT VRx -> A0
 *     POT VRy -> A1
 *     POT BOARD BUTTON – PIN 2
 *     TOP (TRIMMER) – between PIN 3 and GND
 *     MIDDLE (FIRE) – between PIN 4 and GND
 *     BOTTOM – between PIN 5 and GND
 *     TRIGGER INTERCOM – between PIN 6 and GND
 *     TRIGGER PTT (full press) – between PIN 7 and GND
 *     A4 (SDA) -> ethernet cable blue wire
 *     A5 (SCL) -> ethernet cable green-white wire
 *     VCC -> ethernet cable orange-white wire
 *     GND -> ethernet cable orange wire
 */

#include <Wire.h>

//#define GRIP_TEST

#define B8STICK_I2C_ADDRESS 20

#define POT_BOARD_BUTTON_PIN  2
#define TOP_BUTTON_PIN        3
#define MIDDLE_BUTTON_PIN     4
#define BOTTOM_BUTTON_PIN     5
#define INTERCOM_BUTTON_PIN   6
#define PTT_BUTTON_PIN        7

#define INTERCOM_PRESS_DURATION 130  // adjust to your liking, values below 100 are not recommended
unsigned long intercom_press_ts = 0;

uint8_t pot_x_value;     // hat potentiometer X axis reading
uint8_t pot_y_value;     // hat potentiometer Y axis reading
uint8_t buttons = 0x00;  // bit array with the state of the buttons connected
                         // to digital pins 2 to 7 (0 = released, 1 = pressed)
uint8_t buf = 0x00;    
bool ptt_press = 0;

void setup()
{
  #if (defined GRIP_TEST)
    Serial.begin(9600);
  #endif
  Wire.begin(B8STICK_I2C_ADDRESS);
  Wire.onRequest(requestEvent);

  // Power up the pot board
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  /*
   * Setup button pins as input pullup.
   *
   * As the buttons are physically connected to pins 2 to 7, all of them
   * belong to PORTD. Thus, all those pins can be set as input pullups at the
   * same time, directly manipulating the PORTD register.
   * In order to do this first the direction of the pins has to be defined as
   * input, writing a 0 to bits 2..7 of DDRD register. Then the output is put
   * to HIGH to enable pullup resistors, writing a 1 to 2..7 bits of PORTD register.
   */
  DDRD &= ~0b11111110;  // set pins 2..7 of port D as inputs, writing a 0 in the corresponding bits
  PORTD |= 0b11111110;  // activate pullup resistors of 2..7 pins, writing a 1 in the corresponding bits
}

void loop()
{
  /*
   * Convert 10 bit ADC reading to 8 bit value to speed things up.
   * See i2c_preipheral.ino if you want up to 16 bit values.
   * Keep in mind, smaller amount of data to transfer = smaller input lag.
   * I would recommend using 10+ bit values for main controls only.
   */
  pot_x_value = analogRead(A1) >> 2;
  pot_y_value = analogRead(A0) >> 2;

  /*
   * Read all the button pins in one single instruction. This avoids using
   * Arduino's digitalRead(), that is well known for being veeeeery slow
   * compared to direct port register manipulation.
   *
   * After reading the D port pins, as the first button is connected to pin 2,
   * everything is moved 2 bits to the right, to build the byte that will
   * be transmitted by I2C (which is 0 based).
   */
  buttons = ~PIND >> 2;
  buf = buttons; // we need some processing done over INTERCOM and PTT buttons, so this is needed to avoid sending raw values
  buf &= ~(1 << (INTERCOM_BUTTON_PIN - 2)); // intercom button is always depressed by default

  /*
   * The trigger has two positions: half-press (intercom) and full-press
   * with a click (radio push to talk). When the trigger is fully pressed,
   * a half-press button is disabled by the software, so your sim will see
   * these two positions as entirely separate buttons.
   * 
   * We will only press intercom when we are sure we actually want it, because some games
   * will only detect the 1st button press and for some reason will ignore the second one,
   * e.g. ARMA3.
   */

  if (buttons & (1 << (INTERCOM_BUTTON_PIN - 2))) // intercom pressed
  {
    if (ptt_press == 0) // PTT has not been pressed yet
    {
      if (intercom_press_ts == 0) // if we are pressing INTERCOM for the 1st time
      {
        intercom_press_ts = millis();
      }
      else
      {
        if ((millis() - intercom_press_ts) > INTERCOM_PRESS_DURATION) // if intercom is pressed for long enough we are sure we want it rather than PTT
        {
          if (!(buttons & (1 << (PTT_BUTTON_PIN - 2)))) // and the PTT button is not pressed
          {
            buf |= (1 << (INTERCOM_BUTTON_PIN - 2));  // press INTERCOM button
          }
          else
          {
            ptt_press = 1;
          }
        }
      }
    }
  }
  else 
  {
    intercom_press_ts = 0;
    ptt_press = 0;
  }

  #if (defined GRIP_TEST)
    printBits(buf);
  #endif
   
}

/**
 * I2C communication protocol
 *
 * Byte 1: hat potentiometer X value (8-bit resolution)
 * Byte 2: hat potentiometer Y value (8-bit resolution)
 * Byte 3: bit array with the state of the buttons (0 = released, 1 = pressed)
 *     - Bit 0: pot board button
 *     - Bit 1: top button
 *     - Bit 2: middle button
 *     - Bit 3: bottom button
 *     - Bit 4: intercom button
 *     - Bit 5: ptt button
 */

void printBits(byte myByte)
{
  for(byte mask = 0x80; mask; mask >>= 1)
  {
    if(mask  & myByte)
        Serial.print('1');
    else
        Serial.print('0');
  }
  Serial.println();
}
 
void requestEvent() 
{
  uint8_t data[3];
  data[0] = pot_x_value;
  data[1] = pot_y_value;
  data[2] = buf;
  Wire.write(data, 3);
}
