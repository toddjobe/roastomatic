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

#ifndef LOADCELLFUNCTION_H
#define LOADCELLFUNCTION_H

#include <Mode.h>
#include <Device.h>

struct Hx711Mode
{
    char text[22];
    void (HX711::*mode)();
};

class LoadcellFunction : public Mode
{
public:
    explicit LoadcellFunction(Device &deviceRef) : Mode(deviceRef) {}
    void setup() override
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

        Mode::setup();
        device.getButtons()[1].setNStates(2);
        device.getButtons()[2].setNStates(2);
        device.getButtons()[3].setNStates(5);
        device.getButtons()[4].setNStates(8);
    }

    void loop()
    {
        if (device.getButtons()[1].changed())
        {
            delay(2000);
            device.getScale().tare();
            device.getButtons()[1].reset();
        }
        if (device.getButtons()[2].changed())
        {
            delay(2000);
            device.getScale().calibrate_scale(100);
            device.getButtons()[2].reset();
        }
        if (device.getButtons()[3].changed())
        {
            // set hx711 mode
            int index = device.getButtons()[3].count();
            (device.getScale().*(HX711_MODES[index].mode))();
        }

        char float_str[15];
        int i = 0;
        device.getDisplay().set_display_row(i++, "%s", "Test Scale");
        device.getDisplay().set_display_row(i++, "Mode:%d  %s", device.getScale().get_mode(), HX711_MODES[device.getButtons()[3].count()].text);
        device.getDisplay().set_display_row(i++, "Offset:%d", (int32_t)device.getScale().get_offset());
        device.getDisplay().set_display_row(i++, "Tare:  %d", (int32_t)device.getScale().get_tare());
        device.getDisplay().set_display_row(i++, "Scale: %s", dtostrf(device.getScale().get_scale(), 13, 2, float_str));
        device.getDisplay().set_display_row(i++, "Value: %d", (int32_t)device.getScale().get_value());
        device.getDisplay().set_display_row(i++, "Gain:  %d", device.getScale().get_gain());
        device.getDisplay().displayArray();
    }

private:
    const Hx711Mode HX711_MODES[5] = {
        {"Average  ", &HX711::set_average_mode},
        {"Median   ", &HX711::set_median_mode},
        {"Med. Avg.", &HX711::set_medavg_mode},
        {"Run. Avg.", &HX711::set_runavg_mode},
        {"Raw      ", &HX711::set_raw_mode},
    };
};

#endif // LOADCELLFUNCTION_H