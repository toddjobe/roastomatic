#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

// standard includes
#include <Arduino.h>

// local includes
#include "../Filter/Filter.h"
#include "../SoftwareTimer/SoftwareTimer.h"

#define FILTER_WINDOW 100 // Size of the filter window
class Potentiometer
{
public:
    Potentiometer(uint8_t pin)
        : _pin(pin), _filter(), _timer(SAMPLE_RATE_MS, &Potentiometer::add, std::ref(*this))
    {
    }

    // TODO: The resistance isn't linear
    const uint8_t ADC_BIT_DEPTH = 12;                        // 12 bit ADC
    const float_t MAX_DIAL = (270.0 / 360.0) * 10.0;         // 270 degrees of rotation
    const uint16_t MAX_POT_VALUE = (1 << ADC_BIT_DEPTH) - 1; // 2^12 - 1 = 4095
    const uint32_t SAMPLE_RATE_MS = 1;

    static void add(Potentiometer &potentiometer)
    {
        potentiometer._filter.add(analogRead(potentiometer._pin));
    }
    uint16_t get() // retuns the filtered value in the range of the analog read 12 bit value
    {
        return _filter.get();
    }
    uint8_t getDuty() // returns a value between 0 and 100
    {
        return (get() * 100) / MAX_POT_VALUE;
    }
    uint16_t getDial() // returns a value between 0 and the maximum dial angle
    {
        return (MAX_DIAL * get() * 100) / MAX_POT_VALUE;
    }
    void setup()
    {
        pinMode(_pin, INPUT);
        _timer.start();
    }
    void loop()
    {
        _timer.poll();
    }

protected:
    uint8_t _pin;
    Filter<uint16_t, FILTER_WINDOW> _filter;
    SoftwareTimer<Potentiometer> _timer;
};

#endif // POTENTIOMETER_H