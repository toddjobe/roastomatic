#include "Mode.h"
#include "Device.h"

void Mode::setup()
{
    // Initialize the OLED display
    device.getDisplay().setup();

    // Initialize the potentiometers
    device.getFanPot().setup();
    device.getHeatPot().setup();
};

void Mode::loop()
{
    // PWM linked to potentiometers
    device.getFanPwm().update(device.getFanPot().get());
    device.getHeatPwm().update(device.getHeatPot().get());
};