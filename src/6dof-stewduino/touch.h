#ifdef ENABLE_TOUCHSCREEN
#include <TouchScreen.h> //from https://github.com/adafruit/Touch-Screen-Library

#define XP A0   // can be a digital pin
#define XM A1  // must be an analog pin, use "An" notation!

#define YM A2   // can be a digital pin
#define YP A3  // must be an analog pin, use "An" notation!

#define TS_OHMS 300 //resistance between X+ and X-

void processTouchscreen();


// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, TS_OHMS);

#endif
