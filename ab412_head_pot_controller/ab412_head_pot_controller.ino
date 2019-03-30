/*
 * Simchair MKIII AB412 collective head analog controller firmware. Reads 6 8-bit axes.
 *
 * Connect potentiometers as follows:
 *
 * POT BOARD 1 VCC -> POT BOARD 2 VCC -> PIN 10
 * POT BOARD 1 GND -> POT BOARD 2 GND -> GND
 * POT BOARD 1 VRx -> A0
 * POT BOARD 1 VRy -> A1
 * POT BOARD 2 VRx -> A2
 * POT BOARD 2 VRy -> A3
 * POT BOARD 1 SW -> PIN 11
 * POT BOARD 2 SW -> PIN 12
 * ROTARY POT 1 VCC -> ROTARY POT 2 VCC -> PIN9
 * ROTARY POT 1 GND -> ROTARY POT 2 GND -> GND
 * ROTARY POT 1 SIGNAL -> A6
 * ROTARY POT 2 SIGNAL -> A7
 */

#include <Wire.h>

#define AB412_HEAD_POT_I2C_ADDRESS 14

#define ROTARY_POTS_VCC_PIN      9
#define POT_BOARDS_VCC_PIN       10
#define POT_BOARD_1_SW_PIN       11
#define POT_BOARD_2_SW_PIN       12
#define POT_BOARD_1_VRX_PIN      A0
#define POT_BOARD_1_VRY_PIN      A1
#define POT_BOARD_2_VRX_PIN      A2
#define POT_BOARD_2_VRY_PIN      A3
#define ROTARY_POT_1_SIGNAL_PIN  A6
#define ROTARY_POT_2_SIGNAL_PIN  A7


uint8_t pot_board_1_vrx;
uint8_t pot_board_1_vry;
uint8_t pot_board_2_vrx;
uint8_t pot_board_2_vry;
uint8_t rotary_pot_1_signal;
uint8_t rotary_pot_2_signal;
uint8_t buttons = 0x00;

void setup()
{
  Wire.begin(AB412_HEAD_POT_I2C_ADDRESS);
  Wire.onRequest(requestEvent);

  // Power up pot boards
  pinMode(POT_BOARDS_VCC_PIN, OUTPUT);
  digitalWrite(POT_BOARDS_VCC_PIN, HIGH);

  // Power up rotary pot boards
  pinMode(ROTARY_POTS_VCC_PIN, OUTPUT);
  digitalWrite(ROTARY_POTS_VCC_PIN, HIGH);

  // Setup pot boards button's pins as input pullups
  pinMode(POT_BOARD_1_SW_PIN, INPUT_PULLUP);
  pinMode(POT_BOARD_2_SW_PIN, INPUT_PULLUP);
}

void loop()
{
  /*
   * Convert 10 bit ADC reading to 8 bit value to speed things up.
   * See i2c_preipheral.ino if you want up to 16 bit values.
   * Keep in mind, smaller amount of data to transfer = smaller input lag,
   * I would recommend using 10+ bit values for main controls only.
   */
  pot_board_1_vrx = analogRead(POT_BOARD_1_VRX_PIN) >> 2;
  pot_board_1_vry = analogRead(POT_BOARD_1_VRY_PIN) >> 2;
  pot_board_2_vrx = analogRead(POT_BOARD_2_VRX_PIN) >> 2;
  pot_board_2_vry = analogRead(POT_BOARD_2_VRY_PIN) >> 2;
  rotary_pot_1_signal = analogRead(ROTARY_POT_1_SIGNAL_PIN) >> 2;
  rotary_pot_2_signal = analogRead(ROTARY_POT_2_SIGNAL_PIN) >> 2;

  /*
   * The two buttons on the pot boards are connected to Arduino's digital
   * pins 11 and 12. This are AVR pins 3 and 4 of port B.
   * With the help of direct port manipulation, the state of these pins
   * can be read with a few instructions, without the need to iterate with
   * a for loop.
   * The read value has to be inverted, because the buttons are connected to
   * pins with the internal pullup resistors enabled.
   */
   uint8_t mask = (1 << (POT_BOARD_1_SW_PIN - 8)) | (1 << (POT_BOARD_2_SW_PIN - 8));
   buttons = (~PINB & mask) >> 4;
}

/**
 * This function executes whenever data is received from master.
 * This function is registered as an event, see setup().
 *
 * I2C communication protocol
 *
 * Byte 0: X axis reading of the pot board 1 (8-bit resolution)
 * Byte 1: Y axis reading of the pot board 1 (8-bit resolution)
 * Byte 2: X axis reading of the pot board 2 (8-bit resolution)
 * Byte 3: Y axis reading of the pot board 2 (8-bit resolution)
 * Byte 4: X axis reading of the rotary pot 1 signal (8-bit resolution)
 * Byte 5: Y axis reading of the rotary pot 2 signal (8-bit resolution)
 * Byte 6: bit array with state of the buttons (0 = released, 1 = pressed)
 *     - Bit 0: pot board 1 button
 *     - Bit 1: pot board 1 button
 */
void requestEvent()
{
  uint8_t data[7]; //6 8 bit axes + 2 buttons in the 7th byte
  data[0] = pot_board_1_vrx;
  data[1] = pot_board_1_vry;
  data[2] = pot_board_2_vrx;
  data[3] = pot_board_2_vry;
  data[4] = rotary_pot_1_signal;
  data[5] = rotary_pot_2_signal;
  data[6] = buttons;
  Wire.write(data,7);
}
