void setup_cessna_engine_and_prop_controls()
{
  if (!is_device_connected(GA_CONTROLS_I2C_ADDRESS))
    return;

  simchair_aux1.setRxAxisRange(0, 1023);
  simchair_aux1.setRyAxisRange(0, 1023);
  simchair_aux1.setThrottleRange(0, 1023);
  dev_cessna_engine_and_prop_controls = 1;
}


void poll_cessna_engine_and_prop_controls()
{
  uint16_t rx, ry, throttle;

  Wire.requestFrom(GA_CONTROLS_I2C_ADDRESS, 6);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();
    byte b5 = Wire.read(); // receive a byte as character
    byte b6 = Wire.read();
    throttle = b1;
    throttle = (throttle << 8) | b2;
    rx = b3;
    rx = (rx << 8) | b4;
    ry = b5;
    ry = (ry << 8) | b6;
  }
  simchair.setRxAxis(rx);
  simchair.setRyAxis(ry);
  simchair.setThrottle(throttle);
}
