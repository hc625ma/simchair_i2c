void setup_cyclic()
{
  Wire.beginTransmission(0x48);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    //initialize ADS1115 filters
    for (int thisReading = 0; thisReading < xy_readings; thisReading++)
    {
      buf_x[thisReading] = 0;
      buf_y[thisReading] = 0;
    }
    simchair.setXAxisRange(0, ADC_RANGE);
    simchair.setYAxisRange(0, ADC_RANGE);
    dev_cyclic = 1;
    cyclic.begin();
    cyclic.setGain(GAIN_ONE);
  }

  physical_cyclic_center_x = cyclic.readADC_SingleEnded(0) >> (15 - ADS1115_RESOLUTION);
  physical_cyclic_center_y = cyclic.readADC_SingleEnded(1) >> (15 - ADS1115_RESOLUTION);
  if (FSGIMBAL_INVERT_X == 1)
  {
    physical_cyclic_center_x = ADC_RANGE - physical_cyclic_center_x;
  }
  if (FSGIMBAL_INVERT_Y == 1)
  {
    physical_cyclic_center_y = ADC_RANGE - physical_cyclic_center_y;
  }
}

void poll_cyclic()
{
  uint32_t x, y;
  if (XY_FILTERING_ENABLED == 1)
  {
    total_x = total_x - buf_x[xy_read_index];
    total_y = total_y - buf_y[xy_read_index];
    buf_x[xy_read_index] = cyclic.readADC_SingleEnded(0) >> (15 - ADS1115_RESOLUTION);
    buf_y[xy_read_index] = cyclic.readADC_SingleEnded(1) >> (15 - ADS1115_RESOLUTION);

    total_x = total_x + buf_x[xy_read_index];
    total_y = total_y + buf_y[xy_read_index];
    xy_read_index = xy_read_index + 1;

    if (xy_read_index >= xy_readings)
    {
      // ...wrap around to the beginning:
      xy_read_index = 0;
    }
    

    x = total_x / xy_readings;
    y = total_y / xy_readings;


  }
  else
  {
    x = cyclic.readADC_SingleEnded(0) >> (15 - ADS1115_RESOLUTION);
    y = cyclic.readADC_SingleEnded(1) >> (15 - ADS1115_RESOLUTION);

    if (FSGIMBAL_INVERT_X == 1)
    {
      x = ADC_RANGE - x;
    }
    if (FSGIMBAL_INVERT_Y == 1)
    {
      y = ADC_RANGE - y;
    }
    // uncomment next 3 lines for cyclic calibration
//    Serial.print(x);
//    Serial.print(" ");
//    Serial.println(y);
  }
  
  if (SENS_SWITCH_ENABLED == 1)
  {
    x = adjust_sensitivity(x, cyclic_sens);
    y = adjust_sensitivity(y, cyclic_sens);
  }
  else
  {
    x = adjust_sensitivity(x, CUSTOM_CYCLIC_SENS);
    y = adjust_sensitivity(y, CUSTOM_CYCLIC_SENS);
  }

  if (CYCLIC_HAS_CENTERING == 0) // CYCLIC WITHOUT SPRINGS
  {
    if (PSEUDO_FORCE_TRIM_RELEASE_MODE == "ADAPTIVE")
    {
      if ((force_trim_on == 0) && (force_trim_position_set == 0))
      {
        simchair.setXAxis(x);
        simchair.setYAxis(y);
      }
      else if ((force_trim_on == 1) && (force_trim_position_set == 0))
      {
        force_trim_x = x;
        force_trim_y = y;
        force_trim_position_set = 1;
      }
      else if ((force_trim_on == 0) && (force_trim_position_set == 1))
      {
        //int one_percent_range = ADC_RANGE / 100;
        int diff_x = x - force_trim_x; // this is needed because of how the abs() works
        int diff_y = y - force_trim_y;
  
        if (((abs(diff_x)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range)) && ((abs(diff_y)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range)))
        {
          simchair.setXAxis(x);
          simchair.setYAxis(y);
          force_trim_position_set = 0;
        }
      }
    }
    else //INSTANT mode
    {
      if (force_trim_on == 0)
      {
        simchair.setXAxis(x);
        simchair.setYAxis(y);
      }
    }
  }
  else // CYCLIC WITH SPRINGS
  {
//    Serial.print(force_trim_on);
//    Serial.print(" ");
//    Serial.print(force_trim_position_set);
//    Serial.print(" ");
//    Serial.print(cyclic_force_trim_state);
//    Serial.println(" ");
    

    if (force_trim_on == 0) // button is not pressed
    {
      
      if ((cyclic_force_trim_state == 0) || (cyclic_force_trim_state == 2))
      {
        int32_t xval = force_trim_x + (x - (ADC_RANGE / 2));
        int32_t yval = force_trim_y + (y - (ADC_RANGE / 2));
                   
        if(xval < 50)
        {
          xval = 50; 
        }
        else if (xval > (ADC_RANGE))
        {
          xval = ADC_RANGE;
        }
        if(yval < 50)
        {  
          yval = 50; 
        }
          if ((yval > (ADC_RANGE)))
        {
          yval = ADC_RANGE;
                  
        }

        simchair.setXAxis(xval);
        simchair.setYAxis(yval);
      }
      if (cyclic_force_trim_state == 1) // after assigning a new center, wait for the stick to be returned to its mechanical center
      {
          //int one_percent_range = ADC_RANGE / 100;
          int diff_x_center = x - physical_cyclic_center_x; // this is needed because of how the abs() works
          int diff_y_center = y - physical_cyclic_center_y;
  
          if (((abs(diff_x_center)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range)) && ((abs(diff_y_center)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range)))
          {
            
            cyclic_force_trim_state = 2;
          }
          
      }
      
    }
    else if (force_trim_on == 1) // button is pressed
    {
      force_trim_on = 0;
      if (cyclic_force_trim_state == 0) //1st trim button press - assign new center
      {
        force_trim_x = x;
        force_trim_y = y;
        cyclic_force_trim_state = 1;

      }
      if ((cyclic_force_trim_state == 2))// && (force_trim_on != prev_cyclic_force_trim_state)) //2nd button press - return center to its default position
      {
        //int one_percent_range = ADC_RANGE / 100;
        force_trim_x = force_trim_x + (x - (physical_cyclic_center_x));
        force_trim_y = force_trim_y + (y - (physical_cyclic_center_y));
        cyclic_force_trim_state = 1;
      }
      
    }

  }
}
