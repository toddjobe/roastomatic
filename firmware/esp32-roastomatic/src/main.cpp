// MIT License
//
// Copyright (c) Todd Jobe
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Standard libraries
#include <driver/ledc.h> // PWM library.  Works with 3.0.7
#include "esp_err.h"
#include <Wire.h>
#include <stdio.h>

// Third-party libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <max6675.h> // Thermocouple amplifier library
#include <HX711.h>   // Load Cell amplifier Library

// Local libraries
#include "button.h"

// SSR Heater Clock setup for Pulse Width Modulation
#define HEAT_MODE LEDC_LOW_SPEED_MODE
#define HEAT_FREQUENCY 1
#define HEAT_TIMER LEDC_TIMER_0
#define HEAT_CHANNEL LEDC_CHANNEL_0
#define HEAT_DUTY_RES LEDC_TIMER_12_BIT

// TIP120 Fan Clock setup for Pulse Width Modulation
#define FAN_MODE LEDC_LOW_SPEED_MODE
#define FAN_FREQUENCY 15000
#define FAN_TIMER LEDC_TIMER_1
#define FAN_CHANNEL LEDC_CHANNEL_1
#define FAN_DUTY_RES LEDC_TIMER_12_BIT

// OLED display width and height, in pixels
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1;

// I2C address for the OLED display
#define OLED_ADDRESS 0x3C

// The potentiometers will turn 270 degrees
// TODO: The resistance isn't linear.
const int ADC_BIT_DEPTH = 12;
const float MAX_DIAL = (270.0 / 360.0) * 10.0;
const int MAX_POT_VALUE = (1 << ADC_BIT_DEPTH) - 1;

// Only sample the thermocouples every 250ms
const int MIN_TEMP_SAMPLE_RATE = 250;

// Modes for HX711
struct Hx711Mode
{
  char text[22];
  void (HX711::*mode)();
};

const Hx711Mode HX711_MODES[] = {
    {"Average  ", &HX711::set_average_mode},
    {"Median   ", &HX711::set_median_mode},
    {"Med. Avg.", &HX711::set_medavg_mode},
    {"Run. Avg.", &HX711::set_runavg_mode},
    {"Raw      ", &HX711::set_raw_mode},
};

const int MIN_LOAD_CELL_SAMPLE_RATE = 100;
const float START_SCALE = 420.52;

// manual roast
const int N_WEIGHT_SAMPLES = 15;           // Number of samples to be taken for tare and calibrate scale
const float ROAST_WEIGHT_GRAMS = 90.1;     // Will be used to calibrate % drop
const float MIN_TEMP_FOR_PREHEAT = 325.0;  // Reach this temperature to trigger the TARE state.
const float MAX_BEAN_TEMP_FOR_DONE = 80.0; // dropping  below this threshold will trigger DONE state
const float MAX_HEAT_DUTY_FOR_DROP = 10;   // dropping below this threshold will trigger DROP state
const int MIN_SERIAL_PRINT_RATE = 250;     // milliseconds between serial writes
const int MIN_DISPLAY_RATE = 1000 / 60;    // 60Hz display update rate

enum MANUAL_ROAST_STATES
{
  READY,     // 0
  PREHEAT,   // 1
  TARE,      // 2
  LOAD,      // 3
  CALIBRATE, // 4
  ROAST,     // 5
  DROP,      // 6
  DONE,      // 7
  NSTATES,   // For modulo arithmetic
};

enum MANUAL_ROAST_STATES manual_roast_state;

// no more than 4 characters here
const char *state_strings[] = {
    "prep",
    "heat",
    "tare",
    "load",
    "cal.",
    "cook",
    "drop",
    "done",
    "wrap"};

// Switch between programs

typedef void (*FunctionPointer)();
struct Functions
{
  FunctionPointer setup;
  FunctionPointer loop;
};

void test_buttons_setup();
void test_display_setup();
void test_potentiometers_setup();
void test_thermocouples_setup();
void test_load_cell_setup();
void manual_roast_setup();

void test_buttons();
void test_display();
void test_potentiometers();
void test_thermocouples();
void test_load_cell();
void manual_roast();

// Selected Programs to run
const Functions FUNCTIONS[] = {
    //{test_buttons_setup, test_buttons},
    //{test_display_setup, test_display},
    //{test_potentiometers_setup, test_potentiometers},
    //{test_thermocouples_setup, test_thermocouples},
    //{test_load_cell_setup, test_load_cell},
    {manual_roast_setup, manual_roast},
};

/////////////////////////
// Pin Map
/////////////////////////

// Potentiometer pins
const int FAN_POT_PIN = 32;
const int HEAT_POT_PIN = 33;

// Thermocouple pins
// ESP32 Default SPI Pins
// MOSI 23
// MISO 19
// SCK 18
// SS 5
const int MISO_PIN = 23;
const int CS_BEAN_PIN = 5;
const int CS_INTAKE_PIN = 4;

// Screen pins
const int I2C_SDA = 21;
const int I2C_SCL = 22;

// Button pins
const int BUTTON_PINS[] = {15, 13, 12, 14, 27};
const int NUM_BUTTONS = (sizeof(BUTTON_PINS) / sizeof(*BUTTON_PINS));

// PWM pins
const int HEAT_PWM_PIN = 26;
const int FAN_PWM_PIN = 25;

// Load Cell Amplifier Pins
const int LOAD_CELL_SCK_PIN = 16;
const int LOAD_CELL_DT_PIN = 17;

/////////////
// Variables
/////////////

// Variables to hold button states and previous button states
// Button 0: Program
// Button 1: Power
// Button 2: Auto
// Button 3: Zero
// Button 4: 100g zero
Button buttons[NUM_BUTTONS] = {Button(BUTTON_PINS[0], (sizeof(FUNCTIONS) / sizeof(FUNCTIONS[0]))), Button(BUTTON_PINS[1], 3),
                               Button(BUTTON_PINS[2], 4), Button(BUTTON_PINS[3], 5),
                               Button(BUTTON_PINS[4], 6)};

// Create an instance of the SSD1306 display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// MAX6675 Thermocouple amplifiers
MAX6675 bean_thermocouple(SCK, CS_BEAN_PIN, MISO_PIN);
MAX6675 intake_thermocouple(SCK, CS_INTAKE_PIN, MISO_PIN);

// Setup Heat PWM
ledc_timer_config_t heat_timer = {
    .speed_mode = HEAT_MODE,
    .duty_resolution = HEAT_DUTY_RES,
    .timer_num = HEAT_TIMER,
    .freq_hz = HEAT_FREQUENCY,
    .clk_cfg = LEDC_AUTO_CLK};

ledc_channel_config_t heat_channel = {
    .gpio_num = HEAT_PWM_PIN,
    .speed_mode = HEAT_MODE,
    .channel = HEAT_CHANNEL,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = HEAT_TIMER,
    .duty = 0,
    .hpoint = 0};

// Setup Fan PWM
ledc_timer_config_t fan_timer = {
    .speed_mode = FAN_MODE,
    .duty_resolution = FAN_DUTY_RES,
    .timer_num = FAN_TIMER,
    .freq_hz = FAN_FREQUENCY,
    .clk_cfg = LEDC_AUTO_CLK};

ledc_channel_config_t fan_channel = {
    .gpio_num = FAN_PWM_PIN,
    .speed_mode = FAN_MODE,
    .channel = FAN_CHANNEL,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = FAN_TIMER,
    .duty = 0,
    .hpoint = 0};

// Load Cell
HX711 scale;

// Global variables
int fan_value; // ADC value read at pin
int fan_duty;  // Duty cycle in percent
int fan_dial;  // Dial position
int heat_value;
int heat_duty;
int heat_dial;
float bean_temp_f;
float intake_temp_f;

int start_temp_sample;

// HX711 globals
float raw;
float weight;

// manual roast globals
float drop_percent = 0;
int start_roast_time = 0;
int elapsed_roast_time = 0;
int start_total_time = 0;
int elapsed_total_time = 0;
int last_display_time = 0;
int last_serial_write_time = 0;

// program globals
int current_program = 0;
char displayArray1[8][22];
char displayArray2[4][10];
void set_display_row(int row, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vsnprintf(displayArray1[row], sizeof(displayArray1[row]), format, args);
  va_end(args);
}

void displayArray()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  for (int i = 0; i < sizeof(displayArray1) / sizeof(displayArray1[0]); i++)
  {
    display.println(displayArray1[i]);
  }
  display.display();
}
void setup()
{
  Serial.begin(115200);

  // Initialize the OLED display
  if (!display.begin(SSD1306_BLACK, OLED_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  // Clear the display buffer
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Initialize Buttons
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    buttons[i].begin();
  }

  // Initialize Potentiometers
  pinMode(FAN_POT_PIN, INPUT);
  pinMode(HEAT_POT_PIN, INPUT);

  // Initialize Heat PWM
  ESP_ERROR_CHECK(ledc_timer_config(&heat_timer));
  ESP_ERROR_CHECK(ledc_channel_config(&heat_channel));

  // Initialize Fan PWM
  ESP_ERROR_CHECK(ledc_timer_config(&fan_timer));
  ESP_ERROR_CHECK(ledc_channel_config(&fan_channel));

  // Initialize Load Cell
  scale.begin(LOAD_CELL_DT_PIN, LOAD_CELL_SCK_PIN, false);
  // scale.set_scale(START_SCALE);
}

void test_buttons_setup() {}
void test_buttons()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Test Buttons");

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    display.print("Button ");
    display.print(i);
    display.print(": ");
    display.println(buttons[i].count());
  }
  display.display();
}

void test_display_setup()
{
  buttons[1].setNStates(4);
}
void test_display()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Test Display");
  display.setTextSize(buttons[1].count() + 1);
  display.setCursor(0, 8);
  display.println("012345678912345678921");
  display.display();
}

void test_potentiometers_setup() {}
void test_potentiometers()
{
  char buffer[22];
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Test Potentiometers");
  display.println("");
  display.println("Pot   Res Duty Dial");
  display.println("-------------------");
  snprintf(buffer, 22, "Fan  %4d %3d%% %1d.%02d", fan_value, fan_duty, fan_dial / 100, fan_dial % 100);
  display.println(buffer);
  snprintf(buffer, 22, "Heat %4d %3d%% %1d.%02d", heat_value, heat_duty, heat_dial / 100, heat_dial % 100);
  display.println(buffer);
  snprintf(buffer, 22, "SSR LED should match duty");
  display.println(buffer);
  display.display();
}

void test_thermocouples_setup() {}
void test_thermocouples()
{
  char float_str[7];
  int i = 0;
  set_display_row(i++, "%s", "Test Thermocouples");
  set_display_row(i++, "%s", "Therm.   F. deg.");
  set_display_row(i++, "%s", "-------------------");
  set_display_row(i++, "Intake  %s", dtostrf(intake_temp_f, 6, 2, float_str));
  set_display_row(i++, "Bean    %s", dtostrf(bean_temp_f, 6, 2, float_str));
  set_display_row(i++, "%s", "");
  set_display_row(i++, "%s", "");
  set_display_row(i++, "%s", "");
  displayArray();
}

void test_load_cell_setup()
{
  // button 1 Calls the Tare
  // button 2 Calibrates 100.0g
  // button 3 Switches between mode
  // If you weigh the whole apparatus,
  // Then get the raw value right side up
  // and with 100g.
  // Then, you get the raw value upside down.
  // and with 100g.
  // you should be able to calculate the weight of just the top part, and then store an offset
  buttons[1].setNStates(2);
  buttons[2].setNStates(2);
  buttons[3].setNStates(5);
  buttons[4].setNStates(8);
}

void test_load_cell()
{
  if (buttons[1].changed())
  {
    delay(2000);
    scale.tare();
    buttons[1].reset();
  }
  if (buttons[2].changed())
  {
    delay(2000);
    scale.calibrate_scale(100);
    buttons[2].reset();
  }
  if (buttons[3].changed())
  {
    // set hx711 mode
    int index = buttons[3].count();
    (scale.*(HX711_MODES[index].mode))();
  }

  char float_str[15];
  int i = 0;
  set_display_row(i++, "%s", "Test Scale");
  set_display_row(i++, "Mode:%d  %s", scale.get_mode(), HX711_MODES[buttons[3].count()].text);
  set_display_row(i++, "Offset:%d", (int32_t)scale.get_offset());
  set_display_row(i++, "Tare:  %d", (int32_t)scale.get_tare());
  set_display_row(i++, "Scale: %s", dtostrf(scale.get_scale(), 13, 2, float_str));
  set_display_row(i++, "Value: %d", (int32_t)scale.get_value());
  set_display_row(i++, "Gain:  %d", scale.get_gain());
  displayArray();
}
void manual_roast_setup()
{
  // button 1 Calls the Tare
  // button 2 Calibrates 100.0g
  // button 3 Switches between mode
  // If you weigh the whole apparatus,
  // Then get the raw value right side up
  // and with 100g.
  // Then, you get the raw value upside down.
  // and with 100g.
  // you should be able to calculate the weight of just the top part, and then store an offset

  buttons[1].setNStates(2);
  manual_roast_state = READY;
}

void manual_roast()
{
  // manual_roast
  // Heat and Fan are controlled by the potentiometers.
  // Steps preheat-tare-load-calibrate-roast-drop-done
  // Preheat - wait until the inside temp is higher than a french roast would be 455F
  // Tare - automatically happens with maximal sample rate.  Then switches to load.
  // Load - Look for weights above 50g.  When it detects that for at least 2 seconds
  // Calibrate @ 100g - a bunch of times, start timer, percent down
  // Roast - Timer proceeds until weight is x% and then says "drop"
  // Drop - Do nothing, you should just cut the heat manually.
  // Done -
  // Serial Write - step,millis,bean_temp,intake_temp,raw_weight.

  int t = millis();

  // increment the state with button press (as an test)
  if (buttons[1].changed())
  {
    manual_roast_state = (MANUAL_ROAST_STATES)((manual_roast_state + 1) % NSTATES);
    buttons[1].reset();
  }

  switch (manual_roast_state)
  {
  case (READY): // until a reach a temperature
    start_total_time = t;
    manual_roast_state = PREHEAT;
    break;
  case (PREHEAT): // until a reach a temperature
    if (intake_temp_f >= MIN_TEMP_FOR_PREHEAT)
    {
      manual_roast_state = TARE;
    }
    break;
  case (TARE):
    scale.tare(); // This is blocking code
    manual_roast_state = LOAD;
    break;
  case (LOAD):
    /*
    if (weight > (0.5 * ROAST_WEIGHT_GRAMS))
    {
      start_roast_time = t;
      manual_roast_state = CALIBRATE;
    }
    */
    break;
  case (CALIBRATE):
    start_roast_time = t;
    scale.calibrate_scale(ROAST_WEIGHT_GRAMS); // This is blocking code
    manual_roast_state = ROAST;
    break;
  case (ROAST):
    if (heat_duty <= MAX_HEAT_DUTY_FOR_DROP) // percent
    {
      manual_roast_state = DROP;
    }
    drop_percent = 100 * (ROAST_WEIGHT_GRAMS - weight) / ROAST_WEIGHT_GRAMS;
    elapsed_roast_time = t - start_roast_time;
    break;
  case (DROP):
    if (bean_temp_f < MAX_BEAN_TEMP_FOR_DONE)
    {
      manual_roast_state = DONE;
    }
    break;
  }

  elapsed_total_time = t - start_total_time;

  if (t - last_display_time > MIN_DISPLAY_RATE)
  {
    // bigger display than normal
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);

    // line 0
    char buffer[11];
    char float_string[5];
    dtostrf((drop_percent > 0.0) ? drop_percent : 0.0, 4, 2, float_string);
    snprintf(buffer, 10, "%s %s", state_strings[manual_roast_state], float_string);
    display.println(buffer);

    // line 1
    snprintf(buffer, 11, "%01ld:%02ld %02ld:%02ld",
             elapsed_roast_time / (60 * 1000), // Minutes
             (elapsed_roast_time / 1000) % 60, // Seconds
             elapsed_total_time / (60 * 1000), // Minutes
             (elapsed_total_time / 1000) % 60  // Seconds
    );
    display.println(buffer);

    // line 2
    dtostrf(bean_temp_f, 4, 1, float_string);
    snprintf(buffer, 11, "%03d %s", fan_duty, float_string);
    display.println(buffer);

    // line 3
    dtostrf(intake_temp_f, 4, 1, float_string);
    snprintf(buffer, 11, "%03d %s", heat_duty, float_string);
    display.println(buffer);
    display.display();

    last_display_time = t;
  }
  // Write a csv file to serial.
  if ((t - last_serial_write_time) > MIN_SERIAL_PRINT_RATE)
  {
    Serial.print(elapsed_roast_time);
    Serial.print(",");
    Serial.print(elapsed_total_time);
    Serial.print(",");
    Serial.print(state_strings[manual_roast_state]);
    Serial.print(",");
    Serial.print(fan_value);
    Serial.print(",");
    Serial.print(heat_value);
    Serial.print(",");
    Serial.print(bean_temp_f);
    Serial.print(",");
    Serial.print(intake_temp_f);
    Serial.print(",");
    Serial.print(weight);
    Serial.print(",");
    Serial.print(drop_percent);
    Serial.println("");
    last_serial_write_time = t;
  }
}

void loop()
{
  // Read the raw ADC potentiometer values
  fan_value = analogRead(FAN_POT_PIN);
  heat_value = analogRead(HEAT_POT_PIN);

  fan_duty = (fan_value * 100) / MAX_POT_VALUE;
  heat_duty = (heat_value * 100) / MAX_POT_VALUE;

  fan_dial = (MAX_DIAL * fan_value * 100.0) / MAX_POT_VALUE;
  heat_dial = (MAX_DIAL * heat_value * 100.0) / MAX_POT_VALUE;

  // Read the MAX6675 amplified thermocouples
  int t = millis();
  int elapsed_temp_sample = t - start_temp_sample;
  if (elapsed_temp_sample >= MIN_TEMP_SAMPLE_RATE)
  {
    bean_temp_f = bean_thermocouple.readFarenheit();
    intake_temp_f = intake_thermocouple.readFarenheit();
    start_temp_sample = t;
  }

  // Set the duty cycle of the heat PWM based on heat potentiometer
  ledc_set_duty(HEAT_MODE, HEAT_CHANNEL, heat_value);
  ledc_update_duty(HEAT_MODE, HEAT_CHANNEL);

  // Set the duty cycle of the fan PWM based on fan potentiometer
  ledc_set_duty(FAN_MODE, FAN_CHANNEL, fan_value);
  ledc_update_duty(FAN_MODE, FAN_CHANNEL);

  // Read the raw weight
  if ((t - scale.last_time_read()) >= MIN_LOAD_CELL_SAMPLE_RATE)
  {
    raw = scale.read(); // raw has least amount of blocking
    weight = scale.get_units();
  }

  // Select program
  if (current_program != buttons[0].count())
  {
    FUNCTIONS[buttons[0].count()].setup();
  }
  // Run Program
  FUNCTIONS[buttons[0].count()].loop();
}
