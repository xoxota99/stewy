/*
 Toy library, for asynchronously blinking an (e.g.) LED. More generally, toggle between HIGH / LOW on some specific cycle.
*/

#ifndef BLINKER_H
#define BLINKER_H

#include "Arduino.h"

const int DEFAULT_BLINK_INTERVAL_MILLIS = 200;
enum BlinkStatus
{
  OFF,
  BLINKING
};

class Blinker
{

private:
  Blinker(){}; // Private so that it can  not be called

  bool _state = false;
  int _pin;
  int _numerator;                 // Which blink were we most recently / currently on?
  int _denominator;               // How blinks were requested?
  unsigned long _lastRise;        // Last time (in millis) we set the pin HIGH
  unsigned long _lastFall;        // Last time (in millis) we set the pin LOW
  unsigned int _onTime, _offTime; // how long should the LED be on / off (in millis)?
  bool _invert;                   // normally, the LED is off, and driven HIGH on blink. setting this to true inverts the signal, so that the LED is normally on, then driven LOW on blink.

public:
  /*
  Return an instance of Blinker, attached to a digital IO pin. This sets the
  pinMode to OUTPUT, and initializes the pin to LOW.
  */
  static Blinker attach(int pin, bool invert, int onTime, int offtime);

  /*
  Start blinking.
  */

  void blink(int times);

  /*
   Called in every iteration of the main loop.
  */
  void loop();

  /*
    Detach the Blinker from the output pin. subsequent calls to blink() or loop() will have no effect.
  */
  void detach();

  /*
    Get the current pinValue (HIGH or LOW). This is pulled directly from pin value registers in hardware.
  */
  bool getLedState();

  /*
    Is the blinker currently processing a series of blinks (regardless of the actual state of the LED / output pin).
  */
  BlinkStatus getStatus();

  /*
    Is the blinker "inverted"? (Normally OFF instead of ON?)
  */
  bool isInverted();
};

#endif // BLINKER_H
