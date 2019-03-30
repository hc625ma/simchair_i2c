void setup_huey_coll_head()
{
  if (!is_device_connected(HUEY_HEAD_I2C_ADDRESS))
    return;

  COLLECTIVE_HOLD_BUTTON = HUEY_HEAD_COLLECTIVE_HOLD_BUTTON;
  simchair_aux2.setXAxisRange(0, 255);
  simchair_aux2.setYAxisRange(0, 255);
  dev_huey_coll_head = 1;
  IDLE_STOP_BUTTON = huey_coll_head_idle_stop_buttons[0];
  for (byte i=0;i<32;i++)
  {
    switch_matrix[i].sw_id = pgm_read_byte(&huey_switch_matrix[i].sw_id);
    switch_matrix[i].sw_type = pgm_read_byte(&huey_switch_matrix[i].sw_type);
    switch_matrix[i].sw_middle_b = pgm_read_byte(&huey_switch_matrix[i].sw_middle_b) - 1;
  }
}

void poll_huey_coll_head()
{

  uint8_t x, y, s0, s1;

  Wire.requestFrom(HUEY_HEAD_I2C_ADDRESS, 4);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();

    x = b2;
    y = b1;
    s0 = b3;
    s1 = b4;
  }


//  Serial.print(x);
//  Serial.print(" ");
//  Serial.print(y);
//  Serial.print(" ");
//  Serial.print(s0);
//  Serial.print(" ");
//  Serial.print(s1);
//  Serial.println();


  int16_t hat_val = parse_hat_sw(x, y,HUEY_COLL_HEAD_HAT_DIRECTIONS);
 // Serial.println(hat_val);

  simchair_aux2.setHatSwitch(0, hat_val);

  parse_coll_head_sw_matrix(s0,0,0);
  parse_coll_head_sw_matrix(s1,8,0);

}
