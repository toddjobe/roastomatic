/*
MIT License

Copyright (c) 2025 Todd Jobe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef THERMOCOUPLEFUNCTION_H
#define THERMOCOUPLEFUNCTION_H

#include "Mode.h"
#include "Device.h"

class ThermocoupleFunction : public Mode
{
public:
    explicit ThermocoupleFunction(Device &deviceRef) : Mode(deviceRef) {}
    void setup() override
    {
        Mode::setup();
    }

    void loop() override
    {
        Mode::loop();
        Device &device = Device::getInstance();
        char buffer[22];
        device.getDisplay().clearDisplay();
        device.getDisplay().setTextSize(1);
        device.getDisplay().setCursor(0, 0);
        device.getDisplay().println("Test Thermocouples");
        device.getDisplay().println("");
        device.getDisplay().println("Thermocouple Temp");
        device.getDisplay().println("-------------------");
        snprintf(buffer, 22, "Intake %4.1f", device.getIntakeThermocouple().readFahrenheit());
        device.getDisplay().println(buffer);
        snprintf(buffer, 22, "Bean   %4.1f", device.getBeanThermocouple().readFahrenheit());
        device.getDisplay().println(buffer);
        device.getDisplay().display();
    }
};

#endif // THERMOCOUPLEFUNCTION_H