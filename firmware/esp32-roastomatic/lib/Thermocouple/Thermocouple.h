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

#ifndef THERMOCOUPLE_H
#define THERMOCOUPLE_H

#include "max6675.h" // Thermocouple amplifier library
#include "Filter.h"
#include "HardwareTimer.h"
#include <pins_arduino.h> // ESP32 pin definitions

#define THERMOCOUPLE_WINDOW_SIZE 5
#define THERMOCOUPLE_SAMPLE_RATE_MICROSECONDS 250000

// Thermocouple pins
// ESP32 Default SPI Pins
// MOSI 23
// MISO 19
// SCK 18
// SS 5
#define MISO_PIN 23

class Thermocouple : public MAX6675
{
public:
    Thermocouple(uint8_t CS) : MAX6675(SCK, CS, MISO_PIN), _timer(THERMOCOUPLE_SAMPLE_RATE_MICROSECONDS, Thermocouple::add, std::ref(*this)), _filter() // Alternative to setup, providing a more descriptive name
    {
        _timer.start();
    }

    static void add(Thermocouple &thermocouple) // You should not call this directly but rather let the timer call it.
    {
        thermocouple._filter.add(thermocouple.MAX6675::readCelsius());
    }

    float readCelsius() const
    {
        return _filter.get();
    }

private:
    Filter<float, THERMOCOUPLE_WINDOW_SIZE> _filter;
    HardwareTimer<Thermocouple> _timer;
};

#endif