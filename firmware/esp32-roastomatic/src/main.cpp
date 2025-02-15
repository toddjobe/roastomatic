#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "button.h"

// OLED display width and height, in pixels
const int screen_width = 128;
const int screen_height = 64;
const int oled_reset = -1;

// I2C address for the OLED display
#define OLED_ADDRESS 0x3C

// The potentiometers will turn 300 degrees
const int adc_bit_depth = 12;
const float max_dial = (300.0 / 360.0) * 10.0;
const int max_pot_value = (1 << adc_bit_depth) -1;

// pin map
// potentiometer pings
const int fanPotPin = 32;
const int heatPotPin = 33;

// screen pins
const int i2c_sda = 21;
const int i2c_scl = 22;

// button pins
const int buttonPins[] = { 15, 13, 12, 14, 27 };
const int numButtons = (sizeof(buttonPins) / sizeof(*buttonPins));

// Variables to hold button states and previous button states
// Button 0: Program
// Button 1: Power
// Button 2: Auto
// Button 3: Zero
// Button 4: 100g zero
Button buttons[numButtons] = { Button(buttonPins[0], 3), Button(buttonPins[1], 3),
                               Button(buttonPins[2], 4), Button(buttonPins[3], 5),
                               Button(buttonPins[4], 6) };


// Create an instance of the SSD1306 display
Adafruit_SSD1306 display(screen_width, screen_height, &Wire, oled_reset);

int fanValue; // ADC value read at pin
int fanDuty;  // Duty cycle in percent
int fanDial;  // Dial position 
int heatValue;
int heatDuty;
int heatDial;

int current_program = 0;

// in the case of a false hit, when it's triggered I started the clock running.  outDelay
void setup() {
  Serial.begin(115200);

  // Initialize the OLED display
  if (!display.begin(SSD1306_BLACK, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  // Clear the display buffer
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Begin Buttons
  for (int i = 0; i < numButtons; i++) {
    buttons[i].begin();
  }

  // Begin Potentiometers
  pinMode(fanPotPin, INPUT);
  pinMode(heatPotPin, INPUT);
}

void testButtonsSetup(){}
void testButtons() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Test Buttons");

  for (int i = 0; i < numButtons; i++) {
    // Display the weight on the OLED
    display.print("Button ");
    display.print(i);
    display.print(": ");
    display.println(buttons[i].count());
  }
  display.display();
  delay(100);
}

void testDisplaySetup() {
  buttons[1].setNStates(4);
}
void testDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Test Display");
  display.setTextSize(buttons[1].count() + 1);
  display.setCursor(0, 8);
  display.println("012345678912345678921");
  display.display();

  delay(100);
}
void testPotentiometersSetup() {
}
void testPotentiometers() {

  // Read the raw ADC values
  fanValue = analogRead(fanPotPin);
  heatValue = analogRead(heatPotPin);

  fanDuty = (fanValue * 100) / 4095;
  heatDuty = (heatValue * 100) / 4095;

  fanDial = (max_dial * fanValue * 100.0) / max_pot_value;
  heatDial = (max_dial * heatValue * 100.0) / max_pot_value;

  char buffer[22];
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Test Potentiometers");
  display.println("");
  display.println("Pot   Res Duty Dial");
  display.println("-------------------");
  snprintf(buffer, 22, "Fan  %4d %3d%% %1d.%02d", fanValue, fanDuty, fanDial / 100, fanDial % 100);
  display.println(buffer);
  snprintf(buffer, 22, "Heat %4d %3d%% %1d.%02d", heatValue, heatDuty, heatDial / 100, heatDial % 100);
  display.println(buffer);

  display.display();
  delay(100);
}

void testThermocouplesSetup() {
}
void testThermocouples() {
  char buffer[22];
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Test Thermocouples");
  display.println("Therm   °C  °F");
  display.println("---------------------");
  snprintf(buffer, 22, "Intake  %4d %3d.%01d%% %1d.%02d", fanValue, fanDuty / 10, fanDuty % 10, fanDial / 100, fanDial % 100);
  display.println(buffer);
  snprintf(buffer, 22, "Bean  %4d %3d.%01d%% %1d.%02d", fanValue, fanDuty / 10, fanDuty % 10, fanDial / 100, fanDial % 100);
  display.println(buffer);

  display.display();
  delay(100);
}

void loop() {

  // setup programs when program switches
  if (current_program != buttons[0].count()) {
    switch (buttons[0].count()) {
      case 0:  // Button Tester
        testButtonsSetup();
        break;
      case 1:  // Screen Tester
        testDisplaySetup();
        break;
      case 2:  // Potentiometer Tester
        testPotentiometersSetup();
        break;
    }
    current_program = buttons[0].count();
  }

  switch (buttons[0].count()) {
    case 0:  // Button Tester
      testButtons();
      break;
    case 1:  // Screen Tester
      testDisplay();
      break;
    case 2:  // Potentiometer Tester
      testPotentiometers();
      break;
  }

}