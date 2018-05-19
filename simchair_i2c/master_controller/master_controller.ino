#include <Wire.h>
#include <Joystick.h>
#include <Adafruit_ADS1015.h>

//Joystick_ b8stick(0x20,JOYSTICK_TYPE_GAMEPAD,
//  6, 0,                  // Button Count, Hat Switch Count
//  true, true, false,     // X and Y, but no Z Axis
//  false, false, false,   // No Rx, Ry, or Rz
//  false, false,          // No rudder or throttle
//  false, false, false);  // No accelerator, brake, or steering

Joystick_ simchair(0x20,0x04,32, 0,true, true, true, true, true, false, true, true, false, false, false);
Joystick_ simchair_aux1(0x21,0x04,32, 0,true, true, true, true, true, true, false, true, false, false, false);
//Joystick_ simchair(0x20,0x04,64, 0,true, true, true, true, true, true, true, true, true, true, true);
Adafruit_ADS1115 cyclic;
Adafruit_ADS1115 pedals(0x4A);

bool b8stick_lastButtonState[] = {0,0,0,0,0,0};
bool dev_b8stick = 0;
bool dev_cyclic = 0;
bool dev_simple_collective = 0;
bool dev_pedals = 0;
bool dev_cessna_engine_and_prop_controls = 0;
bool zero = 0;

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
    simchair.setXAxisRange(0, 4096);
    simchair.setYAxisRange(0, 4096);
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
    Serial.print("BANG");
    simchair.setRudderRange(0, 4096);
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
    simchair_aux1.setXAxis(rx);
    simchair_aux1.setYAxis(ry);
    for (byte i=0; i<6; i++)
    {
      bool v = (b >> i) & 1;
      if (v != b8stick_lastButtonState[i])
      {
        simchair.setButton(i,v);
      }
      b8stick_lastButtonState[i] = v;
    }
  }
}
void poll_cyclic()
{
    uint16_t x = cyclic.readADC_SingleEnded(0) >> 3;
    uint16_t y = cyclic.readADC_SingleEnded(1) >> 3;
   
    simchair.setXAxis(x);
    simchair.setYAxis(y);
}

void poll_pedals()
{
    uint16_t rudder = pedals.readADC_SingleEnded(0) >> 3;
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

