#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include <Arduino.h>
#include <driver/ledc.h>

// Potentiometer pins
static const int FAN_POT_PIN = 32;
static const int HEAT_POT_PIN = 33;

// Thermocouple pins
static const int CS_BEAN_PIN = 5;
static const int CS_INTAKE_PIN = 4;

// Button pins
static const int BUTTON_PINS[5] = {15, 13, 12, 14, 27};
static const int NUM_BUTTONS = sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);

// PWM pins
static const int HEAT_PWM_PIN = 26;
static const int FAN_PWM_PIN = 25;

// PWM Heater Clock setup
static const ledc_mode_t HEAT_MODE = LEDC_LOW_SPEED_MODE;
static const ledc_timer_bit_t HEAT_DUTY_RES = LEDC_TIMER_12_BIT;
static const ledc_timer_t HEAT_TIMER = LEDC_TIMER_0;
static const int HEAT_FREQUENCY = 1000;
static const ledc_channel_t HEAT_CHANNEL = LEDC_CHANNEL_0;

// PWM Fan Clock setup
static const ledc_mode_t FAN_MODE = LEDC_LOW_SPEED_MODE;
static const ledc_timer_bit_t FAN_DUTY_RES = LEDC_TIMER_12_BIT;
static const ledc_timer_t FAN_TIMER = LEDC_TIMER_1;
static const int FAN_FREQUENCY = 15000;
static const ledc_channel_t FAN_CHANNEL = LEDC_CHANNEL_1;

#endif // DEVICE_CONFIG_H