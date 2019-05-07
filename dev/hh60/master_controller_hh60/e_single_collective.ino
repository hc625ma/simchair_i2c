void setup_single_engine_collective()
{
  if (!is_device_connected(SINGLE_COLLECTIVE_I2C_ADDRESS))
    return;

  simchair.setZAxisRange(SINGLE_COLLECTIVE_MIN, SINGLE_COLLECTIVE_MAX);
  simchair.setThrottleRange(SINGLE_COLLECTIVE_THR_MIN,SINGLE_COLLECTIVE_THR_MAX);//SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL, 1023);
  dev_single_engine_collective = 1;
}

void set_idle_stop_latch_state(uint16_t throttle)
{
  if (idle_stop_pressed[0] == 1)
  {
    throttle_latch_pressed = 1;
  }
  if (throttle > SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL + 100)
  {
    throttle_latch_pressed = 0;
  }

}

void poll_single_engine_collective()
{
  uint16_t z;
  uint16_t throttle;
  uint16_t raw_throttle;

  Wire.requestFrom(SINGLE_COLLECTIVE_I2C_ADDRESS, 4);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();

    z = b1;
    z = (z << 8) | b2;
    throttle = b3;
    throttle = (throttle << 8) | b4;
    if (throttle > 60000)
    {
      throttle = 0;
    }
    raw_throttle = throttle;
  }
  set_idle_stop_latch_state(raw_throttle);
  // uncomment the next line and turn your throttle to idle stop position to see SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL value
  //Serial.println(throttle);
  //Serial.println(z);
  if ((DCS_HUEY_IDLE_STOP_COMPAT_MODE_ENABLED == 1) && (coll_head_mode_sw_position == 0))
  {
      if (min_throttle_val[0] != SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL)
      {
         simchair.setThrottleRange(SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL, SINGLE_COLLECTIVE_THR_MAX);
         min_throttle_val[0] = SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL;
      }

      coll_head_idle_stop_compat_dcs (throttle,0,SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL,0);
      last_throttle_setting[0] = throttle;
      if (throttle < SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL)
      {
        throttle = SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL;
      }
      else
      {
        throttle_idle_cutoff[0] = 0;
      }
      if ((idle_stop_pressed[0] != 1) && (throttle_idle_cutoff[0] == 0))
      {
        simchair.setThrottle(throttle);
      }
      else if (idle_stop_pressed[0] == 1)
      {
        throttle_idle_cutoff[0] = 1;
      }
  }
  else
  {
    if (min_throttle_val[0] != 0)
    {
      simchair.setThrottleRange(SINGLE_COLLECTIVE_THR_MIN, SINGLE_COLLECTIVE_THR_MAX);
      min_throttle_val[0] = 0;
    }
    simchair.setThrottle(throttle);
  }

  if ((collective_hold_active == 0) && (collective_hold_position_set == 0))
  {
    simchair.setZAxis(z);
  }
  else if ((collective_hold_active == 1) && (collective_hold_position_set == 0))
  {
    collective_hold_position = z;
    collective_hold_position_set = 1;
  }
  else if ((collective_hold_active == 0) && (collective_hold_position_set == 1))
  {
    //
    int diff_z = z - collective_hold_position; // this is needed because of how the abs() works

   // if ((abs(diff_z)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range))
   if (abs(diff_z) < (2 * one_percent_coll_range))
    {
      simchair.setZAxis(z);
      collective_hold_position_set = 0;
    }
  }
  if (BUTTON_PRESS_ON_THROTTLE_CUTOFF == 1)
  {
    //Serial.println(raw_throttle);
    if ( ((THROTTLE_LATCH_MODE == "PHYSICAL") && (raw_throttle < (SINGLE_COLLECTIVE_THR_MIN + 15))) || (THROTTLE_LATCH_MODE == "TACTILE") && (raw_throttle < (SINGLE_COLLECTIVE_THR_MIN + 15)) && (throttle_latch_pressed == 1) )
    {
      //Serial.println(raw_throttle);
      if (physical_latch_button_state != 1)
      {
        simchair.setButton(PHYSICAL_LATCH_MOD_JOY_BUTTON - 1, 1);
        physical_latch_button_state = 1;
      }
    }
    else
    {
      if (physical_latch_button_state != 0)
      {
        simchair.setButton(PHYSICAL_LATCH_MOD_JOY_BUTTON - 1, 0);
        physical_latch_button_state = 0;
      }
    }
  }

}
