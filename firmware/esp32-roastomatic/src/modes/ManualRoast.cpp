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

// 3rd party includes
#include <Arduino.h>

// local includes
#include "Mode.h"
#include "Device.h"
#include "ManualRoast.h"

void ManualRoast::setup()
{
    Mode::setup();
    // button 1 Calls the Tare
    // button 2 Calibrates 100.0g
    // button 3 Switches between mode
    // If you weigh the whole apparatus,
    // Then get the raw value right side up
    // and with 100g.
    // Then, you get the raw value upside down.
    // and with 100g.
    // you should be able to calculate the weight of just the top part, and then store an offset

    Device::getInstance().getButtons()[1].setNStates(2);
    manual_roast_state = READY;
}

void ManualRoast::loop()
{
    Mode::loop();
    // manual_roast
    // Heat and Fan are controlled by the potentiometers.
    // Steps preheat-tare-load-calibrate-roast-drop-done
    // Preheat - wait until the inside temp is higher than a french roast would be 455F
    // Tare - automatically happens with maximal sample rate.  Then switches to load.
    // Load - Look for weights above 50g.  When it detects that for at least 2 seconds
    // Calibrate @ 100g - a bunch of times, start timer, percent down
    // Roast - Timer proceeds until weight is x% and then says "drop"
    // Drop - Do nothing, you should just cut the heat manually.
    // Done -
    // Serial Write - step,millis,bean_temp,intake_temp,raw_weight.

    int t = millis();

    // increment the state with button press (as an test)
    if (device.getButtons()[1].changed())
    {
        manual_roast_state = (MANUAL_ROAST_STATES)((manual_roast_state + 1) % NSTATES);
        device.getButtons()[1].reset();
    }

    switch (manual_roast_state)
    {
    case (READY): // until a reach a temperature
        start_total_time = t;
        manual_roast_state = PREHEAT;
        break;
    case (PREHEAT): // until a reach a temperature
        if (device.getIntakeThermocouple().readFahrenheit() >= MIN_TEMP_FOR_PREHEAT)
        {
            manual_roast_state = TARE;
        }
        break;
    case (TARE):
        device.getScale().tare(); // This is blocking code
        manual_roast_state = LOAD;
        break;
    case (LOAD):
        /*
        if (weight > (0.5 * ROAST_WEIGHT_GRAMS))
        {
          start_roast_time = t;
          manual_roast_state = CALIBRATE;
        }
        */
        break;
    case (CALIBRATE):
        start_roast_time = t;
        device.getScale().calibrate_scale(ROAST_WEIGHT_GRAMS); // This is blocking code
        manual_roast_state = ROAST;
        break;
    case (ROAST):
        if (device.getHeatPot().getDuty() <= MAX_HEAT_DUTY_FOR_DROP) // percent
        {
            manual_roast_state = DROP;
        }
        drop_percent = 100 * (ROAST_WEIGHT_GRAMS - device.getScale().getWeight()) / ROAST_WEIGHT_GRAMS;
        elapsed_roast_time = t - start_roast_time;
        break;
    case (DROP):
        if (device.getBeanThermocouple().readFahrenheit() < MAX_BEAN_TEMP_FOR_DONE)
        {
            manual_roast_state = DONE;
        }
        break;
    }

    elapsed_total_time = t - start_total_time;

    if (t - last_display_time > MIN_DISPLAY_RATE)
    {
        // bigger display than normal
        device.getDisplay().clearDisplay();
        device.getDisplay().setTextSize(2);
        device.getDisplay().setCursor(0, 0);

        // line 0
        char buffer[11];
        char float_string[5];
        dtostrf((drop_percent > 0.0) ? drop_percent : 0.0, 4, 2, float_string);
        snprintf(buffer, 10, "%s %s", state_strings[manual_roast_state], float_string);
        device.getDisplay().println(buffer);

        // line 1
        snprintf(buffer, 11, "%01ld:%02ld %02ld:%02ld",
                 elapsed_roast_time / (60 * 1000), // Minutes
                 (elapsed_roast_time / 1000) % 60, // Seconds
                 elapsed_total_time / (60 * 1000), // Minutes
                 (elapsed_total_time / 1000) % 60  // Seconds
        );
        device.getDisplay().println(buffer);

        // line 2
        dtostrf(device.getBeanThermocouple().readFahrenheit(), 4, 1, float_string);
        snprintf(buffer, 11, "%03d %s", device.getFanPwm().getDuty(), float_string);
        device.getDisplay().println(buffer);

        // line 3
        dtostrf(device.getIntakeThermocouple().readFahrenheit(), 4, 1, float_string);
        snprintf(buffer, 11, "%03d %s", device.getHeatPwm().getDuty(), float_string);
        device.getDisplay().println(buffer);
        device.getDisplay().display();

        last_display_time = t;
    }
    // Write a csv file to serial.
    if ((t - last_serial_write_time) > MIN_SERIAL_PRINT_RATE)
    {
        Serial.print(elapsed_roast_time);
        Serial.print(",");
        Serial.print(elapsed_total_time);
        Serial.print(",");
        Serial.print(state_strings[manual_roast_state]);
        Serial.print(",");
        Serial.print(device.getFanPwm().getDuty());
        Serial.print(",");
        Serial.print(device.getHeatPwm().getDuty());
        Serial.print(",");
        Serial.print(device.getBeanThermocouple().readFahrenheit());
        Serial.print(",");
        Serial.print(device.getIntakeThermocouple().readFahrenheit());
        Serial.print(",");
        Serial.print(device.getScale().getWeight());
        Serial.print(",");
        Serial.print(drop_percent);
        Serial.println("");
        last_serial_write_time = t;
    }
}
