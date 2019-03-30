void setup_simple_collective()
{
  if (!is_device_connected(SIMPLE_COLLECTIVE_I2C_ADDRESS))
    return;

  simchair.setZAxisRange(SIMPLE_COLLECTIVE_MIN, SIMPLE_COLLECTIVE_MAX);
  simchair.setThrottleRange(SIMPLE_COLLECTIVE_THR_MIN, SIMPLE_COLLECTIVE_THR_MAX);
  dev_simple_collective = 1;
}

void poll_simple_collective()
{
  uint16_t z;
  uint16_t throttle;

  Wire.requestFrom(SIMPLE_COLLECTIVE_I2C_ADDRESS, 4);
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
