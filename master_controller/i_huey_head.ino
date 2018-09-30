void setup_huey_coll_head()
{

  Wire.beginTransmission(17);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    simchair_aux2.setXAxisRange(0, 255);
    simchair_aux2.setYAxisRange(0, 255);
    dev_huey_coll_head = 1;
    
    for (int i = 0; i < sizeof(coll_head_triggerState); i++)
    {
      coll_head_triggerState[i] = 0;
    }
    parse_sw_array(huey_sw_mode_button_switches, sizeof(huey_sw_mode_button_switches), coll_head_sw_mode_button_switches_parsed);
    parse_sw_array(huey_sw_mode_toggle_switches, sizeof(huey_sw_mode_toggle_switches), coll_head_sw_mode_toggle_switches_parsed);
    parse_sw_array(huey_sw_mode_selector_button_switches, sizeof(huey_sw_mode_selector_button_switches), coll_head_sw_mode_selector_button_switches_parsed);
    parse_sw_array(huey_sw_mode_selector_switches, sizeof(huey_sw_mode_selector_switches), coll_head_sw_mode_selector_switches_parsed);

    for (int i = 0; i < sizeof(huey_sw_mode_selector_button_switches); i++)
    {
      coll_head_lastButtonState[huey_sw_mode_selector_button_switches[i]] = 1;
    }

//          for (int p = 0; p < sizeof(huey_sw_mode_selector_button_switches_parsed); p++)
//        {
//          Serial.print(p);
//          Serial.print(" ");
//          Serial.println(huey_sw_mode_selector_button_switches_parsed[p]);
//        }

  }
}

void poll_huey_coll_head()
{
  
  uint8_t x, y, s0, s1;

  Wire.requestFrom(17, 4);
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

  coll_head_parse_switches(s0, 0, 0);
  coll_head_parse_switches(s1, 8, 0);

}
