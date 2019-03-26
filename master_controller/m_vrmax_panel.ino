void setup_vrmax_panel()
{
  Wire.beginTransmission(21);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    dev_vrmax_panel = 1;
  }
  
}

void poll_vrmax_panel()
{

  uint8_t but0,but1,but2;
  uint8_t e[7];
  
  Wire.requestFrom(22, 5); //radio panel - top left encoder
  while (Wire.available())
  {
    byte b1 = Wire.read(); // buttons #1
    byte b2 = Wire.read(); // buttons #2
    byte b3 = Wire.read(); // encoder 1
    byte b4 = Wire.read(); // encoder 2
    byte b5 = Wire.read(); // encoder 3

    but0 = b1;
    but1 = b2;
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

    but2 = b1;
    e_state[3].val = b5;
    e_state[4].val = b3;
    e_state[5].val = b4;
    e_state[6].val = b2;
  }

  
  // parse encoders
  for (byte i = 0; i < 7; i++)
  {
    if ((millis() - e_state[i].enc_ts) > 100)
    {
      e_state[i].button_val = 0;
      simchair_aux1.setButton(e_state[i].button_id, 0);

      //simchair_aux1.setButton(7 + i, 0);
    }
    if (e_state[i].last_val != e_state[i].val)
    {
      int e_diff = e_state[i].last_val - e_state[i].val;
      if ((e_diff > 3) && (e_diff < 100)) // left turn
      {
        e_state[i].last_val = e_state[i].val;
        //simchair_aux1.setButton(i, 1);       
        set_button_mode_and_radio_switch_aware(i,1,0);
        e_state[i].enc_ts = millis();
        //e_state[i].button_id = i;
        e_state[i].button_val = 1;
      }
      else if ((e_diff < -3) && (e_diff > - 100)) // right turn
      {
        e_state[i].last_val = e_state[i].val;
        //simchair_aux1.setButton(7 + i, 1); 
        //set_button_mode_and_radio_switch_aware(i,1,7);      
        set_button_mode_and_radio_switch_aware(i,1,1);
        e_state[i].enc_ts = millis();
        //e_state[i].button_id = 7 + i;
        e_state[i].button_val = 1;
      }
      else if ((e_diff > 100) || (e_diff < - 100))
      {
        e_state[i].last_val = e_state[i].val;
      }
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
    if (radio_mode == 1) // middle position of panel sw
    {
      if (radio_device == 0) // COM/NAV 1, buttons 31 - 35
      {
        
        //simchair_aux1.setButton(25 + i + offset, val); 
        //e_state[i].button_id = (25 + i + offset);
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
      else if (radio_device == 1) // COM/NAV 2 36-40
      {
        //simchair_aux1.setButton(30 + i + offset, val); 
        //e_state[i].button_id = (30 + i + offset);
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
      else if (radio_device == 2) // XPDR 41 - 45
      {
        //simchair_aux1.setButton(35 + i + offset, val); 
        //e_state[i].button_id = (35 + i + offset);
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
    }
    else if (radio_mode == 0)
    {
      //simchair_aux1.setButton(55 + i + offset, val); 
      //e_state[i].button_id = (55 + i + offset);
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
    else if (radio_mode == 2)
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
  }
  else // side panels knobs
  {
    if (i == 5)
    {
      if (alt_mode == 0)
      {
        //simchair_aux1.setButton(40 + i + offset, val); 
        //e_state[i].button_id = (40 + i + offset);
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
      else if (alt_mode == 2)
      {
        //simchair_aux1.setButton(42 + i + offset, val); 
        //e_state[i].button_id = (42 + i + offset);
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
#endif
    else if (v != radio_matrix[i].sw_val) // regular joy button
    {
      simchair_aux1.setButton(i, v);
      radio_matrix[i].sw_val = v;
    }
      //simchair_aux1.setButton(first_joy_b + i, v);

  }
}

