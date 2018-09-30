void setup_simple_collective()
{
  Wire.beginTransmission(10);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    simchair.setZAxisRange(0, 1023);
    simchair.setThrottleRange(0, 1023);
    dev_simple_collective = 1;
  }
}

void poll_simple_collective()
{
  uint16_t z;
  uint16_t throttle;

  Wire.requestFrom(10, 4);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();

    z = b1;
    z = (z << 8) | b2;
    throttle = b3;
    throttle = (throttle << 8) | b4;

  }

  simchair.setZAxis(z);
  simchair.setThrottle(throttle);
}
