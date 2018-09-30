void setup_ab412_coll_head()
{
  Wire.beginTransmission(13);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    simchair_aux2.setXAxisRange(0, 255);
    simchair_aux2.setYAxisRange(0, 255);
    simchair_aux2.setRxAxisRange(0, 255);
    simchair_aux2.setRyAxisRange(0, 255);
    simchair_aux2.setZAxisRange(0, 255);
    simchair_aux2.setRzAxisRange(0, 255);
    dev_ab412_coll_head = 1;
    for (int i = 0; i < sizeof(coll_head_triggerState); i++)
    {
      coll_head_triggerState[i] = 0;
    }
    parse_sw_array(ab412_sw_mode_button_switches, sizeof(ab412_sw_mode_button_switches), coll_head_sw_mode_button_switches_parsed);
    parse_sw_array(ab412_sw_mode_toggle_switches, sizeof(ab412_sw_mode_toggle_switches), coll_head_sw_mode_toggle_switches_parsed);
    parse_sw_array(ab412_sw_mode_selector_button_switches, sizeof(ab412_sw_mode_selector_button_switches), coll_head_sw_mode_selector_button_switches_parsed);
    parse_sw_array(ab412_sw_mode_selector_switches, sizeof(ab412_sw_mode_selector_switches), coll_head_sw_mode_selector_switches_parsed);
    

    for (int i = 0; i < sizeof(ab412_sw_mode_selector_button_switches); i++)
    {
      coll_head_lastButtonState[ab412_sw_mode_selector_button_switches[i]] = 1;
    }

//        for (int p = 0; p < sizeof(coll_head_sw_mode_button_switches_parsed); p++)
//        {
//          Serial.print(p);
//          Serial.print(" ");
//          Serial.println(coll_head_sw_mode_button_switches_parsed[p]);
//        }

  }
}

void poll_ab412_coll_head()
{
  uint8_t x, y, rx, ry, z, rz, b, s0, s1, s2;

  Wire.requestFrom(14, 7);
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


  Wire.requestFrom(13, 3);
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

  coll_head_parse_switches(b, 0, 2);
  coll_head_parse_switches(s0, 2, 0);
  coll_head_parse_switches(s1, 10, 0);
  coll_head_parse_switches(s2, 18, 0);



  simchair_aux2.setRxAxis(z);
  simchair_aux2.setRyAxis(rz);


}
