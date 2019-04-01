void setup_vrmax_panel()
{
  if (!is_device_connected(VRMAX_I2C_ADDRESS))
    return;
  byte b0,b1,b2;
  dev_vrmax_panel = 1;
  read_bytes_from_vrmax_panel(&b0,&b1,&b2);
  for (byte i = 0; i < 7; i++)
  {
    e_state[i].press_counter = 0;
    e_state[i].last_val = e_state[i].val;
  }
}

void read_bytes_from_vrmax_panel(byte* but0, byte* but1, byte* but2)
{
//  uint8_t but0,but1,but2;
  Wire.requestFrom(22, 5); //radio panel - top left encoder
  while (Wire.available())
  {
    byte b1 = Wire.read(); // buttons #1
    byte b2 = Wire.read(); // buttons #2
    byte b3 = Wire.read(); // encoder 1
    byte b4 = Wire.read(); // encoder 2
    byte b5 = Wire.read(); // encoder 3

    *but0 = b1;
    *but1 = b2;
    e_state[0].val = b3;
    e_state[1].val = b4;
    e_state[2].val = b5;
  }

  Wire.requestFrom(21, 5); // side panel + radio panel top left encoder
  while (Wire.available())
  {
    byte b1 = Wire.read(); // buttons
    byte b2 = Wire.read(); // encoder 1
    byte b3 = Wire.read(); // encoder 2
    byte b4 = Wire.read(); // encoder 3
    byte b5 = Wire.read(); // encoder 4

    *but2 = b1;
    
    e_state[3].val = b5;
    e_state[4].val = b3;
    e_state[5].val = b4;
    e_state[6].val = b2;
  }

  return;
}

void poll_vrmax_panel()
{
  uint8_t but0,but1,but2;
  uint8_t e[7];

  read_bytes_from_vrmax_panel(&but0,&but1,&but2);


  // parse encoders
  for (byte i = 0; i < 7; i++)
  {

    int e_diff = e_state[i].last_val - e_state[i].val;
    if (((millis() - e_state[i].enc_ts) > 20) && (e_state[i].button_val == 1))
    {
        e_state[i].button_val = 0;
        simchair_aux1.setButton(e_state[i].button_id, 0);
    }
   else if ((e_state[i].last_val != e_state[i].val) && (e_state[i].press_counter == 0))
    {
      if ((e_diff > 1) && (e_diff < 100)) // fast left turn
      {         
        set_button_mode_and_radio_switch_aware(i,1,0);
        e_state[i].enc_ts = millis();
        if (((obs_rate == 1) && (i == 4))|| ((i == 6) && (dg_rate == 1)))  // OBS knob
        {
          e_state[i].press_counter = OBS_HIGH_RATE_BUTTON_PRESSES;
        }
        else
        {
          e_state[i].press_counter = 0;
        }
        e_state[i].last_dir = 0;
        //e_state[i].button_id = i;
        e_state[i].button_val = 1;
        e_state[i].last_val = e_state[i].val;
      }
      else if ((e_diff < -1) && (e_diff > - 100)) // fast right turn
      {
        set_button_mode_and_radio_switch_aware(i,1,1);
        e_state[i].enc_ts = millis();
        if (((obs_rate == 1) && (i == 4))|| ((i == 6) && (dg_rate == 1)))  // OBS knob
        {
          e_state[i].press_counter = OBS_HIGH_RATE_BUTTON_PRESSES;
        }
        else
        {
          e_state[i].press_counter = 0;
        }
        e_state[i].last_dir = 1;
        e_state[i].button_val = 1;
        e_state[i].last_val = e_state[i].val;
      }
      
      else if ((e_diff <= 100) && (e_diff >= 1))// slow left turn
      {
        set_button_mode_and_radio_switch_aware(i,1,0);
        e_state[i].enc_ts = millis();
        e_state[i].press_counter = 0;
        e_state[i].last_dir = 0;
        e_state[i].button_val = 1;
        e_state[i].last_val = e_state[i].val;
      }
      else if ((e_diff >= -100) && (e_diff <= -1)) // slow right turn
      {
        set_button_mode_and_radio_switch_aware(i,1,1);
        e_state[i].enc_ts = millis();
        e_state[i].press_counter = 0;
        e_state[i].last_dir = 1;
        e_state[i].button_val = 1;
        e_state[i].last_val = e_state[i].val;
      }
      else if ((e_diff > 100) || (e_diff < - 100))
      {
        e_state[i].last_val = e_state[i].val;
      }
    }
    else if (((e_state[i].press_counter > 0) && (e_state[i].button_val == 0)) && (millis() - e_state[i].enc_ts > 30))
    {
        set_button_mode_and_radio_switch_aware(i,1,e_state[i].last_dir);
        e_state[i].enc_ts = millis();
        e_state[i].press_counter--;
        e_state[i].button_val = 1;   
        e_state[i].last_val = e_state[i].val;    
    }
  }

  // parse switches and buttons - ( 4 encoders * 2 buttons each * 3 radios ) + MAG encoder (2 buttons) + OBS (3 * 2 buttons) + ALT (2 * 2 buttons)
  parse_radio_panel_switches(but0,0);
  parse_radio_panel_switches(but1,8);
  parse_radio_panel_switches(but2,16);

}

void set_button_mode_and_radio_switch_aware (byte i,bool val,byte dir)
{
  byte offset = 7;
 // turn direction - 0 left, 1 right

  if ((i != 6) && (i != 4) && (i != 5)) // radio panel
  {
    if (radio_mode == 1)// middle position of panel sw
    {
      if (radio_device == 0) // COM/NAV 1
      {
        if (dir == 0)
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].r0l - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].r0l - 1);
        }
        else
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].r0r - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].r0r - 1);
        }
      }
      else if (radio_device == 1) // COM/NAV 2
      {
        if (dir == 0)
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].r1l - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].r1l - 1);
        }
        else
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].r1r - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].r1r - 1);
        }
      }
      else if (radio_device == 2) // XPDR
      {
        if (nav_mode != 1)
        {
          if (dir == 0)
          {
            simchair_aux1.setButton(radio_panel_knob_matrix[i].r2l - 1,val);
            e_state[i].button_id = (radio_panel_knob_matrix[i].r2l - 1);
          }
          else
          {
            simchair_aux1.setButton(radio_panel_knob_matrix[i].r2r - 1,val);
            e_state[i].button_id = (radio_panel_knob_matrix[i].r2r - 1);
          }
        }
        else
        {
          if (dir == 0)
          {
            simchair_aux1.setButton(radio_panel_knob_matrix[i].adfl - 1,val);
            e_state[i].button_id = (radio_panel_knob_matrix[i].adfl - 1);
          }
          else
          {
            simchair_aux1.setButton(radio_panel_knob_matrix[i].adfr - 1,val);
            e_state[i].button_id = (radio_panel_knob_matrix[i].adfr - 1);
          }
        }
      }

    }
    else if (radio_mode == 0)
    {
      if (radio_device == 0) // PANEL MODE SW UP + STACK SELECTOR SW UP
      {
        if (dir == 0)
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].r0lpm1 - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].r0lpm1 - 1);
        }
        else
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].r0rpm1 - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].r0rpm1 - 1);
        }
      }
      else if (radio_device == 1) // PANEL MODE SW UP + STACK SELECTOR SW MIDDLE
      {
        if (dir == 0)
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].m0l - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].m0l - 1);
        }
        else
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].m0r - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].m0r - 1);
        }
      }
      else if (radio_device == 2) // PANEL MODE SW UP + STACK SELECTOR SW DOWN
      {
        if (dir == 0)
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].r2lpm1 - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].r2lpm1 - 1);
        }
        else
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].r2rpm1 - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].r2rpm1 - 1);
        }       
      }

    }
    else if (radio_mode == 2)
    {


      if (radio_device == 0) // PANEL MODE SW DOWN + STACK SELECTOR SW UP
      {
        if (dir == 0)
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].r0lpm2 - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].r0lpm2 - 1);
        }
        else
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].r0rpm2 - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].r0rpm2 - 1);
        }
      }
      else if (radio_device == 1) // PANEL MODE SW DOWN + STACK SELECTOR SW MIDDLE
      {
        if (dir == 0)
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].m2l - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].m2l - 1);
        }
        else
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].m2r - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].m2r - 1);
        }
      }
      else if (radio_device == 2) // PANEL MODE SW DOWN + STACK SELECTOR SW DOWN
      {
        if (dir == 0)
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].r2lpm2 - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].r2lpm2 - 1);
        }
        else
        {
          simchair_aux1.setButton(radio_panel_knob_matrix[i].r2rpm2 - 1,val);
          e_state[i].button_id = (radio_panel_knob_matrix[i].r2rpm2 - 1);
        }       
      }
    }
  }
  else // side panels knobs
  {
    if (i == 5)
    {
      if (alt_mode == 0) // BARO
      {
        if (dir == 0)
        {
          simchair_aux1.setButton(alt_knob_conf[0].a0l - 1,val);
          e_state[i].button_id = (alt_knob_conf[0].a0l - 1);
        }
        else
        {
          simchair_aux1.setButton(alt_knob_conf[0].a0r - 1,val);
          e_state[i].button_id = (alt_knob_conf[0].a0r - 1);
        }
      }
      else if (alt_mode == 2) // RADIO
      {
        if (dir == 0)
        {
          simchair_aux1.setButton(alt_knob_conf[0].a2l - 1,val);
          e_state[i].button_id = (alt_knob_conf[0].a2l - 1);
        }
        else
        {
          simchair_aux1.setButton(alt_knob_conf[0].a2r - 1,val);
          e_state[i].button_id = (alt_knob_conf[0].a2r - 1);
        }
      }
      else
      {
        if (dir == 0) // just in case you need them
        {
          simchair_aux1.setButton(alt_knob_conf[0].a1l - 1,val);
          e_state[i].button_id = (alt_knob_conf[0].a1l - 1);
        }
        else
        {
          simchair_aux1.setButton(alt_knob_conf[0].a1r - 1,val);
          e_state[i].button_id = (alt_knob_conf[0].a1r - 1);
        }
      }
    }
    else if (i == 4)
    {
      if (nav_mode == 0)
      {
        //simchair_aux1.setButton(45 + i + offset, val);
        //e_state[i].button_id = (45 + i + offset);
        if (dir == 0)
        {
          simchair_aux1.setButton(nav_knob_conf[0].n0l - 1,val);
          e_state[i].button_id = (nav_knob_conf[0].n0l - 1);
        }
        else
        {
          simchair_aux1.setButton(nav_knob_conf[0].n0r - 1,val);
          e_state[i].button_id = (nav_knob_conf[0].n0r - 1);
        }
      }
      else if (nav_mode == 1)
      {
       // simchair_aux1.setButton(49 + i + offset, val);
       // e_state[i].button_id = (49 + i + offset);
       if (dir == 0)
        {
          simchair_aux1.setButton(nav_knob_conf[0].n1l - 1,val);
          e_state[i].button_id = (nav_knob_conf[0].n1l - 1);
        }
        else
        {
          simchair_aux1.setButton(nav_knob_conf[0].n1r - 1,val);
          e_state[i].button_id = (nav_knob_conf[0].n1r - 1);
        }
      }
      else if (nav_mode == 2)
      {
       // simchair_aux1.setButton(52 + i + offset, val);
       // e_state[i].button_id = (52 + i + offset);
       if (dir == 0)
        {
          simchair_aux1.setButton(nav_knob_conf[0].n2l - 1,val);
          e_state[i].button_id = (nav_knob_conf[0].n2l - 1);
        }
        else
        {
          simchair_aux1.setButton(nav_knob_conf[0].n2r - 1,val);
          e_state[i].button_id = (nav_knob_conf[0].n2r - 1);
        }
      }
    }
    else // MAG knob has 1 mode
    {
      //simchair_aux1.setButton(41 + i + offset, val);
      //e_state[i].button_id = (41 + i + offset);
      if (dir == 0)
      {
        simchair_aux1.setButton(mag_knob_conf[0].l - 1,val);
        e_state[i].button_id = (mag_knob_conf[0].l - 1);
      }
      else
      {
        simchair_aux1.setButton(mag_knob_conf[0].r - 1,val);
        e_state[i].button_id = (mag_knob_conf[0].r - 1);
      }
    }
  }
}




void parse_radio_panel_switches (byte b, byte start_pos)
{
  byte end_pos = 8 + start_pos;
  for (byte i = start_pos; i < end_pos; i++)
  {
    //bool v = (b >> i) & 1;
    bool v = (b >> i - start_pos) & 1;
    if ((radio0_unchecked == 1) && (radio2_unchecked == 1))
    {
      radio_device = 1;
    }
    if ((radio_mode0_unchecked == 1) && (radio_mode2_unchecked == 1))
    {
      radio_mode = 1;
    }
    if ((alt_mode0_unchecked == 1) && (alt_mode2_unchecked == 1))
    {
      alt_mode = 1;
    }
    if ((nav_mode0_unchecked == 1) && (nav_mode2_unchecked == 1))
    {
      nav_mode = 1;
    }
    if ((xpdr_mode0_unchecked == 1) && (xpdr_mode2_unchecked == 1))
    {
      if (xpdr_val != 1)
      {
        simchair_aux1.setButton(XPDR_MODE_C_JOY_BUTTON, 1);
        xpdr_val = 1;
      }
    }
    else
    {
      if (xpdr_val != 0)
      {
        simchair_aux1.setButton(XPDR_MODE_C_JOY_BUTTON, 0);
        xpdr_val = 0;
      }
    }
#if !defined(RADIO_PANEL_SETUP)
    if (i == RADIO_DEVICE_SWITCH_XPDR_POSITION_JOY_BUTTON - 1)
    {
      if (v == 1)
      {
        radio_device = 2;
        radio2_unchecked = 0;
      }
      else
      {
        radio2_unchecked = 1;
      }
    }
    else if (i == RADIO_DEVICE_SWITCH_XPDR_POSITION_JOY_BUTTON)
    {
      if (v == 1)
      {
        radio_device = 0;
        radio0_unchecked = 0;
      }
      else
      {
        radio0_unchecked = 1;
      }
    }
    else if (i == RADIO_MODE_SWITCH_LOWER_POSITION_JOY_BUTTON - 1)
    {
      if (v == 1)
      {
        radio_mode = 2;
        radio_mode2_unchecked = 0;
      }
      else
      {
        radio_mode2_unchecked = 1;
      }
    }
    else if (i == RADIO_MODE_SWITCH_LOWER_POSITION_JOY_BUTTON)
    {
      if (v == 1)
      {
        radio_mode = 0;
        radio_mode0_unchecked = 0;
      }
      else
      {
        radio_mode0_unchecked = 1;
      }
    }
    else if (i == ALTIMETER_SELECTOR_JOY_BUTTON - 1)
    {
      if (v == 1)
      {
        alt_mode = 2;
        alt_mode2_unchecked = 0;
      }
      else
      {
        alt_mode2_unchecked = 1;
      }
    }
    else if (i == ALTIMETER_SELECTOR_JOY_BUTTON)
    {
      if (v == 1)
      {
        alt_mode = 0;
        alt_mode0_unchecked = 0;
      }
      else
      {
        alt_mode0_unchecked = 1;
      }
    }
    else if (i == NAV_MODE_SELECTOR_JOY_BUTTON - 1)
    {
      if (v == 1)
      {
        nav_mode = 2;
        nav_mode2_unchecked = 0;
      }
      else
      {
        nav_mode2_unchecked = 1;
      }
    }
    else if (i == NAV_MODE_SELECTOR_JOY_BUTTON)
    {
      if (v == 1)
      {
        nav_mode = 0;
        nav_mode0_unchecked = 0;
      }
      else
      {
        nav_mode0_unchecked = 1;
      }
    }
    else if (i == OBS_RATE_SELECTOR_JOY_BUTTON - 1)
    {
      if (v != radio_matrix[i].sw_val)
      {
        if (v == 1)
        {
         //Serial.println(obs_rate);
         obs_rate = !obs_rate;
         
         e_state[4].press_counter = 0;
        }
        radio_matrix[i].sw_val = v;
      }
    }
    else if (i == DIR_GYRO_RATE_SELECTOR_JOY_BUTTON - 1)
    {
      if (v != radio_matrix[i].sw_val)
      {
        if (v == 1)
        {
         //Serial.println(obs_rate);
         dg_rate = !dg_rate;
         
         e_state[6].press_counter = 0;
        }
        radio_matrix[i].sw_val = v;
      }
    }
    else if (i == RP_COM_FINE_PB_JOY_BUTTON - 1)
    {
      if (v != radio_matrix[i].sw_val)
      {
          if (radio_device == 0)
          {
            simchair_aux1.setButton(radio_panel_pb_matrix[0].c1f, v);
            radio_matrix[i].sw_val = v;
          }
          else if (radio_device == 1)
          {
            simchair_aux1.setButton(radio_panel_pb_matrix[0].c2f, v);
            radio_matrix[i].sw_val = v;
          } 
          if (radio_device == 2)
          {
            simchair_aux1.setButton(radio_panel_pb_matrix[0].x1, v);
            radio_matrix[i].sw_val = v;
          }
      }
    }
    else if (i == RP_COM_COARSE_PB_JOY_BUTTON - 1)
    {
      if (v != radio_matrix[i].sw_val)
      {
          if (radio_device == 0)
          {
            simchair_aux1.setButton(radio_panel_pb_matrix[0].c1c, v);
            radio_matrix[i].sw_val = v;
          }
          else if (radio_device == 1)
          {
            simchair_aux1.setButton(radio_panel_pb_matrix[0].c2c, v);
            radio_matrix[i].sw_val = v;
          } 
          if (radio_device == 2)
          {
            simchair_aux1.setButton(radio_panel_pb_matrix[0].x2, v);
            radio_matrix[i].sw_val = v;
          }
      }
    }
    else if (i == RP_NAV_FINE_PB_JOY_BUTTON - 1)
    {
      if (v != radio_matrix[i].sw_val)
      {
          if (radio_device == 0)
          {
            simchair_aux1.setButton(radio_panel_pb_matrix[0].n1f, v);
            radio_matrix[i].sw_val = v;
          }
          else if (radio_device == 1)
          {
            simchair_aux1.setButton(radio_panel_pb_matrix[0].n2f, v);
            radio_matrix[i].sw_val = v;
          } 
          if (radio_device == 2)
          {
            simchair_aux1.setButton(radio_panel_pb_matrix[0].x3, v);
            radio_matrix[i].sw_val = v;
          }
      }
    }
    else if (i == RP_NAV_COARSE_PB_JOY_BUTTON - 1)
    {
      if (v != radio_matrix[i].sw_val)
      {
          if (radio_device == 0)
          {
            simchair_aux1.setButton(radio_panel_pb_matrix[0].n1c, v);
            radio_matrix[i].sw_val = v;
          }
          else if (radio_device == 1)
          {
            simchair_aux1.setButton(radio_panel_pb_matrix[0].n2c, v);
            radio_matrix[i].sw_val = v;
          } 
          if (radio_device == 2)
          {
            simchair_aux1.setButton(radio_panel_pb_matrix[0].x4, v);
            radio_matrix[i].sw_val = v;
          }
      }
    }
#endif

    else if (v != radio_matrix[i].sw_val) // regular joy button
    {
      simchair_aux1.setButton(i, v);
      radio_matrix[i].sw_val = v;
    }

    if (i == XPDR_MODE_SW_JOY_BUTTON - 1)
    {
      if (v == 1)
      {
        xpdr_mode2_unchecked = 0;
      }
      else
      {
        xpdr_mode2_unchecked = 1;
      }
    }
    else if (i == XPDR_MODE_SW_JOY_BUTTON)
    {
      if (v == 1)
      {
        xpdr_mode0_unchecked = 0;
      }
      else
      {
        xpdr_mode0_unchecked = 1;
      }
    }
      //simchair_aux1.setButton(first_joy_b + i, v);

  }
}
