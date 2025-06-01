#ifndef HARDWARE_TIMER_H
#define HARDWARE_TIMER_H

#include "driver/gptimer.h"

// Debugging macro
#define DEBUG 0 // Set to 1 to enable debugging, 0 to disable

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
public:
    HardwareTimer(T* instance, uint64_t interval_us, bool auto_reload = true)
        : _instance(instance), _interval_us(interval_us), _auto_reload(auto_reload),
          _timer(nullptr), _timerMux(portMUX_INITIALIZER_UNLOCKED)
    {
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
        for (auto &pair : instances)
        {
            if (pair.second)
            {
                portENTER_CRITICAL_ISR(&pair.second->_timerMux);
                pair.second->_callback(pair.second->_object);
                portEXIT_CRITICAL_ISR(&pair.second->_timerMux);
            }
        }
    }
    

private:
    static bool IRAM_ATTR timer_isr(gptimer_handle_t, const gptimer_alarm_event_data_t* event, void* user_data)
    {
        HardwareTimer* self = static_cast<HardwareTimer*>(user_data)
        if (self->_instance &

    gptimer_handle_t _timer = nullptr;
    T* _instance = nullptr;
    uint64_t _interval_us;
    bool _auto_reload;
    bool (T::*_callback)(const gptimer_alarm_event_data_t*) = nullptr;
};

#endif // HARDWARE_TIMER_H
