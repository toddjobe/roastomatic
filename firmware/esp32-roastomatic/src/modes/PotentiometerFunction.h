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

#ifndef POTENTIOMETERFUNCTION_H
#define POTENTIOMETERFUNCTION_H

#include "Mode.h"

class PotentiometerFunction : public Mode
{
public:
    explicit PotentiometerFunction(Device &deviceRef) : Mode(deviceRef) {}

    void setup() override
    {
        Mode::setup();
    }

    void loop()
    {
        Mode::loop();
        char buffer[22];
        device.getDisplay().clearDisplay();
        device.getDisplay().setTextSize(1);
        device.getDisplay().setCursor(0, 0);
        device.getDisplay().println("Test Potentiometers");
        device.getDisplay().println("");
        device.getDisplay().println("Pot   Res Duty Dial");
        device.getDisplay().println("-------------------");
        snprintf(buffer, 22, "Fan  %4d %3d%% %1d.%02d", device.getFanPot().get(), device.getFanPot().getDuty(), device.getFanPot().getDial() / 100, device.getFanPot().getDial() % 100);
        device.getDisplay().println(buffer);
        snprintf(buffer, 22, "Heat %4d %3d%% %1d.%02d", device.getHeatPot().get(), device.getHeatPot().getDuty(), device.getHeatPot().getDuty() / 100, device.getHeatPot().getDuty() % 100);
        device.getDisplay().println(buffer);
        snprintf(buffer, 22, "SSR LED should match duty");
        device.getDisplay().println(buffer);
        device.getDisplay().display();
    }
};

#endif // POTENTIOMETERFUNCTION_H