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

#ifndef SCALE_H
#define SCALE_H

#include "HX711.h" // Load Cell amplifier Library
#include "Filter.h"
#include "HardwareTimer.h"
#include <pins_arduino.h> // ESP32 pin definitions

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

class Scale : public HX711
{
public:
    Scale() : HX711(), _timer(MIN_LOAD_CELL_SAMPLE_RATE, Scale::add, std::ref(*this)), _filter()
    {
        DEBUG_PRINT("Scale constructor called.");
    }

    void setup()
    {
        DEBUG_PRINT("Scale setup started.");
        HX711::begin(LOAD_CELL_DT_PIN, LOAD_CELL_SCK_PIN, false);
        DEBUG_PRINT_VAL("LOAD_CELL_DT_PIN: ", LOAD_CELL_DT_PIN);
        DEBUG_PRINT_VAL("LOAD_CELL_SCK_PIN: ", LOAD_CELL_SCK_PIN);

        HX711::set_scale(START_SCALE);
        DEBUG_PRINT_VAL("Initial scale set to: ", START_SCALE);

        _timer.start(); // Start the timer. Add will be called by interrupt.
        DEBUG_PRINT("Timer started for Scale.");
        DEBUG_PRINT("Scale setup completed.");
    }

    static void add(Scale &scale)
    {
        DEBUG_PRINT("Adding new value to filter...");
        float units = scale.HX711::get_units();
        DEBUG_PRINT_VAL("Raw units from HX711: ", units);
        scale._filter.add(units);
        DEBUG_PRINT("Value added to filter.");
    }

    float get_units() const
    {
        float filtered_units = _filter.get();
        DEBUG_PRINT_VAL("Filtered units: ", filtered_units);
        return filtered_units;
    }

    float getWeight()
    {
        float weight = get_scale() * get_units() + get_tare();
        DEBUG_PRINT_VAL("Calculated weight: ", weight);
        return weight;
    }

    // Load Cell Constants
    static const int SCALE_WINDOW_SIZE = 10;
    static const int MIN_LOAD_CELL_SAMPLE_RATE = 100;
    static constexpr float START_SCALE = 420.52;
    static const int LOAD_CELL_DT_PIN = 17;
    static const int LOAD_CELL_SCK_PIN = 16;

private:
    Filter<float, SCALE_WINDOW_SIZE> _filter;
    HardwareTimer<Scale> _timer;
};

#endif