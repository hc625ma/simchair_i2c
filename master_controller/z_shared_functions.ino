// This file contains subroutines, shared by other files.

int adjust_sensitivity (int val, int param)
{
  if ((SENS_ADJ_METHOD == "LINEAR") && (param < 100))
  {
    int percent = param;
    // this will simply limit your controls throw range by the given percent,
    // and map full ADC range for it.
    // may be a good option for flying helicopters in x-plane
    int center = ADC_RANGE / 2;
    int adj_range = (ADC_RANGE / 100) * percent;
    val = map(val, 0, ADC_RANGE, center - (adj_range / 2), center + (adj_range / 2));
  }

  return val;
}

uint16_t coll_head_idle_stop_compat_dcs (uint16_t throttle0, uint16_t throttle1, uint16_t idle_val0, uint16_t idle_val1)
{
  uint16_t throttle[] = {throttle0,throttle1};
  uint16_t idle_val[] = {idle_val0,idle_val1};

  for (byte i = 0; i < sizeof(ab412_coll_head_idle_stop_buttons);i++)
    {
      if ((throttle[i] < idle_val[i]) && (throttle_idle_cutoff[i] == 1))
      {
        int diff = last_throttle_setting[i] - throttle[i];
        if (diff < - COLL_HEAD_IDLE_STOP_COMPAT_TRESHOLD)
        {
          Keyboard.press(coll_head_idle_stop_compat_throttle_down_keys[i]);
          delay(COLL_HEAD_DCS_HUEY_COMPAT_MODE_BUTTON_HOLD);
          Keyboard.releaseAll();
        }
      }
      if (idle_stop_pressed[i] == 1)
      {
        int diff = last_throttle_setting[i] - throttle[i];
        if (diff > COLL_HEAD_IDLE_STOP_COMPAT_TRESHOLD)
        {
          Keyboard.press(coll_head_idle_stop_compat_throttle_up_keys[i]);
          delay(COLL_HEAD_DCS_HUEY_COMPAT_MODE_BUTTON_HOLD);
          Keyboard.releaseAll();
        }
        last_throttle_setting[i] = throttle[i];
      }
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

void coll_head_parse_switches (int sw, int start_pos, int end_pos)
{
  if (end_pos == 0)
  {
    end_pos = 8 + start_pos;
  }
  for (byte i = start_pos; i < end_pos; i++)
  {
    bool v = (sw >> i - start_pos) & 1;
 //   if (AB412_COLL_HEAD_MODE_SWITCH_ENABLED == 1)
    //{
      if (dev_ab412_coll_head == 1)
      {
        if (i == AB412_COLL_HEAD_MODE_SWITCH - 1)
        {
         // Serial.println("BANG");
          if (v == 1)
          {
            coll_head_mode_sw_position = 1;
            mode_sw_pos_0 = 1;
          }
          else
          {
            mode_sw_pos_0 = 0;
          }
        }

          else if (i == (ab412_coll_head_starter_buttons[0]-1))
          {
            if ((COLL_HEAD_IDLE_STOP_COMPAT_PROFILE_XTRIDENT412 == 1) && ((coll_head_mode_sw_position == 0) || (AB412_COLL_HEAD_MODE_SWITCH == 0)))
            {
                if (v != coll_head_lastButtonState[i])
                {
                  Keyboard.write(AB412_STARTER2_SW_KEY);
                  coll_head_lastButtonState[i] = v;
                }
            }
          }
          else if (i == (ab412_coll_head_starter_buttons[1]-1))
          {
            if ((COLL_HEAD_IDLE_STOP_COMPAT_PROFILE_XTRIDENT412 == 1) && ((coll_head_mode_sw_position == 0) || (AB412_COLL_HEAD_MODE_SWITCH == 0)))
            {
                if (v != coll_head_lastButtonState[i])
                {
                  Keyboard.write(AB412_STARTER1_SW_KEY);
                  coll_head_lastButtonState[i] = v;
                }
            }
          }
          else if (i == (ab412_coll_head_idle_stop_buttons[0]-1))
          {
            if ((COLL_HEAD_IDLE_STOP_COMPAT_PROFILE_XTRIDENT412 == 1) && ((coll_head_mode_sw_position == 0) || (AB412_COLL_HEAD_MODE_SWITCH == 0)))
            {
                if (v != coll_head_lastButtonState[i])
                {
                  Keyboard.write(AB412_IDLE1_SW_KEY);
                  coll_head_lastButtonState[i] = v;
                }
            }
          }
          else if (i == (ab412_coll_head_idle_stop_buttons[1]-1))
          {
            //Serial.println("BOOM!");
            if ((COLL_HEAD_IDLE_STOP_COMPAT_PROFILE_XTRIDENT412 == 1) && ((coll_head_mode_sw_position == 0) || (AB412_COLL_HEAD_MODE_SWITCH == 0)))
            {
                if (v != coll_head_lastButtonState[i])
                {
                  //Serial.println("BOOM!");
                  Keyboard.write(AB412_IDLE2_SW_KEY);
                  coll_head_lastButtonState[i] = v;
                }
            }
          }
          
          else if (i == AB412_COLL_HEAD_MODE_SWITCH)
          {
            if (v == 1)
            {
              coll_head_mode_sw_position = 2;
            }
            else if (v == mode_sw_pos_0)
            {
              coll_head_mode_sw_position = 0;
            }
          }

         if ((COLLECTIVE_HOLD_ENABLED == 1) && ((coll_head_mode_sw_position == 0) || (AB412_COLL_HEAD_MODE_SWITCH == 0)))
         {                      
            if ((i == AB412_HEAD_COLLECTIVE_HOLD_BUTTON - 1) )
            {
              if (v != coll_head_lastButtonState[i])
              {
                if (v == 1)
                {
                  if (collective_hold_active == 0)
                  {
                    collective_hold_active = 1;
                  }
                  else if (collective_hold_active == 1)
                  {
                    collective_hold_active = 0;
                  }
                }
              }
            }
          }
        
      }
      else if (dev_huey_coll_head == 1)
      {
        if (i == HUEY_COLL_HEAD_MODE_SWITCH - 1)
        {
         // Serial.println("BANG");
          if (v == 1)
          {
            coll_head_mode_sw_position = 1;
            mode_sw_pos_0 = 1;
          }
          else
          {
            mode_sw_pos_0 = 0;
          }
        }
        else if (i == HUEY_COLL_HEAD_MODE_SWITCH)
        {
          if (v == 1)
          {
            coll_head_mode_sw_position = 2;
          }
          else if (v == mode_sw_pos_0)
          {
            coll_head_mode_sw_position = 0;
          }
        }
        if ((COLLECTIVE_HOLD_ENABLED == 1) && ((coll_head_mode_sw_position == 0) || (HUEY_COLL_HEAD_MODE_SWITCH == 0)))
        {                      
            if ((i == HUEY_HEAD_COLLECTIVE_HOLD_BUTTON - 1) )
            {
              if (v != coll_head_lastButtonState[i])
              {
                if (v == 1)
                {
                  if (collective_hold_active == 0)
                  {
                    collective_hold_active = 1;
                    //coll_head_lastButtonState[i] = v;// added
                  }
                  else if (collective_hold_active == 1)
                  {
                    collective_hold_active = 0;
                    //coll_head_lastButtonState[i] = v; //added
                  }
                }
              }
            }
        }
      }

    if (coll_head_sw_mode_toggle_switches_parsed[i] == 1)
    {
      if (v != coll_head_lastButtonState[i])
      { 
        if (COLL_HEAD_IDLE_STOP_COMPAT_MODE == 1)
        {
          if (dev_ab412_coll_head == 1)
          {
            for (byte j = 0; j < sizeof(ab412_coll_head_idle_stop_buttons); j++)
            {
              if (i == ab412_coll_head_idle_stop_buttons[j] - 1)
              {
                idle_stop_pressed[j] = v;           
              }
            }
          }
          else if (dev_huey_coll_head == 1)
          {
            for (byte j = 0; j < sizeof(huey_coll_head_idle_stop_buttons); j++)
            {
              if (i == huey_coll_head_idle_stop_buttons[j] - 1)
              {
                idle_stop_pressed[j] = v;           
              }
            }
          }
        }    
        long now = millis();
        if ((now - coll_head_sw_ts[i]) > COLL_HEAD_SW_HOLD_TIMEOUT)
        {
          simchair_aux2.setButton(i, v);
          //set_button_mode_aware(i,v);
          coll_head_lastButtonState[i] = v;
          coll_head_sw_ts[i] = millis();
        }        
      }
      else if ((v == 1) && (coll_head_triggerState[i] == 0))
      {
        long now = millis();
        if ((now - coll_head_sw_ts[i]) > COLL_HEAD_SW_HOLD_TIMEOUT)
        {
          simchair_aux2.setButton(i, !v);
          //set_button_mode_aware(i,!v);
          coll_head_triggerState[i] = 1;
        }
      }
      else if ((v == 0) && (coll_head_triggerState[i] == 1))
      {
        long now = millis();
        simchair_aux2.setButton(i, !v);
        //set_button_mode_aware(i,!v);
        coll_head_triggerState[i] = 0;
        coll_head_sw_ts[i] = millis();
        coll_head_lastButtonState[i] = !v;
      }
    }
    else if (coll_head_sw_mode_button_switches_parsed[i] == 1)
    {
      if (v != coll_head_lastButtonState[i])
      {
        if (COLL_HEAD_IDLE_STOP_COMPAT_MODE == 1)
        {
//          for (byte j = 0; j < sizeof(ab412_coll_head_idle_stop_buttons); j++)
//          {
//            if (i == ab412_coll_head_idle_stop_buttons[j] - 1)
//            {
//              idle_stop_pressed[j] = v;  
//            }
//          }
          if (dev_ab412_coll_head == 1)
          {
            for (byte j = 0; j < sizeof(ab412_coll_head_idle_stop_buttons); j++)
            {
              if (i == ab412_coll_head_idle_stop_buttons[j] - 1)
              {
                idle_stop_pressed[j] = v;           
              }
            }
          }
          else if (dev_huey_coll_head == 1)
          {
            for (byte j = 0; j < sizeof(huey_coll_head_idle_stop_buttons); j++)
            {
              if (i == huey_coll_head_idle_stop_buttons[j] - 1)
              {
                idle_stop_pressed[j] = v;           
              }
            }
          }
        }  
        //}
        //simchair_aux2.setButton(i, v);
        set_button_mode_aware(i,v);
        coll_head_lastButtonState[i] = v;
      }
    }
//    if (coll_head_mode_sw_position == 0)
//    {
    else if (coll_head_sw_mode_selector_button_switches_parsed[i] == 1)
      //if (coll_head_sw_mode_selector_button_switches_parsed[i] == 1)
      {          
        if (v != coll_head_lastButtonState[i])
        {
          //simchair_aux2.setButton(i, v);
          set_button_mode_aware(i,v);
          coll_head_lastButtonState[i] = v;
          //simchair_aux2.setButton(33 - i, !v);
          set_button_mode_aware(31 - i, !v);
          //coll_head_lastButtonState[33 - i] = !v;
          coll_head_triggerState[31 - i] = 0;
        }
      }
      else if (coll_head_sw_mode_selector_button_switches_parsed[i - 1] == 1)
      {      
        if (v != coll_head_lastButtonState[i])
        {
          //simchair_aux2.setButton(i, v);
          set_button_mode_aware(i,v);
          coll_head_lastButtonState[i] = v;
          //simchair_aux2.setButton(32 - i + 1, !v);
          set_button_mode_aware(31 - i + 1, !v);
          coll_head_lastButtonState[31 - i + 1] = !v;
          coll_head_triggerState[31 - i + 1] = 0;
        }
      }
      else if (coll_head_sw_mode_selector_switches_parsed[i] == 1)
      {
        if (v != coll_head_lastButtonState[i])
        {
          //Serial.println("BANG");
          simchair_aux2.setButton(i, v);
          //set_button_mode_aware(i, v);
          coll_head_lastButtonState[i] = v;
          simchair_aux2.setButton(31 - i, !v);
          //set_button_mode_aware(32 - i, !v);
          coll_head_lastButtonState[31 - i] = !v;
          //ab412_coll_head_triggerState[32 - i] = 0;
          coll_head_sw_ts[i] = millis();
          coll_head_sw_ts[31 - i] = millis();
        }
        else if ((v == 1) && (coll_head_triggerState[i] == 0))
        {
          long now = millis();
          if ((now - coll_head_sw_ts[i]) > COLL_HEAD_SW_HOLD_TIMEOUT)
          {
            simchair_aux2.setButton(i, !v);
            //set_button_mode_aware(i, !v);
            coll_head_triggerState[i] = 1;
          }
        }
        else if ((v == 0) && (coll_head_triggerState[i] == 1))
        {
          long now = millis();
          if ((now - coll_head_sw_ts[i]) > COLL_HEAD_SW_HOLD_TIMEOUT)
          {
            simchair_aux2.setButton(i, v);
            //set_button_mode_aware(i,v);
            coll_head_lastButtonState[i] = v;
            coll_head_triggerState[i] = 0;
  
            simchair_aux2.setButton(31 - i, v);
            //set_button_mode_aware(32 - i, v);
            coll_head_lastButtonState[31 - i] = v;
            coll_head_triggerState[31 - i] = 0;
          }
        }
  
      }
      else if (coll_head_sw_mode_selector_switches_parsed[i - 1] == 1)
      {
        if (v != coll_head_lastButtonState[i])
        {
          simchair_aux2.setButton(i, v);
          //set_button_mode_aware(i, v);
          coll_head_lastButtonState[i] = v;
          simchair_aux2.setButton(31 - i + 1, !v);
          //set_button_mode_aware(32 - i + 1,!v);
          coll_head_lastButtonState[31 - i + 1] = !v;
          coll_head_sw_ts[i] = millis();
          coll_head_sw_ts[31 - i + 1] = millis();
        }
        else if ((v == 1) && (coll_head_triggerState[i] == 0))
        {
          long now = millis();
          if ((now - coll_head_sw_ts[i]) > COLL_HEAD_SW_HOLD_TIMEOUT)
          {
            simchair_aux2.setButton(i, !v);
            //set_button_mode_aware(i, !v);
            coll_head_triggerState[i] = 1;
          }
        }
        else if ((v == 0) && (coll_head_triggerState[i] == 1))
        {
          long now = millis();
          if ((now - coll_head_sw_ts[i]) > COLL_HEAD_SW_HOLD_TIMEOUT)
          {
            simchair_aux2.setButton(i, v);
            //set_button_mode_aware(i, v);
            coll_head_lastButtonState[i] = v;
            coll_head_triggerState[i] = 0;
  
            simchair_aux2.setButton(31 - i + 1, v);
            //set_button_mode_aware(32 - i + 1, v);
            coll_head_lastButtonState[31 - i + 1] = v;
            coll_head_triggerState[31 - i + 1] = 0;
          }
        }
      }
 //   }
  }
}

void set_button_mode_aware (int i, int v)
{
  if (coll_head_mode_sw_position == 0)
  {
    simchair_aux2.setButton(i, v);
  }
  else if (coll_head_mode_sw_position == 1)
  {
    simchair_aux2.setButton(32 + i, v);
  }
  else if (coll_head_mode_sw_position == 2)
  {
    simchair_aux2.setButton(64 + i, v);
  }

}

int parse_hat_sw (int x, int y, byte dirs)
{
  int hat_val;
  if (dirs == 8)
  {
    if (y > 145)
    {
      if (x > 145)
      {
        hat_val = 45;
      }
      else if (x < 105)
      {
        hat_val = 315;
      }
      else
      {
        hat_val = 0;
      }
    }
    else if (y < 105)
    {
      if (x > 160)
      {
        hat_val = 135;
      }
      else if (x < 105)
      {
        hat_val = 225;
      }
      else
      {
        hat_val = 180;
      }
    }
    else if (x > 145)
    {
      hat_val = 90;
    }
    else if (x < 105)
    {
      hat_val = 270;
    }
    else
    {
      hat_val = JOYSTICK_HATSWITCH_RELEASE;
    }
  }
  else
  {
    if (y > 145)
    {
      hat_val = 0;
    }
    else if (y < 105)
    {
       hat_val = 180;
    }
    else if (x > 145)
    {
      hat_val = 90;
    }
    else if (x < 105)
    {
      hat_val = 270;
    }
    else
    {
      hat_val = JOYSTICK_HATSWITCH_RELEASE;
    }
  }
  return hat_val;
}


