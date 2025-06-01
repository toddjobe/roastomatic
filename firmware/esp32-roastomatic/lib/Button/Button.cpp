#include "Button.h"

void Button::setup()
{
  pinMode(_pin, INPUT_PULLUP);
  attachInterruptArg(digitalPinToInterrupt(_pin), handleInterrupt, this, FALLING);
}

void Button::reset()
{
  _state = 0;
}

void Button::setNStates(int nStates)
{
  reset();
  _nStates = nStates;
}

int IRAM_ATTR Button::count()
{
  return _state;
}

bool IRAM_ATTR Button::changed()
{
  bool changed = _state != _prev_state;
  if (changed)
  {
    _prev_state = _state;
  }
  return changed;
}

// Generic ISR (Interrupt Service Routine)
void IRAM_ATTR Button::handleInterrupt(void *arg)
{
  Button *btn = static_cast<Button *>(arg); // Cast argument back to Button object
  unsigned int now = micros();

  if ((now - btn->_lastTriggerTime) <= debounceDelay)
    return; // Ignore if within debounce delay

  btn->_lastTriggerTime = now;

  if (btn->_isIn)
  { // Button was pressed
    btn->_isIn = false;
    if ((now - btn->_lastInTime) < outDelay)
    { // Valid release within outDelay
      btn->_prev_state = btn->_state;
      btn->_state = (btn->_state + 1) % btn->_nStates;
    }
  }
  else
  { // Button is pressed
    btn->_isIn = true;
    btn->_lastInTime = now;
  }
}
