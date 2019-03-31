#include <Wire.h>

#define GA_CONTROLS_I2C_ADDRESS 11

#define POWER_POTS_PIN  10
#define THROTTLE_PIN    A0
#define PROPELLER_PIN   A1
#define MIXTURE_PIN     A2

#define THROTTLE_FILTER_SAMPLES   8
#define PROPELLER_FILTER_SAMPLES  8
#define MIXTURE_FILTER_SAMPLES    8

uint16_t throttle;
uint16_t propeller;
uint16_t mixture;

void setup() {
  Wire.begin(GA_CONTROLS_I2C_ADDRESS);
  Wire.onRequest(requestEvent);

  pinMode(POWER_POTS_PIN, OUTPUT);
  digitalWrite(POWER_POTS_PIN, HIGH);
}

void loop()
{
  throttle = filteredRead(THROTTLE_PIN, THROTTLE_FILTER_SAMPLES);
  propeller = filteredRead(PROPELLER_PIN, PROPELLER_FILTER_SAMPLES);
  mixture = filteredRead(MIXTURE_PIN, MIXTURE_FILTER_SAMPLES);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void requestEvent() {
  uint8_t data[6];
  data[0] = throttle >> 8;
  data[1] = throttle & 0xFF;
  data[2] = propeller >> 8;
  data[3] = propeller & 0xFF;
  data[4] = mixture >> 8;
  data[5] = mixture & 0xFF;
  Wire.write(data,6);
}

uint16_t filteredRead (uint16_t input, uint8_t samples)
{
  uint32_t filter = 0;

  for (uint8_t i = 0; i < samples; i++) {
      filter += analogRead(input);
      delay(1);
  }

  return filter / samples;
}
