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

enum Device: uint8_t {
  DEVICE_B8_STICK,
  DEVICE_CYCLIC,
  DEVICE_SIMPLE_COLLECTIVE,
  DEVICE_SINGLE_COLLECTIVE,
  DEVICE_TWIN_COLLECTIVE,
  DEVICE_PEDALS,
  DEVICE_GA_CONTROLS,
  DEVICE_THROTTLE_QUADRANT,
  DEVICE_AB412_HEAD,
  DEVICE_HUEY_HEAD,
  DEVICE_VRMAX
};

uint16_t connected_devices = 0;

bool SW_MODE_BUTTON = 0;
bool SW_MODE_TOGGLE = 1;
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
r_matrix radio_matrix[24];

byte radio_device;
bool radio0_unchecked = 0;
bool radio2_unchecked = 0;
byte radio_mode = 1;
bool radio_mode0_unchecked = 0;
bool radio_mode2_unchecked = 0;
byte alt_mode = 0;
bool alt_mode0_unchecked = 0;
bool alt_mode2_unchecked = 0;
byte nav_mode = 0;
bool nav_mode0_unchecked = 0;
bool nav_mode2_unchecked = 0;
bool xpdr_mode0_unchecked = 0;
bool xpdr_mode2_unchecked = 0;
bool xpdr_val = 0;

bool obs_rate = 0;
bool dg_rate = 0;


byte knob_button_hold_timeout = 100;

enc_state e_state[7];
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
  setup_simple_collective();
  setup_single_engine_collective();
  setup_twin_engine_collective();
  setup_pedals();
  setup_cessna_engine_and_prop_controls();
  setup_ab412_coll_head();
  setup_vrmax_panel();
  setup_huey_coll_head();
  setup_throttle_quadrant();
  if ((PTT_KEYBOARD_PRESS == 1) || (DCS_HUEY_IDLE_STOP_COMPAT_MODE_ENABLED == 1))
  {
    Keyboard.begin();
  }
}



void loop()
{
  //Serial.println("WHEEE");
  if (connected_devices | DEVICE_B8_STICK)
  {
    poll_b8stick();
  }
  if (connected_devices | DEVICE_CYCLIC)
  {
    poll_cyclic();
  }
  if (connected_devices | DEVICE_SIMPLE_COLLECTIVE)
  {
    poll_simple_collective();
  }
  if (connected_devices | DEVICE_SINGLE_COLLECTIVE)
  {
    poll_single_engine_collective();
  }
  if (connected_devices | DEVICE_TWIN_COLLECTIVE)
  {
    poll_twin_engine_collective();
  }
  if (connected_devices | DEVICE_PEDALS)
  {
    poll_pedals();
  }
  if (connected_devices | DEVICE_GA_CONTROLS)
  {
    poll_cessna_engine_and_prop_controls();
  }
  if (connected_devices | DEVICE_AB412_HEAD)
  {
    poll_ab412_coll_head();
  }
  if (connected_devices | DEVICE_VRMAX)
  {
    poll_vrmax_panel();
  }
  if (connected_devices | DEVICE_HUEY_HEAD)
  {
    poll_huey_coll_head();
  }
  if (connected_devices | DEVICE_THROTTLE_QUADRANT)
  {
    poll_throttle_quadrant();
  }
}
