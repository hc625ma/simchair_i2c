//version 0.2
#include <Wire.h>
#include <Joystick.h>
#include <Adafruit_ADS1015.h>
#include <Keyboard.h>



//Joystick_ b8stick(0x20,JOYSTICK_TYPE_GAMEPAD,
//  6, 0,                  // Button Count, Hat Switch Count
//  true, true, false,     // X and Y, but no Z Axis
//  false, false, false,   // No Rx, Ry, or Rz
//  false, false,          // No rudder or throttle
//  false, false, false);  // No accelerator, brake, or steering

Joystick_ simchair(0x20,0x04,32, 1,true, true, true, true, true, false, true, true, false, false, false);
Joystick_ simchair_aux1(0x21,0x04,32, 0,true, true, true, true, true, true, false, true, false, false, false);
//Joystick_ simchair(0x20,0x04,64, 0,true, true, true, true, true, true, true, true, true, true, true);
Adafruit_ADS1115 cyclic;
Adafruit_ADS1115 pedals(0x4A);

//<CONFIGURATION>
//----------------------------------------------------------------------------------------------

#define B8_POT_MODE "HAT_SW" // HAT_SW or ANALOG

#define SENS_ADJ_METHOD  "LINEAR"

#define ADS1115_RESOLUTION 15 //bits, min 12, max 15

// use a button on cyclic to switch sensitivity in flight.
// this kind of functionality is not available in a real aircraft, however
// it may somewhat resemble simulated force trim.

#define SENS_DEVICE "b8_stick"
#define SENS_SWITCH_ENABLED 1
#define SENS_SWITCH_BUTTON 0 //0 is the first button

// sens. switch behavior available options: "FORCE_TRIM" or "TOGGLE"
// FORCE_TRIM will reduce sensitivity to CYCLIC_SENS and RUDDER_SENS
// while SENSITIVITY_SWITCH_BUTTON is pressed, "TOGGLE" will act
// as a regular switch

#define SENS_SWITCH_MODE "TOGGLE" 

// if SENS_SWITCH is disabled, these values will be used;
// set to 100 for full axis range
#define CUSTOM_CYCLIC_SENS 80
#define CUSTOM_RUDDER_SENS 80

#define XY_FILTERING_ENABLED 0
#define RUDDER_FILTERING_ENABLED 0

#define PTT_KEYBOARD_PRESS 0 // set to 1 to enable keyboard combination press instead of a joystick button
#define PTT_BUTTON 4 //1st button is 0, 4 should be PTT trigger position

// this should press CTRL + Q
#define PTT_KEYBOARD_KEY 'q'
char PTT_KEYBOARD_KEY_MOD = KEY_LEFT_CTRL;
//----------------------------------------------------------------------------------------------
//</CONFIGURATION>


int cyclic_sens = 100;
int rudder_sens = 100;

bool b8stick_lastButtonState[] = {0,0,0,0,0,0};
bool dev_b8stick = 0;
bool dev_cyclic = 0;
bool dev_simple_collective = 0;
bool dev_pedals = 0;
bool dev_cessna_engine_and_prop_controls = 0;
bool zero = 0;

//ADS1115 filter variables
const int xy_readings = 6;
const int rudder_readings = 8;
int buf_x[xy_readings]; 
int buf_y[xy_readings]; 
int buf_rudder[rudder_readings];
int xy_read_index = 0; // the index of the current reading
int rudder_read_index = 0;
long total_x = 0;  // the running total
long total_y = 0;
long total_rudder = 0;

void setup() 
{
  delay(5000);
  simchair.begin();
  simchair_aux1.begin();
  //detect and configure connected peripherals
  Wire.begin(); 
  setup_b8stick();
  setup_cyclic();
  setup_simple_collective();
  setup_pedals();
  setup_cessna_engine_and_prop_controls();
  if (PTT_KEYBOARD_PRESS == 1)
  {
    Keyboard.begin();
  }
}



void loop() 
{
  if (dev_b8stick == 1)
  {
      poll_b8stick();    
  }
  if (dev_cyclic == 1)
  {
    poll_cyclic();    
  }
  if (dev_simple_collective == 1)
  {
    poll_simple_collective();    
  }
  if (dev_pedals == 1)
  {
    poll_pedals();    
  }
  if (dev_cessna_engine_and_prop_controls == 1)
  {
    poll_cessna_engine_and_prop_controls();    
  }
}


int adjust_sensitivity (int val, int param)
{
  if ((SENS_ADJ_METHOD == "LINEAR") && (param < 100))
  {
    int percent = param;
    // this will simply limit your controls throw range by the given percent,
    // and map full ADC range for it.
    // may be a good option for flying helicopters in x-plane
    int center = (0.5 + pow(2,ADS1115_RESOLUTION))/2;
    int adj_range = ((0.5 + pow(2,ADS1115_RESOLUTION)) / 100 * percent);
    val = map(val,0,0.5 + pow(2,ADS1115_RESOLUTION),center - (adj_range / 2), center + (adj_range / 2));
  }
  
  return val;
}

void setup_b8stick()
{
  //MKIII B8 stick
  Wire.beginTransmission(20);
  int error = Wire.endTransmission();
  if(error == 0)
  {
    simchair_aux1.setXAxisRange(0, 255);
    simchair_aux1.setYAxisRange(0, 255);
    dev_b8stick = 1;
  }
}

void setup_cyclic()
{
  Wire.beginTransmission(0x48);
  int error = Wire.endTransmission();
  if(error == 0)
  {
    //initialize ADS1115 filters
    for (int thisReading = 0; thisReading < xy_readings; thisReading++) 
    {
      buf_x[thisReading] = 0;
      buf_y[thisReading] = 0;
    }
    simchair.setXAxisRange(0, 0.5 + pow(2,ADS1115_RESOLUTION));
    simchair.setYAxisRange(0, 0.5 + pow(2,ADS1115_RESOLUTION));
    dev_cyclic = 1;
    cyclic.begin();
    cyclic.setGain(GAIN_ONE);
  }
}

void setup_pedals()
{
  Wire.beginTransmission(0X4A);
  int error = Wire.endTransmission();
  if(error == 0)
  {
    //initialize ADS1115 filters
    for (int thisReading = 0; thisReading < xy_readings; thisReading++) 
    {
      buf_rudder[thisReading] = 0;
    }
    simchair.setRudderRange(0, 0.5 + pow(2,ADS1115_RESOLUTION));
    dev_pedals = 1;
    pedals.begin();
    pedals.setGain(GAIN_ONE);
  }
}

void setup_simple_collective()
{
  Wire.beginTransmission(10);
  int error = Wire.endTransmission();
  if(error == 0)
  {
    simchair.setZAxisRange(0, 961);
    simchair.setThrottleRange(0, 982);
    dev_simple_collective = 1;
    //Serial.println("BOOM");
  }
}

void setup_cessna_engine_and_prop_controls()
{
  Wire.beginTransmission(11);
  int error = Wire.endTransmission();
  if(error == 0)
  {
    simchair_aux1.setRxAxisRange(0, 1023);
    simchair_aux1.setRyAxisRange(0, 1023);
    simchair_aux1.setThrottleRange(0, 1023);
    dev_cessna_engine_and_prop_controls = 1;
  }
}

void poll_b8stick()
{
  //hot swap support for cyclic grip - a very useful feature
  Wire.beginTransmission(20);
  int error = Wire.endTransmission();
  if(error == 0)
  {
    uint8_t rx;
    uint8_t ry;
    uint8_t b;
    Wire.requestFrom(20, 3);
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
      int16_t hat_val;

      if (ry > 145)
      {
        if (rx > 145)
        {
          hat_val = 45;
        }
        else if (rx < 105)
        {
          hat_val = 315;
        }
        else
        {
          hat_val = 0;
        }
      }
      else if (ry < 105)
      {
        if (rx > 160)
        {
          hat_val = 135;
        }
        else if (rx < 105)
        {
          hat_val = 225;
        }
        else
        {
          hat_val = 180;
        }
      }
      else if (rx > 145)
      {
        hat_val = 90;
      }
      else if (rx < 105)
      {
        hat_val = 270;
      }
      else
      {
        hat_val = JOYSTICK_HATSWITCH_RELEASE;
      }
      simchair.setHatSwitch(0,hat_val);
    }
    for (byte i=0; i<6; i++)
    {
      bool v = (b >> i) & 1;
      if (v != b8stick_lastButtonState[i])
      {
        if (((SENS_SWITCH_ENABLED == 1) && (SENS_DEVICE == "b8_stick")) || (PTT_KEYBOARD_PRESS == 1))
        {
          if ((i != SENS_SWITCH_BUTTON) && (i != PTT_BUTTON))
          {
            simchair.setButton(i,v);
          }
          else
          {
            if (((SENS_SWITCH_MODE == "FORCE_TRIM") || ((SENS_SWITCH_MODE == "TOGGLE") && (v == 1))) && (i != PTT_BUTTON))
            {
              if (cyclic_sens == 100)
              {
                cyclic_sens = CUSTOM_CYCLIC_SENS;
                rudder_sens = CUSTOM_RUDDER_SENS;
              }
              else
              {
                cyclic_sens = 100;
                rudder_sens = 100;
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
            else
            {
              simchair.setButton(i,v);
            }
          }
        }
        else
        {
          simchair.setButton(i,v);
        }
      }
      b8stick_lastButtonState[i] = v;
    }
  }
}

void poll_cyclic()
{
  uint16_t x,y;
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
  }

  if (SENS_SWITCH_ENABLED == 1)
  {
    x = adjust_sensitivity(x,cyclic_sens);
    y = adjust_sensitivity(y,cyclic_sens);
  }
  else
  {
    x = adjust_sensitivity(x,CUSTOM_CYCLIC_SENS);
    y = adjust_sensitivity(y,CUSTOM_CYCLIC_SENS);
  }
  simchair.setXAxis(x);
  simchair.setYAxis(y);
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
  }
  if (SENS_SWITCH_ENABLED == 1)
  {
    rudder = adjust_sensitivity(rudder,rudder_sens);
  }
  else
  {
    rudder = adjust_sensitivity(rudder,CUSTOM_RUDDER_SENS);
  }
  simchair.setRudder(rudder);
    
}

void poll_simple_collective()
{
    uint16_t z;
    uint16_t throttle;
    
    Wire.requestFrom(10, 4);
    while (Wire.available()) 
    { 
      byte b1 = Wire.read(); // receive a byte as character
      byte b2 = Wire.read();
      byte b3 = Wire.read(); // receive a byte as character
      byte b4 = Wire.read();
  
      z = b1;
      z = (z << 8)|b2;
      throttle = b3;
      throttle = (throttle << 8)|b4;

    }

    simchair.setZAxis(z);
    simchair.setThrottle(throttle);
}

void poll_cessna_engine_and_prop_controls()
{
  uint16_t rx,ry,throttle;
  
  Wire.requestFrom(11, 6);
  while (Wire.available()) 
  { 
    Serial.println("test");
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();
    byte b5 = Wire.read(); // receive a byte as character
    byte b6 = Wire.read();
    //Serial.println(b1);
    throttle = b1;
    throttle = (throttle << 8)|b2;
    rx = b3;
    rx = (rx << 8)|b4;
    ry = b5;
    ry = (ry << 8)|b6;  
  }
    //Serial.println(rx);
    simchair.setRxAxis(rx);
    simchair.setRyAxis(ry);
    simchair.setThrottle(throttle);
}



