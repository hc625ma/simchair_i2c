void setup_pedals()
{
  Wire.beginTransmission(0X4A);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    //initialize ADS1115 filters
    for (int thisReading = 0; thisReading < xy_readings; thisReading++)
    {
      buf_rudder[thisReading] = 0;
    }
    if (INVERT_RUDDER == 1)
    {
      simchair.setRudderRange(ADC_RANGE, 0);
    }
    else
    {
      simchair.setRudderRange(0, ADC_RANGE);
    }
    dev_pedals = 1;
    pedals.begin();
    pedals.setGain(GAIN_ONE);

    physical_pedals_center = pedals.readADC_SingleEnded(0) >> (15 - ADS1115_RESOLUTION);
    if (INVERT_RUDDER == 1)
    {
      physical_pedals_center = ADC_RANGE - physical_pedals_center;
    }
  }
}

void poll_pedals()
{
  uint16_t rudder;
  if (RUDDER_FILTERING_ENABLED == 1)
  {
    total_rudder = total_rudder - buf_rudder[rudder_read_index];
    buf_rudder[rudder_read_index] = pedals.readADC_SingleEnded(0) >> (15 - ADS1115_RESOLUTION);
    total_rudder = total_rudder + buf_rudder[rudder_read_index];
    rudder_read_index = rudder_read_index + 1;
    if (rudder_read_index >= rudder_readings)
    {
      rudder_read_index = 0;
    }
    rudder = total_rudder / rudder_readings;

  }
  else
  {
    rudder = pedals.readADC_SingleEnded(0) >> (15 - ADS1115_RESOLUTION);
    //Serial.println(rudder); //uncomment to calibrate pedals
  }
  if (SENS_SWITCH_ENABLED == 1)
  {
    rudder = adjust_sensitivity(rudder, rudder_sens);
  }
  else
  {
    rudder = adjust_sensitivity(rudder, CUSTOM_RUDDER_SENS);
  }

  if (CYCLIC_HAS_CENTERING == 0) // CYCLIC WITHOUT SPRINGS
  {
    if (PSEUDO_FORCE_TRIM_EFFECTS_PEDALS == 1)
    {
      if (PSEUDO_FORCE_TRIM_RELEASE_MODE == "ADAPTIVE")
      {
        if ((force_trim_rudder_on == 0) && (force_trim_rudder_position_set == 0))
        {
          simchair.setRudder(rudder);
        }
        else if ((force_trim_rudder_on == 1) && (force_trim_rudder_position_set == 0))
        {
          force_trim_rudder = rudder;
          force_trim_rudder_position_set = 1;
        }
        else if ((force_trim_rudder_on == 0) && (force_trim_rudder_position_set == 1))
        {
          //int one_percent_range = ADC_RANGE / 100;
          int diff_rudder = rudder - force_trim_rudder; // this is needed because of how the abs() works
  
          if ((abs(diff_rudder)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range))
          {
            simchair.setRudder(rudder);
            force_trim_rudder_position_set = 0;
          }
        }
      }
      else //INSTANT mode
      {
        if (force_trim_rudder_on == 0)
        {
          simchair.setRudder(rudder);
        }
      }
    }
    else
    {
      simchair.setRudder(rudder);
    }
  }
  else if (CYCLIC_HAS_CENTERING == 1) // CYCLIC WITH SPRINGS
  {
//    Serial.print(force_trim_rudder_on);
//    Serial.print(" ");
//    Serial.print(pedals_force_trim_state);
//    Serial.print(" ");
//    Serial.print(physical_pedals_center);
//    Serial.print(" ");
//    Serial.println(rudder);
    
    if (force_trim_rudder_on == 0) // button is not pressed
    {
      
      if ((pedals_force_trim_state == 0) || (pedals_force_trim_state == 2))
      {
        uint16_t rudder_val = force_trim_rudder + (rudder - (ADC_RANGE / 2));

//        Serial.println(rudder_val);
        if ((rudder_val > 0) && (rudder_val < ADC_RANGE))
        {
          simchair.setRudder(rudder_val);
        }
        //prev_cyclic_force_trim_state = 0;
      }
      if (pedals_force_trim_state == 1) // after assigning a new center, wait for the stick to be returned to its mechanical center
      {
          //int one_percent_range = ADC_RANGE / 100;
          int diff_rudder_center = rudder - physical_pedals_center; // this is needed because of how the abs() works
          //Serial.println(diff_rudder_center);
         if ((abs(diff_rudder_center)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range))
         {
            
            pedals_force_trim_state = 2;
         }
          
      }
      
      
    }
    else if (force_trim_rudder_on == 1) // button is pressed
    {
      force_trim_rudder_on = 0;
      if (pedals_force_trim_state == 0) //1st trim button press - assign new center
      {
        force_trim_rudder = force_trim_rudder + (rudder - (physical_pedals_center));
        pedals_force_trim_state = 1;
      }
      if ((pedals_force_trim_state == 2))//2nd button press - return center to its default position
      {
        force_trim_rudder = force_trim_rudder + (rudder - (physical_pedals_center));
        pedals_force_trim_state = 1;
      }
      
    }
  }

}
