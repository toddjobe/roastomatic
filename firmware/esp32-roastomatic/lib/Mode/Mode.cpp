#include "Mode.h"
#include "Device.h"

void Mode::setup()
{
    // Initialize the OLED display
    device.getDisplay().setup();

    // Initialize the potentiometers
    device.getFanPot().setup();
    device.getHeatPot().setup();

    // Initialize the PWM for the fan and heat
    device.getFanPwm().setup();
    device.getHeatPwm().setup();
};

void Mode::loop()
{
    // PWM linked to potentiometers
    device.getFanPwm().update(device.getFanPot().get());
    device.getHeatPwm().update(device.getHeatPot().get());
};