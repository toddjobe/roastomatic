#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button
{
public:
  Button(int pin, int nStates = 2) : _pin(pin), _nStates(nStates), _state(0), _prev_state(0), _lastTriggerTime(0), _lastInTime(0) {};
  void setup();
  void reset();
  void setNStates(int nStates = 2);
  int IRAM_ATTR count();                            // Virtual method for subclasses
  bool IRAM_ATTR changed();                         // notification
  static void IRAM_ATTR handleInterrupt(void *arg); // Generic ISR

protected:
  int _pin;
  int _nStates = 2;
  int _state = 0;
  int _prev_state = 0;
  int _isIn = false;
  volatile unsigned long _lastTriggerTime;
  volatile unsigned long _lastInTime;
  static const unsigned long debounceDelay = 50 * 1000; // 50ms in microseconds
  static const unsigned long outDelay = 1000 * 1000;    // 1000ms in microseconds
};

#endif
