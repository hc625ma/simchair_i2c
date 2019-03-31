#include <Wire.h>

#define SIMPLE_COLLECTIVE_I2C_ADDRESS 10 // do not change this!

/*
 * Uncomment #define CALIBRATE statement and change the following constants
 * to define the min and max values of your device.
 */
//#define CALIBRATE
#define COLLECTIVE_CALIBRATE_MIN  1002
#define COLLECTIVE_CALIBRATE_MAX  18
#define THROTTLE_CALIBRATE_MIN    10
#define THROTTLE_CALIBRATE_MAX    976

#define COLLECTIVE_FILTER_SAMPLES  6
#define THROTTLE_FILTER_SAMPLES    6

#define POT_BOARD_VCC_PIN  10
#define COLLECTIVE_PIN     A0
#define THROTTLE_PIN       A1

uint16_t collective;
uint16_t throttle;


void setup()
{
  Wire.begin(SIMPLE_COLLECTIVE_I2C_ADDRESS);
  Wire.onRequest(requestEvent);

  #if defined(CALIBRATE)
    Serial.begin(9600);
  #endif

  // Power up the pot board
  pinMode(POT_BOARD_VCC_PIN, OUTPUT);
  digitalWrite(POT_BOARD_VCC_PIN, HIGH);
}

void loop()
{
  collective = filteredRead(COLLECTIVE_PIN, COLLECTIVE_FILTER_SAMPLES);
  throttle = filteredRead(THROTTLE_PIN, THROTTLE_FILTER_SAMPLES);

  // Change the order of the last 2 values to invert the axis
  collective = map(collective, COLLECTIVE_CALIBRATE_MIN, COLLECTIVE_CALIBRATE_MAX, 0, 1023);
  throttle = map(throttle, THROTTLE_CALIBRATE_MIN, THROTTLE_CALIBRATE_MAX, 1023, 0);

  validate(collective);
  validate(throttle);

  #if defined(CALIBRATE)
    Serial.print(collective);
    Serial.print(" ");
    Serial.println(throttle);
  #endif
}

/*
 * Function that executes whenever data is received from master
 * this function is registered as an event, see setup()
 *
 * I2C communication protocol
 *
 * Byte 0: collective high nibble
 * Byte 1: collective low nibble
 * Byte 2: throttle high nibble
 * Byte 3: throttle low nibble
 */
void requestEvent() {
  uint8_t data[4];
  data[0] = (collective >> 8) & 0xFF;
  data[1] = collective & 0xFF;
  data[2] = (throttle >> 8) & 0xFF;
  data[3] = throttle & 0xFF;
  Wire.write(data, 4);
}

uint16_t filteredRead (uint8_t input, uint8_t samples)
{
  uint32_t filter = 0;

  for (uint8_t i = 0; i < samples; i++)
  {
      filter += analogRead(input);
      delay(1);
  }

  return filter / samples;
}

void validate(uint16_t& value)
{
  if (value > 1023)
  {
    value = 1023;
  }

  if (value > 30000)
  {
    value = 0;
  }
}
