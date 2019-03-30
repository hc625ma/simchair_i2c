// This file contains subroutines, shared by other files.

bool is_device_connected(uint8_t address)
{
  Wire.beginTransmission(address);
  return (Wire.endTransmission() == 0);
}

int adjust_sensitivity (int val, int param)
{
//  if ((SENS_ADJ_METHOD == "LINEAR") && (param < 100))
  if (param < 100)
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

  for (byte i = 0; i < 2;i++)
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

void parse_coll_head_sw_matrix (int sw, uint8_t start_pos, uint8_t end_pos)
{
  if (end_pos == 0)
  {
    end_pos = 8 + start_pos;
  }
  for (byte i = start_pos; i < end_pos; i++)
  {
    bool v = (sw >> i - start_pos) & 1;
    //switch_matrix[i].sw_val = v;
    //<determine mode switch position>
    //special stuff - DCS Huey compat mode
    if ((DCS_HUEY_IDLE_STOP_COMPAT_MODE_ENABLED == 1) && (i == IDLE_STOP_BUTTON) && (v == 1))
    {
      idle_stop_pressed[0] = v;
    }
    //
    if (i == MODE_SWITCH_BUTTON - 1)
    {
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
    if ( i == MODE_SWITCH_BUTTON)
    {
      if (v == 1)
      {
        coll_head_mode_sw_position = 2;
      }
      else if (switch_matrix[i].sw_val == mode_sw_pos_0)
      {
        coll_head_mode_sw_position = 0;

      }
    }
    else if ((i == (COLLECTIVE_HOLD_BUTTON - 1)) && (COLLECTIVE_HOLD_ENABLED == 1))
    {
      if (v != switch_matrix[i].sw_val)
      {
        if (v == 1)
        {
          collective_hold_active = !collective_hold_active;
        }
        switch_matrix[i].sw_val = v;
      }
    }
    //</determine mode switch position>
    else if (switch_matrix[i].sw_type == 1) // regular button
    {
      if (v != switch_matrix[i].sw_val)
      {
        set_button_mode_aware(i,v);
        switch_matrix[i].sw_val = v;
      }
    }
    else if (switch_matrix[i].sw_type == 2) // momentary button press on switch flip
    {
      if (v != switch_matrix[i].sw_val)
      {
        long now = millis();
        if ((now - switch_matrix[i].sw_ts) > COLL_HEAD_SW_HOLD_TIMEOUT)
        {
          simchair_aux2.setButton(i, v);
          switch_matrix[i].sw_val = v;
          switch_matrix[i].sw_ts = millis();
        }
      }
      else if ((v == 1) && (switch_matrix[i].sw_tr_state == 0))
      {
        long now = millis();
        if ((now - switch_matrix[i].sw_ts) > COLL_HEAD_SW_HOLD_TIMEOUT)
        {
          simchair_aux2.setButton(i, !v);
          switch_matrix[i].sw_tr_state = 1;
        }
      }
      else if ((v == 0) && (switch_matrix[i].sw_tr_state == 1))
      {
        long now = millis();
        simchair_aux2.setButton(i, !v);
        switch_matrix[i].sw_tr_state = 0;
        switch_matrix[i].sw_ts = millis();
        switch_matrix[i].sw_val = !v;
      }
    }
    else if (switch_matrix[i].sw_type == 3) // selector button switch type 1st button press
    {
      if (v != switch_matrix[i].sw_val)
      {
        set_button_mode_aware(i,v);
        switch_matrix[i].sw_val = v;
        set_button_mode_aware(switch_matrix[i].sw_middle_b, !v);
        switch_matrix[switch_matrix[i].sw_middle_b].sw_tr_state = 0;
      }
    }
    else if (switch_matrix[i].sw_type == 4) // selector toggle switch type 1st button press
    {
      if (v != switch_matrix[i].sw_val)
      {
          simchair_aux2.setButton(i, v);
          switch_matrix[i].sw_val = v;
          simchair_aux2.setButton(switch_matrix[i].sw_middle_b, !v);
          switch_matrix[switch_matrix[i].sw_middle_b].sw_val = !v;
          switch_matrix[i].sw_ts = millis();
          switch_matrix[switch_matrix[i].sw_middle_b].sw_ts = millis();
       }
       else if ((v == 1) && (switch_matrix[i].sw_tr_state == 0))
       {
          long now = millis();
          if ((now - switch_matrix[i].sw_ts) > COLL_HEAD_SW_HOLD_TIMEOUT)
          {
            simchair_aux2.setButton(i, !v);
            switch_matrix[i].sw_tr_state = 1;
          }
       }
       else if ((v == 0) && (switch_matrix[i].sw_tr_state == 1))
       {
          long now = millis();
          if ((now - switch_matrix[i].sw_ts) > COLL_HEAD_SW_HOLD_TIMEOUT)
          {
            simchair_aux2.setButton(i, v);
            switch_matrix[i].sw_val = v;
            switch_matrix[i].sw_tr_state = 0;

            simchair_aux2.setButton(switch_matrix[i].sw_middle_b, v);
            switch_matrix[switch_matrix[i].sw_middle_b].sw_val = v;
            switch_matrix[switch_matrix[i].sw_middle_b].sw_tr_state = 0;
          }
        }

    }
    else if (switch_matrix[i].sw_type == 5) // slave button position of switch types 3 and 4
    {
      if (switch_matrix[i-1].sw_type == 3) // selector button sw type slave button press
      {
        if (v != switch_matrix[i].sw_val)
        {
          set_button_mode_aware(i,v);
          switch_matrix[i].sw_val = v;
          set_button_mode_aware(switch_matrix[i].sw_middle_b, !v);
          switch_matrix[switch_matrix[i].sw_middle_b].sw_val = !v;
          switch_matrix[switch_matrix[i].sw_middle_b].sw_tr_state = 0;
        }
      }
      else if (switch_matrix[i-1].sw_type == 4) // selector toggle switch type slave button press
      {
        if (v != switch_matrix[i].sw_val)
        {
          simchair_aux2.setButton(i, v);
          switch_matrix[i].sw_val = v;
          simchair_aux2.setButton(switch_matrix[i].sw_middle_b, !v);
          switch_matrix[switch_matrix[i].sw_middle_b].sw_val = !v;
          switch_matrix[i].sw_ts = millis();
          switch_matrix[switch_matrix[i].sw_middle_b].sw_ts = millis();
        }
        else if ((v == 1) && (switch_matrix[i].sw_tr_state == 0))
        {
          long now = millis();
          if ((now - switch_matrix[i].sw_ts) > COLL_HEAD_SW_HOLD_TIMEOUT)
          {
            simchair_aux2.setButton(i, !v);
            switch_matrix[i].sw_tr_state = 1;
          }
        }
        else if ((v == 0) && (switch_matrix[i].sw_tr_state == 1))
        {
          long now = millis();
          if ((now - switch_matrix[i].sw_ts) > COLL_HEAD_SW_HOLD_TIMEOUT)
          {
            simchair_aux2.setButton(i, v);
            switch_matrix[i].sw_val = v;
            switch_matrix[i].sw_tr_state = 0;

            simchair_aux2.setButton(switch_matrix[i].sw_middle_b, v);
            switch_matrix[switch_matrix[i].sw_middle_b].sw_val = v;
            switch_matrix[switch_matrix[i].sw_middle_b].sw_tr_state = 0;
          }
        }
      }
    }
  }
}

void set_button_mode_aware (int i, int v)
{
  if (coll_head_mode_sw_position == 0)
  {
    simchair_aux2.setButton(i, v);
//    Serial.print("i");
//    Serial.print(" ");
//    Serial.print(i);
//    Serial.print(" ");
//    Serial.print("v");
//    Serial.print(" ");
//    Serial.println(v);
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
