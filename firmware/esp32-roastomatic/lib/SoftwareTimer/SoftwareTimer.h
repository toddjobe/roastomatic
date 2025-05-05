#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H

#include <Arduino.h> // Include Arduino library for millis() function

// A blocking timer
template <typename T>
class SoftwareTimer
{
private:
    uint32_t _interval;     // Interval in milliseconds
    void (*_callback)(T &); // _callback function to call
    T &_object;             // _object passed to _callback
    uint32_t _last_time;    // Last time the _callback was triggered

public:
    SoftwareTimer(uint32_t interval, void (*callback)(T &), T &object)
        : _interval(interval), _callback(callback), _object(object), _last_time(0) {}

    // Update method (check if the interval has passed)
    void poll()
    {
        uint32_t currentTime = millis();
        if ((currentTime - _last_time >= _interval) ||
            (_last_time > currentTime && (UINT32_MAX - _last_time + currentTime + 1) >= _interval))
        {
            _last_time = currentTime;
            _callback(_object); // Call the passed _callback function
        }
    }

    // setup the timer
    void start()
    {
        _last_time = millis();
    }
};

#endif // SOFTWARE_TIMER_H
