#ifndef HARDWARE_TIMER_H
#define HARDWARE_TIMER_H

#include "driver/timer.h"
#include <map>

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

template <typename T>
class HardwareTimer
{
private:
    uint8_t _timerId; // Auto-incremented timer index
    uint32_t _interval;
    void (*_callback)(T &);
    T &_object;
    hw_timer_t *_timer;
    portMUX_TYPE _timerMux;

    static std::map<uint8_t, HardwareTimer<T> *> instances; // Store instances
    static uint8_t nextTimerId;                             // Static counter for unique timer IDs

public:
    HardwareTimer(uint32_t interval, void (*callback)(T &), T &object)
        : _interval(interval), _callback(callback), _object(object),
          _timer(nullptr), _timerMux(portMUX_INITIALIZER_UNLOCKED)
    {
        _timerId = nextTimerId++;   // Assign unique timer ID and increment counter
        instances[_timerId] = this; // Register instance
        DEBUG_PRINT_VAL("HardwareTimer created with ID: ", _timerId);
        DEBUG_PRINT_VAL("Interval set to: ", _interval);
    }

    void start()
    {
        DEBUG_PRINT_VAL("Starting HardwareTimer with ID: ", _timerId);
        _timer = timerBegin(_timerId, 80, true);
        if (_timer == nullptr)
        {
            DEBUG_PRINT("Failed to initialize timer");
            return;
        }
        DEBUG_PRINT("Timer initialized successfully.");

        timerAttachInterrupt(_timer, &HardwareTimer::onTimerInterrupt, true);
        DEBUG_PRINT("Interrupt attached to timer.");

        timerAlarmWrite(_timer, _interval, true);
        DEBUG_PRINT_VAL("Timer alarm set with interval: ", _interval);

        timerAlarmEnable(_timer);
        DEBUG_PRINT("Timer alarm enabled.");
    }

    static void IRAM_ATTR onTimerInterrupt()
    {
        DEBUG_PRINT("Timer interrupt triggered.");
        for (auto &pair : instances)
        {
            if (pair.second)
            {
                DEBUG_PRINT_VAL("Processing timer ID: ", pair.first);
                portENTER_CRITICAL_ISR(&pair.second->_timerMux);
                pair.second->_callback(pair.second->_object);
                portEXIT_CRITICAL_ISR(&pair.second->_timerMux);
                DEBUG_PRINT_VAL("Callback executed for timer ID: ", pair.first);
            }
        }
    }
};

// Initialize static members
template <typename T>
std::map<uint8_t, HardwareTimer<T> *> HardwareTimer<T>::instances;

template <typename T>
uint8_t HardwareTimer<T>::nextTimerId = 0; // Start at 0 and increment with each instance

#endif // HARDWARE_TIMER_H