// This file contains all user configurable parameters. This allows you to keep your settings when updating software.


// <GLOBAL>

  #define ADS1115_RESOLUTION 15 //bits, min 12, max 15

// <CYCLIC>

  #define FSGIMBAL_INVERT_X 1
  #define FSGIMBAL_INVERT_Y 0  

  #define B8_POT_MODE "HAT_SW" // HAT_SW or ANALOG
  
  
  #define PSEUDO_FORCE_TRIM 1
  // this will freeze cyclic in-place until you press the button again.
  // this is not exactly how it works in a real helicopter (force trim only provides centering forces, not disables cyclic input),
  // but is useful in a sim and probably is the closest thing we can get on a non-FFB stick.
  #define PSEUDO_FORCE_TRIM_BUTTON 1
  // this will disable a choosen button and dedicate it to force trim. Top button in Huey, mid button in 407
  #define PSEUDO_FORCE_TRIM_EFFECTS_PEDALS 1 //in a real heli force trim affects both cyclic and pedals, this is a default behavior
  #define FORCE_TRIM_BUTTON_MODE "HOLD"// "MOMENTARY" or "HOLD" - use "MOMENTARY" for non-centering cyclic

  #define SENS_SWITCH_TRIM_RESET 0 // DEPRECATED - USE FTCR INSTEAD - release the trim when you press SENS SWITCH
  
  // use a button on cyclic to switch sensitivity in flight.
  // this kind of functionality is not available in a real aircraft, however
  // it may somewhat resemble simulated force trim.
  
  #define SENS_DEVICE "b8_stick"
  #define SENS_SWITCH_ENABLED 1
  #define SENS_SWITCH_BUTTON 0 //0 is the first button
  
  #define B8_HAT_SWITCH_MODE "ATT_TRIM" //"ATT_TRIM","HAT", or "BOTH", it will adjust the position of the trimmed cyclic with hat(Bell 407-style), only use "BOTH" with FORCE_TRIM_BUTTON_MODE "MOMENTARY"
  #define ATT_TRIM_STEP_Y 0.02 //0.05 //IN PERCENTS OF AXIS LENGHT, 0.05 for non-centering cyclic, 0.02 for a spring-loaded one
  #define ATT_TRIM_STEP_X 0.02 //0.05 //IN PERCENTS OF AXIS LENGHT
  #define INVERT_HAT_TRIM_X 0
  #define INVERT_HAT_TRIM_Y 1
  
  // if SENS_SWITCH is disabled, these values will be used;
  // set to 100 for full axis range
  #define CUSTOM_CYCLIC_SENS 80
  
  #define XY_FILTERING_ENABLED 0 // this decreases jitter, but increases input lag; recommended setting is 0
  
  #define PTT_KEYBOARD_PRESS 0 // set to 1 to enable keyboard combination press instead of a joystick button
  #define PTT_BUTTON 4 //1st button is 0, 4 should be PTT trigger position
  // this should press CTRL + Q
  #define PTT_KEYBOARD_KEY 'q'
  char PTT_KEYBOARD_KEY_MOD = KEY_LEFT_CTRL;
  
// </CYCLIC>

// <PEDALS>

  #define INVERT_RUDDER 0
  #define RUDDER_FILTERING_ENABLED 0
  #define CUSTOM_RUDDER_SENS 80 // this value will be used if SENS_SWITCH is disabled

// </PEDALS>

// <COLLECTIVE>

  #define COLLECTIVE_HOLD_ENABLED 1 // this will hold your collective in place until you press the button again and return the lever to the initial position
  #define HUEY_HEAD_COLLECTIVE_HOLD_BUTTON 13
  #define AB412_HEAD_COLLECTIVE_HOLD_BUTTON 2
  
  //for sims that do not support axis movement below idle stop, sends the throttle up/down key when holding idle stop and rotating throttle simultaneously
  //not 100% realistic, yet better than nothing
  #define DCS_HUEY_IDLE_STOP_COMPAT_MODE_ENABLED 1 // ASSIGN IDLE STOP IN_GAME WHILE IN MODE 0 (MODE SWITCH CENTERED)!

  byte coll_head_idle_stop_compat_throttle_up_keys[] = {KEY_PAGE_DOWN,'z'};
  byte coll_head_idle_stop_compat_throttle_down_keys[] = {KEY_PAGE_UP,'x'};

  #define COLL_HEAD_DCS_HUEY_COMPAT_MODE_BUTTON_HOLD 50 // how long to hold throttle up/down buttons, adjusted depending on idle stop axis range (smaller range- bigger hold time and vice versa)

  #define BUTTON_PRESS_ON_THROTTLE_CUTOFF 1// this feature will send a joystick button press when the throttle is fully closed
  #define THROTTLE_LATCH_MODE "PHYSICAL" // PHYSICAL for physical latch mod or TACTILE for levers with tactile marks
  #define PHYSICAL_LATCH_MOD_JOY_BUTTON 32 // joystick button number as seen in joy.cpl for the 1st Leonardo in the list
 // #define THROTTLE_MIN_AXIS_VALUE 0 // check this if the default doesn't work, print raw_throttle value in e_single_collective.ino and set this line to its value, corresponding to a fully closed throttle

  // all pots are different! Please look into the corresponding page for your lever
  // uncomment Serial.print's and set values below. To invert an axis, set MIN to MAX and vice versa.
  
  #define SINGLE_COLLECTIVE_MIN 63
  #define SINGLE_COLLECTIVE_MAX 1023
  #define SINGLE_COLLECTIVE_THR_MIN 0
  #define SINGLE_COLLECTIVE_THR_MAX 1023

  #define TWIN_COLLECTIVE_MIN 1023
  #define TWIN_COLLECTIVE_MAX 0
  #define TWIN_COLLECTIVE_THR1_MIN 0
  #define TWIN_COLLECTIVE_THR1_MAX 1023
  #define TWIN_COLLECTIVE_THR2_MIN 0
  #define TWIN_COLLECTIVE_THR2_MAX 1023

  #define SIMPLE_COLLECTIVE_MIN 0
  #define SIMPLE_COLLECTIVE_MAX 1023
  #define SIMPLE_COLLECTIVE_THR_MIN 0
  #define SIMPLE_COLLECTIVE_THR_MAX 1023
  
// </COLLECTIVE>

// <AB412 COLL HEAD>

  #define AB412_COLL_HEAD_ROTARY_POTS  2 // 0, 1 or 2
  #define AB412_COLL_HEAD_LEFT_HAT_DIRECTIONS 4
  #define AB412_COLL_HEAD_RIGHT_HAT_DIRECTIONS 8
  #define AB412_IDLE1_SW_KEY  'y'
  #define AB412_IDLE2_SW_KEY  'u'
  #define AB412_STARTER1_SW_KEY  'i'
  #define AB412_STARTER2_SW_KEY  'o'
  #define COLL_HEAD_SW_HOLD_TIMEOUT 200
  #define AB412_COLL_HEAD_MODE_SWITCH 18 // 3-way fixed switch only; first (lower number) switch button must be defined there; remove it and next button from everywhere else; set to 0 to disable MODE SWITCH

  #define TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL 50
  #define TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE2_VAL 50
// This is the compatibility mode that can send keyboard presses instead of joystick button pressed if you need so. This was needed because of how old 412 compat mode worked
// and will probably be removed in the future, because there's now a much better lua script.
  byte ab412_coll_head_idle_stop_buttons[] = {17};// for Huey compat mode

 // COLLECTIVE SWITCHPANEL SWITCH MATRIX - NOW SUPPORTS UP TO 32 BUTTONS + 10 ADDITIONAL BUTTONS FOR TYPES 3 AND 4
 // 4 SWITCH TYPES ARE SUPPORTED: 
 // 1: button
 // 2: momentarily press button
 // 3: selector button (button + middle button press when switch is centered)
 // 4: selector momentary press button (momentary press button + middle button momentary press when switch is centered)
 // ASSIGN THE BUTTON AFTER TYPES 3 AND 4 TO TYPE 5 (SLAVE) AND SET JOYSTICK BUTTON YOU WANT TO BE THE MIDDLE BUTTON IN 3RD COLUMN! 
 // BUTTONS FROM 33 TO 42 ARE RESERVED FOR MIDDLE BUTTONS

  //0 - disabled, 1 - button, 2 - 3 - selector_button, 4 - selector, 5 - slave
  const sw_matrix ab412_switch_matrix[] PROGMEM = 
  {
  // i  t  m  is  i - id, t - type, m - middle button for types 3 and 4
    {1, 1, 0},
    {2, 1, 0},
    {3, 2, 0},
    {4, 2, 0},
    {5, 2, 0},
    {6, 2, 0},
    {7, 2, 0},
    {8, 2, 0},
    {9, 3, 33},
    {10,5, 33},
    {11,3, 34},
    {12,5, 34},
    {13,3, 35},
    {14,5, 35},
    {15,4, 36},
    {16,5, 36},
    {17,1, 0},
    {18,0, 0},
    {19,0, 0},
    {20,0, 0},
    {21,0, 0},
    {22,0, 0},
    {23,0, 0},
    {24,0, 0},
    {25,0, 0},
    {26,0, 0},
    {27,0, 0},
    {28,0, 0},
    {29,0, 0},
    {30,0, 0},
    {31,0, 0},
    {32,0, 0},

  };
//  
// </AB412 COLL HEAD>

// <HUEY COLL HEAD>

  #define HUEY_COLL_HEAD_HAT_DIRECTIONS 4
  #define HUEY_COLL_HEAD_MODE_SWITCH 3 // 3-way fixed switch only; first (lower number) switch button must be defined there; remove it and next button from everywhere else; set to 0 to disable MODE SWITCH
  
  byte huey_coll_head_idle_stop_buttons[] = {11};
   
  #define COLL_HEAD_IDLE_STOP_COMPAT_TRESHOLD 5 // should be larger than jitter of your throttle pot
  #define SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL 137 //find it out with uncommenting Serial.print(throttle) in poll_single_engine_collective sub, see below

 // COLLECTIVE SWITCHPANEL SWITCH MATRIX - NOW SUPPORTS UP TO 32 BUTTONS + 10 ADDITIONAL BUTTONS FOR TYPES 3 AND 4
 // 4 SWITCH TYPES ARE SUPPORTED: 
 // 1: button
 // 2: momentarily press button
 // 3: selector button (button + middle button press when switch is centered)
 // 4: selector momentary press button (momentary press button + middle button momentary press when switch is centered)
 // ASSIGN THE BUTTON AFTER TYPES 3 AND 4 TO TYPE 5 (SLAVE) AND SET JOYSTICK BUTTON YOU WANT TO BE THE MIDDLE BUTTON IN 3RD COLUMN! 
 // BUTTONS FROM 33 TO 42 ARE RESERVED FOR MIDDLE BUTTONS
  
  const sw_matrix huey_switch_matrix[] PROGMEM = 
  {
  // i  t  m  is  i - id, t - type, m - middle button for types 3 and 4
    {1, 3, 33},
    {2, 5, 33},
    {3, 0, 0}, // buttons 3 and 4 are mode switch
    {4, 0, 0},
    {5, 2, 0},
    {6, 2, 0},
    {7, 1, 0},
    {8, 1, 0},
    {9, 3, 34},
    {10,5, 34},
    {11,1, 0},
    {12,1, 0},
    {13,1, 0},
    {14,0, 0},
    {15,0, 0},
    {16,0, 0},
    {17,0, 0},
    {18,0, 0},
    {19,0, 0},
    {20,0, 0},
    {21,0, 0},
    {22,0, 0},
    {23,0, 0},
    {24,0, 0},
    {25,0, 0},
    {26,0, 0},
    {27,0, 0},
    {28,0, 0},
    {29,0, 0},
    {30,0, 0},
    {31,0, 0},
    {32,0, 0},

  };

// </HUEY COLL HEAD>

//<THROTTLE QUADRANT>

//  #define RADIO_PANEL_SETUP // <--- UNCOMMENT TO SEE BUTTON NUMBERS IN JOY.CPL, COMMENT OUT FOR NORMAL OPERATION!

  #define THROTTLE_QUADRANT_PHYSICAL_AXIS_MAX 1006
  #define THROTTLE_QUADRANT_PHYSICAL_AXIS_MIN 10

  #define THROTTLE_QUADRANT_MAIN_AXIS_TRESHOLD 718 //450
  #define THROTTLE_QUADRANT_SECONDARY_AXIS_TRESHOLD 763 //600

//</THROTTLE QUADRANT>

//<RADIO PANEL>//
#define RADIO_DEVICE_SWITCH_XPDR_POSITION_JOY_BUTTON 3 // put the lower button number here
#define RADIO_MODE_SWITCH_LOWER_POSITION_JOY_BUTTON 10 // put the lower button number here
#define ALTIMETER_SELECTOR_JOY_BUTTON 22 // put the lower button number here
#define NAV_MODE_SELECTOR_JOY_BUTTON 19 // put the lower button number here
//</RADIO PANEL>


