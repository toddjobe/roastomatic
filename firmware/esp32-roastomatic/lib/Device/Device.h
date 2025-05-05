#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include "DeviceConfig.h"
#include "Button.h"
#include "Thermocouple.h"
#include "Potentiometer.h"
#include "Filter.h"
#include "HardwareTimer.h"
#include "SoftwareTimer.h"
#include "PWM.h"
#include "ModeRegistry.h"
#include "Scale.h"
#include "Display.h"

// Debugging macro
#define DEBUG 1 // Set to 1 to enable debugging, 0 to disable

#if DEBUG
#define DEBUG_PRINT(x) Serial.println(x)
#define DEBUG_PRINT_VAL(label, value) \
    Serial.print(label);              \
    Serial.println(value)
#else
#define DEBUG_PRINT(x)                // No-op
#define DEBUG_PRINT_VAL(label, value) // No-op
#endif

class Mode; // Forward declaration of Mode

class Device
{
public:
    static Device &getInstance() // Singleton pattern
    {
        static Device instance; // Guaranteed to be destroyed.
        DEBUG_PRINT("Device instance accessed.");
        return instance; // Instantiated on first use.
    }

    void setup();
    void loop();
    void setMode(Mode *mode);
    Potentiometer &getFanPot() { return _fan_pot; }
    Potentiometer &getHeatPot() { return _heat_pot; }
    Button *getButtons() { return _buttons; }
    Thermocouple &getIntakeThermocouple() { return _intake_thermocouple; }
    Thermocouple &getBeanThermocouple() { return _bean_thermocouple; }
    Scale &getScale() { return _scale; }
    PWM &getHeatPwm() { return _heat_pwm; }
    PWM &getFanPwm() { return _fan_pwm; }
    Display &getDisplay() { return _display; }

    Device(const Device &) = delete;            // Can not be cloned
    Device &operator=(const Device &) = delete; // Not assignable

protected:
    Device()
        : _display(),
          _fan_pot(FAN_POT_PIN),
          _heat_pot(HEAT_POT_PIN),
          _intake_thermocouple(CS_INTAKE_PIN),
          _bean_thermocouple(CS_BEAN_PIN),
          _scale(),
          _heat_pwm(ledc_timer_config_t{.speed_mode = HEAT_MODE,
                                        .duty_resolution = HEAT_DUTY_RES,
                                        .timer_num = HEAT_TIMER,
                                        .freq_hz = HEAT_FREQUENCY,
                                        .clk_cfg = LEDC_AUTO_CLK},
                    ledc_channel_config_t{.gpio_num = HEAT_PWM_PIN,
                                          .speed_mode = HEAT_MODE,
                                          .channel = HEAT_CHANNEL,
                                          .intr_type = LEDC_INTR_DISABLE}),
          _fan_pwm(ledc_timer_config_t{.speed_mode = FAN_MODE,
                                       .duty_resolution = FAN_DUTY_RES,
                                       .timer_num = FAN_TIMER,
                                       .freq_hz = FAN_FREQUENCY,
                                       .clk_cfg = LEDC_AUTO_CLK},
                   ledc_channel_config_t{.gpio_num = FAN_PWM_PIN,
                                         .speed_mode = FAN_MODE,
                                         .channel = FAN_CHANNEL,
                                         .intr_type = LEDC_INTR_DISABLE}),
          _buttons({Button(BUTTON_PINS[0]), Button(BUTTON_PINS[1]), Button(BUTTON_PINS[2]),
                    Button(BUTTON_PINS[3]), Button(BUTTON_PINS[4])}),
          _mode(nullptr)
    {
    }

private:
    Mode *_mode;

    // Outputs
    Display _display;

    // Controllers
    Potentiometer _fan_pot;
    Potentiometer _heat_pot;
    Button _buttons[NUM_BUTTONS];

    // Sensors
    Thermocouple _intake_thermocouple;
    Thermocouple _bean_thermocouple;
    Scale _scale;

    // Inputs
    PWM _heat_pwm;
    PWM _fan_pwm;
};

#endif // DEVICE_H