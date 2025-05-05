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

#ifndef PWM_H
#define PWM_H

#include <driver/ledc.h> // PWM library.  Works with 3.0.7
#include "esp_err.h"     // ESP32 error codes

class PWM
{
public:
    PWM(ledc_timer_config_t timer, ledc_channel_config_t channel) : _timer(timer), _channel(channel) {}
    void setup()
    {
        // Initialize Heat PWM
        ESP_ERROR_CHECK(ledc_timer_config(&_timer));
        ESP_ERROR_CHECK(ledc_channel_config(&_channel));
    };
    void update(uint32_t duty)
    {
        ledc_set_duty(_timer.speed_mode, _channel.channel, duty);
        ledc_update_duty(_timer.speed_mode, _channel.channel);
    };
    uint32_t getDuty()
    {
        uint32_t duty = ledc_get_duty(_timer.speed_mode, _channel.channel);
        return (duty);
    }

private:
    ledc_channel_config_t _channel;
    ledc_timer_config_t _timer;
};

#endif // PWM_H
