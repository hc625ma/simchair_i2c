// Generic I2C device template

#define I2C_PERIPHERAL_I2C_ADDRESS 8 // change the address to a free one (look what's occupied in the master sketch)

#include <Wire.h>

//#define DEBUG

uint16_t x,y;
uint8_t b = 0b00000000; //digital pins 2 to 9; x ^= (1 << n); - toggles nth bit of x.  all other bits left alone.
byte pins[] = {2};


byte data[5];

void setup() {
  Wire.begin(I2C_PERIPHERAL_I2C_ADDRESS);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  #if defined(DEBUG)
    Serial.begin(9600);           // start serial for output
  #endif

  for (int i = 0; i < sizeof(pins); i++)
  {
    pinMode(pins[i], INPUT_PULLUP);
  }

}

void loop() {
  x = analogRead(A0);
  y = analogRead(A1);
  for (int i = 0; i < sizeof(pins); i++)
  {
    bool pin = !digitalRead(pins[i]);
    if (pin == 1)
    {
       b |= (1 << i);       // forces ith bit of b to be 1.  all other bits left alone.
    }
    else
    {
      b &= ~(1 << i);      // forces ith bit of b to be 0.  all other bits left alone.
    }

    #if defined(DEBUG)
      Serial.println(b);
    #endif
  }

  #if defined(DEBUG)
    Serial.println(x);
    Serial.println(y);
  #endif

}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void requestEvent() {
  data[0] = (x >> 8) & 0xFF;
  data[1] = x & 0xFF;
  data[2] = (y >> 8) & 0xFF;
  data[3] = y & 0xFF;
  data[4] = b;
  Wire.write(data,5);
 // Wire.write(y);
  // as expected by master
}
