#include "Blinker.h"
/*
Return an instance of Blinker, attached to a digital IO pin. This sets the
pinMode to OUTPUT, and initializes the pin to LOW.
*/
Blinker Blinker::attach(int pin, bool invert, int onTime, int offTime)
{
  Blinker b;
  b._pin = pin;
  b._invert = invert;
  b._onTime = onTime;
  b._offTime = offTime;
  b._state = false;
  pinMode(b._pin, OUTPUT);
  digitalWrite(b._pin, b._invert ? HIGH : LOW);
  b._lastFall = millis();
  return b;
}

/*
  Start blinking.
*/

void Blinker::blink(int times)
{
  if (_pin > -1)
  {
    _denominator = times;
    _numerator = 0;
    _lastFall = 0;
    _state = true;
    digitalWrite(_pin, _invert ? LOW : HIGH); // start.
    _lastRise = millis();
  }
}

/*
 Called in every iteration of the main loop.
*/
void Blinker::loop()
{
  if (_pin > -1)
  {
    if (_numerator < _denominator)
    {
      if (_state && millis() - _lastRise >= _onTime)
      {
        // time to turn off.
        _state = false;
        digitalWrite(_pin, _invert ? HIGH : LOW);
        _numerator++;
        _lastFall = millis();
      }
      else if (!_state && millis() - _lastFall >= _offTime)
      {
        _state = true;
        // time to turn on.
        digitalWrite(_pin, _invert ? LOW : HIGH);
        _lastRise = millis();
      }
    }
  }
}

/*
  Detach the Blinker from the output pin. subsequent calls to blink() or loop() will have no effect.
*/
void Blinker::detach()
{
  digitalWrite(_pin, _invert ? HIGH : LOW);
  _lastFall = millis();

  _pin = -1; // Yes, okay, this is a magic number.
}

/*
  Is the led currently on?
*/
bool Blinker::getLedState()
{
  return _state;
}

/*
  Is the blinker currently "On"? That is, is the blinker currently processing a series of blinks (regardless of the actual state of the LED / output pin).
*/
BlinkStatus Blinker::getStatus()
{
  if (_pin > -1 && _numerator < _denominator)
  {
    return BLINKING;
  }
  return OFF;
}
