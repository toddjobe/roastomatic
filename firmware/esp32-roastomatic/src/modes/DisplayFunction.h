/*
MIT License 2025 Todd Jobe

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

#ifndef DISPLAYFUNCTION_H
#define DISPLAYFUNCTION_H

#include <Mode.h>
#include <Device.h>

class DisplayFunction : public Mode
{
public:
    explicit DisplayFunction(Device &deviceRef) : Mode(deviceRef) {}
    void setup()
    {
        Mode::setup();
        device.getButtons()[1].setNStates(4);
    }

    void loop()
    {
        Mode::loop();
        device.getDisplay().clearDisplay();
        device.getDisplay().setTextSize(1);
        device.getDisplay().setCursor(0, 0);
        device.getDisplay().println("Test Display");
        device.getDisplay().setTextSize(device.getButtons()[1].count() + 1);
        device.getDisplay().setCursor(0, 8);
        device.getDisplay().println("012345678912345678921");
        device.getDisplay().display();
    }
};

#endif // DISPLAYFUNCTION_H