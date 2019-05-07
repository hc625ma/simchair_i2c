// This file contains global variables definitions and main subroutines.

int cyclic_sens = 100;
int rudder_sens = 100;
int physical_cyclic_center_x = 0;
int physical_cyclic_center_y = 0;
int physical_pedals_center = 0;

byte coll_head_mode_sw_position = 0;
bool collective_hold_active = 0;
uint16_t collective_hold_position = 0;
bool collective_hold_position_set = 0;
bool collective_hold_button_was_pressed = 0;

uint16_t min_throttle_val[] = {0,0};
bool mode_sw_pos_0 = 0; // buffer that stores the current value of a 1st position in a mode switch
uint16_t last_throttle_setting[] = {0,0};
bool idle_stop_pressed[] = {0,0};
bool throttle_idle_cutoff[] = {1,1}; //we presume engines are off on startup


bool b8stick_lastButtonState[] = {0, 0, 0, 0, 0, 0};
bool dev_b8stick = 0;
bool dev_cyclic = 0;
bool dev_simple_collective = 0;
bool dev_single_engine_collective = 0;
bool dev_twin_engine_collective = 0;
bool dev_pedals = 0;
bool dev_cessna_engine_and_prop_controls = 0;
bool dev_ab412_coll_head = 0;
bool dev_huey_coll_head = 0;
bool dev_throttle_quadrant = 0;
bool dev_vrmax_panel = 0;
bool SW_MODE_BUTTON = 0;
bool SW_MODE_TOGGLE = 1;
bool dev_hh60g_cyclic_grip = 0;
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

int ADC_RANGE = 0.5 + pow(2, ADS1115_RESOLUTION);
int COLLECTIVE_ADC_RANGE = 0.5 + pow(2, 10);

bool force_trim_on = 0;
bool force_trim_position_set = 0;
byte cyclic_force_trim_state = 0;
bool force_trim_rudder_on = 0;
bool force_trim_rudder_position_set = 0;
byte pedals_force_trim_state = 0;
bool force_trim_button_pressed = 0;
bool controls_freezed = 0;

int cyclic_x_adj = 0;
int cyclic_y_adj = 0;

uint16_t force_trim_x = ADC_RANGE / 2;
uint16_t force_trim_y = ADC_RANGE / 2 ;
uint16_t force_trim_rudder = ADC_RANGE / 2;


int one_percent_range = ADC_RANGE / 100;
int one_percent_coll_range = COLLECTIVE_ADC_RANGE / 100;

uint8_t ftcr;

bool physical_latch_button_state = 0;
bool throttle_latch_pressed = 0;

int x_diff;
int y_diff;
int rudder_diff;
int diffy;
int diffx;
int32_t xval;
int32_t yval;
int32_t rudder_val;
int xval_prev;
sw_matrix switch_matrix[32];

byte MODE_SWITCH_BUTTON = 0;
byte IDLE_STOP_BUTTON = 0;

byte COLLECTIVE_HOLD_BUTTON = 255;

void setup()
{
  delay (2000);
  Serial.begin(115200);
  simchair.begin();
  simchair_aux1.begin();
  simchair_aux2.begin();
  //detect and configure connected peripherals
  Wire.begin();
  setup_b8stick();
  setup_cyclic();
  setup_single_engine_collective();
  setup_pedals();
  setup_hh60g_cyclic_grip();
  setup_ab412_coll_head();

  if ((PTT_KEYBOARD_PRESS == 1) || (DCS_HUEY_IDLE_STOP_COMPAT_MODE_ENABLED == 1))
  {
    Keyboard.begin();
  }
}



void loop()
{
  if (dev_b8stick == 1)
  {
    //poll_b8stick();
  }
  if (dev_cyclic == 1)
  {
    poll_cyclic();
  }
  if (dev_single_engine_collective == 1)
  {
    poll_single_engine_collective();
  }
  if (dev_pedals == 1)
  {
    poll_pedals();
  }
  if (dev_hh60g_cyclic_grip == 1)
  {
    poll_hh60g_cyclic_grip();
  }
  if (dev_ab412_coll_head == 1)
  {
    poll_ab412_coll_head();
  }
  
}

