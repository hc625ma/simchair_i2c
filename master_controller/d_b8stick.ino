void setup_b8stick()
{
  //MKIII B8 stick
  if (!is_device_connected(B8STICK_I2C_ADDRESS))
    return;

  simchair_aux1.setXAxisRange(0, 255);
  simchair_aux1.setYAxisRange(0, 255);
  dev_b8stick = 1;
}

void poll_b8stick()
{
  //hot swap support for cyclic grip - a very useful feature
  if (!is_device_connected(B8STICK_I2C_ADDRESS))
    return;

  uint8_t rx;
  uint8_t ry;
  uint8_t b;
  Wire.requestFrom(B8STICK_I2C_ADDRESS, 3);
  while (Wire.available())
  {
    byte b1 = Wire.read();
    byte b2 = Wire.read();
    b = Wire.read();

    rx = b1;
    ry = b2;
  }
  if (B8_POT_MODE == "ANALOG")
  {
    simchair_aux1.setXAxis(rx);
    simchair_aux1.setYAxis(ry);
  }
  else
  {
    if (B8_HAT_SWITCH_MODE == "HAT")
    {
      int16_t hat_val = parse_hat_sw(rx, ry, 8);
      simchair.setHatSwitch(0, hat_val);
    }
    else if (B8_HAT_SWITCH_MODE == "BOTH")
    {
      //if (force_trim_on == 0)
      if (controls_freezed == 0)
      {
        int16_t hat_val = parse_hat_sw(rx, ry, 8);
        simchair.setHatSwitch(0, hat_val);
      }
      else
      {
        int16_t hat_val = parse_hat_trim(rx, ry, INVERT_HAT_TRIM_X, INVERT_HAT_TRIM_Y);
      }
    }
    else
    {
      int16_t hat_val = parse_hat_trim(rx, ry, INVERT_HAT_TRIM_X, INVERT_HAT_TRIM_Y);
    }

  }
  for (byte i = 0; i < 6; i++)
  {
    bool v = (b >> i) & 1;
    if (v != b8stick_lastButtonState[i])
    {
      if (((SENS_SWITCH_ENABLED == 1) && (SENS_DEVICE == "b8_stick")) || (PTT_KEYBOARD_PRESS == 1) || (PSEUDO_FORCE_TRIM == 1))
      {
        if ((i != SENS_SWITCH_BUTTON) && (i != PTT_BUTTON) && (i != PSEUDO_FORCE_TRIM_BUTTON))
        {
          simchair.setButton(i, v);
        }
        else
        {
          //if (((SENS_SWITCH_MODE == "FORCE_TRIM") || ((SENS_SWITCH_MODE == "TOGGLE") && (v == 1))) && (i != PTT_BUTTON) && (i != PSEUDO_FORCE_TRIM_BUTTON))
          if ((v == 1) && (i != PTT_BUTTON) && (i != PSEUDO_FORCE_TRIM_BUTTON))
          {
            if (cyclic_sens == 100)
            {
              cyclic_sens = CUSTOM_CYCLIC_SENS;
              rudder_sens = CUSTOM_RUDDER_SENS;
              physical_cyclic_center_x = adjust_sensitivity (physical_cyclic_center_x, CUSTOM_CYCLIC_SENS);
              physical_cyclic_center_y = adjust_sensitivity (physical_cyclic_center_y, CUSTOM_CYCLIC_SENS);
              if (SENS_SWITCH_TRIM_RESET == 1)
              {
                force_trim_on = 0;
                force_trim_position_set = 0;
                force_trim_rudder_on = 0;
                force_trim_rudder_position_set = 0;
              }
            }
            else
            {
              cyclic_sens = 100;
              rudder_sens = 100;
              physical_cyclic_center_x = adjust_sensitivity (physical_cyclic_center_x, 100);
              physical_cyclic_center_y = adjust_sensitivity (physical_cyclic_center_y, 100);
              if (SENS_SWITCH_TRIM_RESET == 1)
              {
                x_diff = 0;
                y_diff = 0;
                cyclic_x_adj = 0;
                cyclic_y_adj = 0;
                force_trim_on = 0;
                force_trim_position_set = 0;
                force_trim_rudder_on = 0;
                force_trim_rudder_position_set = 0;

                force_trim_x = ADC_RANGE / 2;
                force_trim_y = ADC_RANGE / 2 ;
                force_trim_rudder = ADC_RANGE / 2;
                cyclic_force_trim_state = 0;
                pedals_force_trim_state = 0;
              }
            }
          }
          else if ((i == PTT_BUTTON) && (PTT_KEYBOARD_PRESS == 1))
          {
            if (v == 1)
            {
              Keyboard.press(PTT_KEYBOARD_KEY_MOD);
              Keyboard.press(PTT_KEYBOARD_KEY);
            }
            else
            {
              Keyboard.releaseAll();
            }

          }
          else if ((i == PSEUDO_FORCE_TRIM_BUTTON) && (PSEUDO_FORCE_TRIM == 1) && (v == 1))
          {
            force_trim_on = !force_trim_on;
            controls_freezed = !controls_freezed;

            force_trim_rudder_on = !force_trim_rudder_on;
            force_trim_button_pressed = 1;
          }
          else
          {
            simchair.setButton(i, v);
            force_trim_button_pressed = 0;
          }
        }
      }
      else
      {
        simchair.setButton(i, v);
      }
    }
    b8stick_lastButtonState[i] = v;
  }
}

int parse_hat_trim (int x, int y, bool invert_x, bool invert_y)
{
  int hat_val;

    //int one_percent_range = ADC_RANGE / 100;
    int adj_step_x = one_percent_range * ATT_TRIM_STEP_X;
    int adj_step_y = one_percent_range * ATT_TRIM_STEP_Y;
    if (y > 145)
    {
      //hat up
      if (invert_y == 0)
      {
        cyclic_y_adj = cyclic_y_adj + adj_step_y;
        if ((FORCE_TRIM_BUTTON_MODE == "MOMENTARY") && (controls_freezed == 1))
        {
         // yval = yval + cyclic_y_adj;
          simchair.setYAxis(yval + cyclic_y_adj);
        }
      }
      else
      {
        cyclic_y_adj = cyclic_y_adj - adj_step_y;
        if ((FORCE_TRIM_BUTTON_MODE == "MOMENTARY") && (controls_freezed == 1))
        {
         // yval = yval + cyclic_y_adj;
          simchair.setYAxis(yval + cyclic_y_adj);
        }
      }
    }
    else if (y < 105)
    {
      //hat down
      if (invert_y == 0)
      {
       cyclic_y_adj = cyclic_y_adj - adj_step_y;
       if ((FORCE_TRIM_BUTTON_MODE == "MOMENTARY") && (controls_freezed == 1))
        {
         // yval = yval + cyclic_y_adj;
          simchair.setYAxis(yval + cyclic_y_adj);
        }
      }
      else
      {
        cyclic_y_adj = cyclic_y_adj + adj_step_y;
        if ((FORCE_TRIM_BUTTON_MODE == "MOMENTARY") && (controls_freezed == 1))
        {
         // yval = yval + cyclic_y_adj;
          simchair.setYAxis(yval + cyclic_y_adj);
        }
      }
    }
    else if (x > 145)
    {
      //hat right
      if (invert_x == 0)
      {
        cyclic_x_adj = cyclic_x_adj + adj_step_x;
        if ((FORCE_TRIM_BUTTON_MODE == "MOMENTARY") && (controls_freezed == 1))
        {
         // yval = yval + cyclic_y_adj;
          simchair.setXAxis(xval + cyclic_x_adj);
        }
      }
      else
      {
        cyclic_x_adj = cyclic_x_adj - adj_step_x;
        if ((FORCE_TRIM_BUTTON_MODE == "MOMENTARY") && (controls_freezed == 1))
        {
          simchair.setXAxis(xval + cyclic_x_adj);
        }
      }
    }
    else if (x < 105)
    {
      //hat left
      if (invert_x == 0)
      {
        cyclic_x_adj = cyclic_x_adj - adj_step_x;
        if ((FORCE_TRIM_BUTTON_MODE == "MOMENTARY") && (controls_freezed == 1))
        {
         // yval = yval + cyclic_y_adj;
          simchair.setXAxis(xval + cyclic_x_adj);
        }
      }
      else
      {
        cyclic_x_adj = cyclic_x_adj + adj_step_x;
        if ((FORCE_TRIM_BUTTON_MODE == "MOMENTARY") && (controls_freezed == 1))
        {
         // yval = yval + cyclic_y_adj;
          simchair.setXAxis(xval + cyclic_x_adj);
        }
      }
    }

}
