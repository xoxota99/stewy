#ifdef ENABLE_TOUCHSCREEN
#include "touch.h"

void processTouchscreen() {
  // a point object holds x y and z coordinates
  TSPoint p = ts.getPoint();

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > ts.pressureThreshhold) {
     Serial.printf("X = %d\tY = %d\tPressure = %d",p.x,p.y,p.z);
     //TODO: start up the PID controller, and give it something to do.
  }

  delay(100);
}

#endif
