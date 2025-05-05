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

#ifndef MANUAL_ROAST_H
#define MANUAL_ROAST_H

#include "Mode.h"

enum MANUAL_ROAST_STATES
{
    READY,     // 0
    PREHEAT,   // 1
    TARE,      // 2
    LOAD,      // 3
    CALIBRATE, // 4
    ROAST,     // 5
    DROP,      // 6
    DONE,      // 7
    NSTATES,   // For modulo arithmetic
};

class ManualRoast : public Mode
{
public:
    explicit ManualRoast(Device &deviceRef) : Mode(deviceRef) {};
    // manual roast
    const int N_WEIGHT_SAMPLES = 15;           // Number of samples to be taken for tare and calibrate scale
    const float ROAST_WEIGHT_GRAMS = 90.1;     // Will be used to calibrate % drop
    const float MIN_TEMP_FOR_PREHEAT = 325.0;  // Reach this temperature to trigger the TARE state.
    const float MAX_BEAN_TEMP_FOR_DONE = 80.0; // dropping  below this threshold will trigger DONE state
    const float MAX_HEAT_DUTY_FOR_DROP = 10;   // dropping below this threshold will trigger DROP state
    const int MIN_SERIAL_PRINT_RATE = 250;     // milliseconds between serial writes
    const int MIN_DISPLAY_RATE = 1000 / 60;    // 60Hz display update rate

    // no more than 4 characters here
    const char *state_strings[9] = {
        "prep",
        "heat",
        "tare",
        "load",
        "cal.",
        "cook",
        "drop",
        "done",
        "wrap"};

    void setup() override;
    void loop() override;

private:
    enum MANUAL_ROAST_STATES manual_roast_state = READY;
    int start_total_time = 0;
    int elapsed_total_time = 0;
    int start_roast_time = 0;
    int elapsed_roast_time = 0;
    int last_display_time = 0;
    int last_serial_write_time = 0;
    float drop_percent = 0;
};

#endif