void setup_cessna_engine_and_prop_controls()
{
  Wire.beginTransmission(11);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    simchair_aux1.setRxAxisRange(0, 1023);
    simchair_aux1.setRyAxisRange(0, 1023);
    simchair_aux1.setThrottleRange(0, 1023);
    dev_cessna_engine_and_prop_controls = 1;
  }
}

void parse_sw_array (byte sw_arr[], byte sw_arr_size, bool parsed_sw_arr[])
{
  byte z = 0;
  for (int i = 0; i < sw_arr_size; i++)
  {
    for (int k = z; k <= sw_arr[i]; k++)
    {
      if (k == (sw_arr[i] - 1))
      {
        parsed_sw_arr[k] = 1;
      }
      else
      {
        parsed_sw_arr[k] = 0;
      }
    }
    z = sw_arr[i];
  }
}

void poll_cessna_engine_and_prop_controls()
{
  uint16_t rx, ry, throttle;

  Wire.requestFrom(11, 6);
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
