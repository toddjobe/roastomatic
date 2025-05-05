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

#ifndef BUTTONFUNCTION_H
#define BUTTONFUNCTION_H

#include <Mode.h>
#include <Device.h>

class ButtonFunction : public Mode
{
public:
    explicit ButtonFunction(Device &deviceRef) : Mode(deviceRef) {}
    void setup() override
    {
        Mode::setup();
        Device &device = Device::getInstance();
        device.getButtons()[1].setNStates(4);
        device.getButtons()[2].setNStates(2);
        device.getButtons()[3].setNStates(5);
        device.getButtons()[4].setNStates(8);
    }

    void loop() override
    {
        Mode::loop();
        device.getDisplay().clearDisplay();
        device.getDisplay().setTextSize(1);
        device.getDisplay().setCursor(0, 0);
        device.getDisplay().println("Test Buttons");

        for (int i = 0; i < NUM_BUTTONS; i++)
        {
            device.getDisplay().print("Button ");
            device.getDisplay().print(i);
            device.getDisplay().print(": ");
            device.getDisplay().println(device.getButtons()[i].count());
        }
        device.getDisplay().display();
    }
};

#endif // BUTTONFUNCTION_H
