void setup_ab412_coll_head()
{
  if (!is_device_connected(AB412_HEAD_I2C_ADDRESS))
    return;

  COLLECTIVE_HOLD_BUTTON = AB412_HEAD_COLLECTIVE_HOLD_BUTTON;
  simchair_aux2.setXAxisRange(0, 255);
  simchair_aux2.setYAxisRange(0, 255);
  simchair_aux2.setRxAxisRange(0, 255);
  simchair_aux2.setRyAxisRange(255, 0);
  simchair_aux2.setZAxisRange(0, 255);
  simchair_aux2.setRzAxisRange(0, 255);
  dev_ab412_coll_head = 1;

  for (byte i=0;i<32;i++)
  {
    switch_matrix[i].sw_id = pgm_read_byte(&ab412_switch_matrix[i].sw_id);
    switch_matrix[i].sw_type = pgm_read_byte(&ab412_switch_matrix[i].sw_type);
    switch_matrix[i].sw_middle_b = pgm_read_byte(&ab412_switch_matrix[i].sw_middle_b) - 1;
  }

  MODE_SWITCH_BUTTON = AB412_COLL_HEAD_MODE_SWITCH;
  IDLE_STOP_BUTTON = ab412_coll_head_idle_stop_buttons[0] - 1;
}

void poll_ab412_coll_head()
{
  uint8_t x, y, rx, ry, z, rz, b, s0, s1, s2;

  Wire.requestFrom(AB412_HEAD_POT_I2C_ADDRESS, 7);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();
    byte b5 = Wire.read(); // receive a byte as character
    byte b6 = Wire.read();
    byte b7 = Wire.read();

    x = b2;
    y = b1;
    rx = b4;
    ry = b3;
    z = b5;
    rz = b6;
    b = b7;
  }

  Wire.requestFrom(AB412_HEAD_I2C_ADDRESS, 3);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read();

    s0 = b1;
    s1 = b2;
    s2 = b3;
  }

  if (AB412_COLL_HEAD_ROTARY_POTS == 0)
  {
    z = 0;
    rz = 0;
  }
  else if (AB412_COLL_HEAD_ROTARY_POTS == 1)
  {
    rz = 0;
  }


  int16_t hat0_val = parse_hat_sw(x, y,AB412_COLL_HEAD_LEFT_HAT_DIRECTIONS);

  simchair_aux2.setHatSwitch(0, hat0_val);

  int16_t hat1_val = parse_hat_sw(rx, ry, AB412_COLL_HEAD_RIGHT_HAT_DIRECTIONS);
  simchair_aux1.setHatSwitch(0, hat1_val);

  parse_coll_head_sw_matrix(b,0,2);
  parse_coll_head_sw_matrix(s0,2,0);
  parse_coll_head_sw_matrix(s1,10,0);
  parse_coll_head_sw_matrix(s2,18,0);


  simchair_aux2.setRxAxis(z);
  simchair_aux2.setRyAxis(rz);


}
