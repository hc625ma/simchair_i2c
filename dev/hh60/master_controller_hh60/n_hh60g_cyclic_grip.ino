

//#define HH60G_GRIP_SETUP

// bottom left
#define HH60G_GRIP_HAT1_LEFT  32
#define HH60G_GRIP_HAT1_RIGHT 64
#define HH60G_GRIP_HAT1_UP    16
#define HH60G_GRIP_HAT1_DOWN  128
#define HH60G_GRIP_HAT1_PUSH  1
// top left
#define HH60G_GRIP_HAT2_LEFT  2
#define HH60G_GRIP_HAT2_RIGHT 1
#define HH60G_GRIP_HAT2_UP    128
#define HH60G_GRIP_HAT2_DOWN  4
// slider
#define HH60G_GRIP_HAT3_UP    8
#define HH60G_GRIP_HAT3_DOWN  16

bool hh60g_cyclic_grip_lastButtonState[13];

bool hh60g_hatsw_btn_state = 0;

void setup_hh60g_cyclic_grip()
{
 
    if (!is_device_connected(HH60G_CYCLIC_GRIP_HAT_BOARD_ADDRESS))
    return;
    dev_hh60g_cyclic_grip = 1;
  
}

void poll_hh60g_cyclic_grip()
{
  uint8_t h1;
  uint8_t h2;
  uint8_t h3;
  uint8_t b;
  //hot swap support for cyclic grip - a very useful feature
//    if (!is_device_connected(HH60G_CYCLIC_GRIP_HAT_BOARD_ADDRESS))
//      return;
//      
  Wire.requestFrom(HH60G_CYCLIC_GRIP_HAT_BOARD_ADDRESS, 3);
  while (Wire.available())
  {
    h1 = Wire.read();
    h2 = Wire.read();
    h3 = Wire.read();

#if defined (HH60G_GRIP_SETUP)
    Serial.print(h1);
    Serial.print(" ");
    Serial.print(h2);
    Serial.print(" ");
    Serial.print(h3);
    Serial.print(" ");
#endif
  }

  Wire.requestFrom(HH60G_CYCLIC_GRIP_BTN_BOARD_ADDRESS, 1);
  while (Wire.available())
  {
    b = Wire.read();
    
    #if defined (HH60G_GRIP_SETUP)
      Serial.println(b);
    #endif
  }

set_hh60g_hat_val(h3,h2,h1);
set_btns(b);

}

void set_hh60g_hat_val (uint8_t h1,uint8_t h2,uint8_t h3)
{
  if (h1 == HH60G_GRIP_HAT1_LEFT)
  {
    simchair.setHatSwitch(0,270);
  }
  else if (h1 == HH60G_GRIP_HAT1_RIGHT)
  {
    simchair.setHatSwitch(0, 90);
  }
  else if (h1 == HH60G_GRIP_HAT1_UP)
  {
    simchair.setHatSwitch(0,0);
  }
  else if (h1 == HH60G_GRIP_HAT1_DOWN)
  {
    simchair.setHatSwitch(0,180);
  }
  else if (h1 == HH60G_GRIP_HAT1_PUSH)
  {
    if (h1 != hh60g_cyclic_grip_lastButtonState[10])
    {
      if (SENS_SWITCH_ENABLED == 1)
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
              force_trim_y = ADC_RANGE / 2;
              force_trim_rudder = ADC_RANGE / 2;
              cyclic_force_trim_state = 0;
              pedals_force_trim_state = 0;
            }
          }
          hh60g_cyclic_grip_lastButtonState[10] = 1;
      }
      else
      {
        simchair.setButton(10,1);
        hh60g_cyclic_grip_lastButtonState[10] = 1;
      }
    }
  }
  else
  {
    simchair.setHatSwitch(0,JOYSTICK_HATSWITCH_RELEASE);
      if (h1 != hh60g_cyclic_grip_lastButtonState[10])
      {
        hh60g_hatsw_btn_state = 0;
        simchair.setButton(10,0);
        hh60g_cyclic_grip_lastButtonState[10] = 0;
      }
  }

  if (h2 == HH60G_GRIP_HAT2_LEFT)
  {
    simchair.setHatSwitch(1,270);
  }
  else if (h2 == HH60G_GRIP_HAT2_RIGHT)
  {
    simchair.setHatSwitch(1,90);
  }
  else if (h2 == HH60G_GRIP_HAT2_UP)
  {
    simchair.setHatSwitch(1,0);
  }
  else if (h2 == HH60G_GRIP_HAT2_DOWN)
  {
    simchair.setHatSwitch(1,180);
  }
  else
  {
    simchair.setHatSwitch(1,JOYSTICK_HATSWITCH_RELEASE);
  }
  
  if ((h3 == HH60G_GRIP_HAT3_UP) || (h3 == 40) || (h3 == 12))
  {
    simchair.setButton(11,1);
    hh60g_cyclic_grip_lastButtonState[11] = 1;
  }
  else if ((h3 == HH60G_GRIP_HAT3_DOWN) || (h3 == 48) || (h3 == 20))
  {
    simchair.setButton(12,1);
    hh60g_cyclic_grip_lastButtonState[12] = 1;
  }
  else if (h3 != hh60g_cyclic_grip_lastButtonState[11])
  {
    simchair.setButton(11,0);
    hh60g_cyclic_grip_lastButtonState[11] = 0;
  }
  else if (h3 != hh60g_cyclic_grip_lastButtonState[12])
  {
    simchair.setButton(12,0);
    hh60g_cyclic_grip_lastButtonState[12] = 0;
  }
  
}



void set_btns (uint8_t b)
{
  for (byte i = 0; i <= 8; i++)
  {
    bool v = (b >> i) & 1;
    if (v != hh60g_cyclic_grip_lastButtonState[i])
    {
      //Serial.println(i);
      if (i != HH60G_CYCLIC_GRIP_PSEUDO_FORCE_TRIM_BUTTON)
      {
        simchair.setButton(i,v);
      }
      else if ((i == HH60G_CYCLIC_GRIP_PSEUDO_FORCE_TRIM_BUTTON) && (PSEUDO_FORCE_TRIM == 1) && (v == 1))
      {
        //Serial.println("BOOM");
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
    hh60g_cyclic_grip_lastButtonState[i] = v;
  }
}

