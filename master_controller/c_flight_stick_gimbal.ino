void setup_cyclic()
{
  if (is_device_connected(FLIGHT_STICK_GIMBAL_I2C_ADDRESS))
  {
    //initialize ADS1115 filters
    for (int thisReading = 0; thisReading < xy_readings; thisReading++)
    {
      buf_x[thisReading] = 0;
      buf_y[thisReading] = 0;
    }
    simchair.setXAxisRange(0, ADC_RANGE);
    simchair.setYAxisRange(0, ADC_RANGE);
    connected_devices |= (1 << DEVICE_CYCLIC);
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
  force_trim_x = physical_cyclic_center_x;
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
    ftcr = cyclic.readADC_SingleEnded(2) >> 5;
    if (ftcr == 255)
    {
      x_diff = 0;
      y_diff = 0;
      cyclic_x_adj = 0;
      cyclic_y_adj = 0;
      if (FSGIMBAL_INVERT_X == 1)
      {
        physical_cyclic_center_x = ADC_RANGE - physical_cyclic_center_x;
      }
      if (FSGIMBAL_INVERT_Y == 1)
      {
        physical_cyclic_center_y = ADC_RANGE - physical_cyclic_center_y;
      }
    }

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
    //Serial.println(ftcr);
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

    if (force_trim_on == 0) // button is not pressed
    {

      if ((cyclic_force_trim_state == 0) || (cyclic_force_trim_state == 2))
      {
       if (force_trim_x - (ADC_RANGE /2) - x < 0)
       {
          xval = x + x_diff + cyclic_x_adj;
          xval_prev = x;

       }
       //else if (x > force_trim_x)
       else if (force_trim_x + (ADC_RANGE /2) - x > 0)
       {
          xval = x - x_diff + cyclic_x_adj;
          xval_prev = x;
       }

       if (xval < 0)
       {
        xval = 0;
       }
       else if (xval > ADC_RANGE)
      {
        xval = ADC_RANGE;
      }


      if (force_trim_y - (ADC_RANGE /2) - y < 0)
       {
          yval = y + y_diff + cyclic_y_adj;
//          yval_prev = y;

       }
       //else if (x > force_trim_x)
       else if (force_trim_y + (ADC_RANGE /2) - y > 0)
       {
          yval = y - y_diff + cyclic_y_adj;
       //   yval_prev = y;
       }

       if (yval < 0)
       {
        yval = 0;
       }
       else if (yval > ADC_RANGE)
      {
        yval = ADC_RANGE;
      }
//                Serial.print(force_trim_y);
//        Serial.print(" ");
//        Serial.print(y_diff);
//        Serial.print(" ");
//        Serial.print(y);
//        Serial.print(" ");
//        Serial.println(yval);


        simchair.setXAxis(xval);
        simchair.setYAxis(yval);
      }
      if (cyclic_force_trim_state == 1) // after assigning a new center, wait for the stick to be returned to its mechanical center
      {
          //int one_percent_range = ADC_RANGE / 100;
          //int diff_x_center = x - physical_cyclic_center_x; // this is needed because of how the abs() works
         // int diff_y_center = y - physical_cyclic_center_y;


          //if (((abs(diff_x_center)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range)) && ((abs(diff_y_center)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range)))
          if (FORCE_TRIM_BUTTON_MODE == "HOLD")
          {
            if (force_trim_button_pressed == 0)
            {

              cyclic_force_trim_state = 2;
              x_diff = x - force_trim_x;
              y_diff = y - force_trim_y;
              cyclic_x_adj = 0;
              cyclic_y_adj = 0;
              //x_diff = abs(x_diff);
            }
          }
          else
          {
           // Serial.println(force_trim_on);
            if (controls_freezed == 0)
            {
              cyclic_force_trim_state = 2;
              x_diff = x - force_trim_x - cyclic_x_adj;
              y_diff = y - force_trim_y - cyclic_y_adj;

              cyclic_x_adj = 0;
              cyclic_y_adj = 0;
            }

          }


      }

    }
    else if (force_trim_on == 1) // button is pressed
    {
                     diffy = y - force_trim_y;
          diffx = xval - force_trim_x;
      force_trim_on = 0;
      if (cyclic_force_trim_state == 0) //1st trim button press - assign new center
      {
        force_trim_x = xval;//(x - ADC_RANGE / 2);//(physical_cyclic_center_x));
        force_trim_y = yval;//(y - ADC_RANGE / 2);//(physical_cyclic_center_y));
        cyclic_force_trim_state = 1;

      }
      if ((cyclic_force_trim_state == 2))// && (force_trim_on != prev_cyclic_force_trim_state)) //2nd button press - return center to its default position
      {
        //int one_percent_range = ADC_RANGE / 100;
        force_trim_x = xval;//force_trim_x + x/2;//(x - ADC_RANGE / 2);//(physical_cyclic_center_x));
        force_trim_y = yval;//force_trim_y + y/2;//(y - ADC_RANGE / 2);//(physical_cyclic_center_y));
        cyclic_force_trim_state = 1;
      }

    }

//  }
}
