void setup_throttle_quadrant()
{
  if (!is_device_connected(THROTTLE_QUADRANT_I2C_ADDRESS))
    return;

  simchair_aux1.setRxAxisRange(THROTTLE_QUADRANT_MAIN_AXIS_TRESHOLD, THROTTLE_QUADRANT_PHYSICAL_AXIS_MIN);
  simchair_aux1.setRyAxisRange(THROTTLE_QUADRANT_MAIN_AXIS_TRESHOLD, THROTTLE_QUADRANT_PHYSICAL_AXIS_MIN);
  simchair_aux1.setThrottleRange(THROTTLE_QUADRANT_MAIN_AXIS_TRESHOLD, THROTTLE_QUADRANT_PHYSICAL_AXIS_MIN);
  simchair_aux1.setRzAxisRange(THROTTLE_QUADRANT_SECONDARY_AXIS_TRESHOLD, THROTTLE_QUADRANT_PHYSICAL_AXIS_MAX);
  simchair_aux1.setRudderRange(THROTTLE_QUADRANT_SECONDARY_AXIS_TRESHOLD, THROTTLE_QUADRANT_PHYSICAL_AXIS_MAX);
  simchair_aux1.setZAxisRange(THROTTLE_QUADRANT_SECONDARY_AXIS_TRESHOLD, THROTTLE_QUADRANT_PHYSICAL_AXIS_MAX);

  dev_throttle_quadrant = 1;
}


void poll_throttle_quadrant()
{
  uint16_t rx, ry, throttle;

  Wire.requestFrom(THROTTLE_QUADRANT_I2C_ADDRESS, 6);
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

//  Serial.print(rx);
//  Serial.print(" ");
//  Serial.print(ry);
//  Serial.print(" ");
//  Serial.print(throttle);
//  Serial.println(" ");



  if (rx < THROTTLE_QUADRANT_MAIN_AXIS_TRESHOLD)
  {
     simchair_aux1.setRxAxis(rx);
  }
  else if (rx > THROTTLE_QUADRANT_SECONDARY_AXIS_TRESHOLD)
  {
    simchair_aux1.setRzAxis(rx);

  }
  else
  {
    simchair_aux1.setRxAxis(THROTTLE_QUADRANT_MAIN_AXIS_TRESHOLD);
    simchair_aux1.setRzAxis(THROTTLE_QUADRANT_SECONDARY_AXIS_TRESHOLD);
  }

  if (ry < THROTTLE_QUADRANT_MAIN_AXIS_TRESHOLD)
  {
     simchair_aux1.setRyAxis(ry);
  }
  else if (ry > THROTTLE_QUADRANT_SECONDARY_AXIS_TRESHOLD)
  {
    simchair_aux1.setRudder(ry);

  }
  else
  {
    simchair_aux1.setRyAxis(THROTTLE_QUADRANT_MAIN_AXIS_TRESHOLD);
    simchair_aux1.setRudder(THROTTLE_QUADRANT_SECONDARY_AXIS_TRESHOLD);
  }

  if (throttle < THROTTLE_QUADRANT_MAIN_AXIS_TRESHOLD)
  {
     simchair_aux1.setThrottle(throttle);
  }
  else if (throttle > THROTTLE_QUADRANT_SECONDARY_AXIS_TRESHOLD)
  {
    simchair_aux1.setZAxis(throttle);

  }
  else
  {
    simchair_aux1.setThrottle(THROTTLE_QUADRANT_MAIN_AXIS_TRESHOLD);
    simchair_aux1.setZAxis(THROTTLE_QUADRANT_SECONDARY_AXIS_TRESHOLD);
  }




}
