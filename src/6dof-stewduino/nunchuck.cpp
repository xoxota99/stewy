#ifdef ENABLE_NUNCHUCK
#include "nunchuck.h"

void processNunchuck()
{
  // Read the current state
  nc.read();

  Serial.printf("Buttons (C/Z): %s/%s\t", (nc.getButtonC() ? "true" : "false"), (nc.getButtonZ() ? "true" : "false"));
  Serial.printf("Joystick (X/Y): %.2f/%.2f\t", nc.getJoyX(), nc.getJoyY());
  Serial.printf("Tilt (X/Y/X): %.2f/%.2f/%.2f\t", nc.getTiltX(), nc.getTiltY(), nc.getTiltZ());
  Serial.printf("Accel (X/Y/X): %.2f/%.2f/%.2f\n", nc.getAccelX(), nc.getAccelY(), nc.getAccelZ());

  if (nc.isOk()) {
    // de-bounce C
    if (nc.getButtonC()) {
      if (!cDown) {
        onCButtonDown();
      }
      cDown = true;
    } else {
      if (cDown) {
        onCButtonUp();
      }
      cDown = false;
    }

    // de-bounce Z
    if (nc.getButtonZ()) {
      if (!zDown) {
        onZButtonDown();
      }
      zDown = true;
    } else {
      if (zDown) {
        onZButtonUp();
      }
      zDown = false;
    }

    switch (mode) {
      case ANGLE:
        break;
      case CIRCLE:
        break;
      case EIGHT:
        break;
      case SQUARE:
        break;
      case SPIRAL:
        break;
      case MIDDLE:
      default:
        break;
    }
  } else {
    mode = MIDDLE; //Nunchuck is on the fritz. Default back to the center setpoint.
    dir = CW;
    Serial.println("WHOOPS, there was a problem reading the nunchuck!");
  }

  // Wait a short while
  //  delay(50);
}

//C Button changes direction.
void onCButtonDown() {
  switch (dir) {
    case CW:
      dir = CCW;
      break;
    case CCW:
      dir = CW;
      break;
  }

  Serial.printf("dir=%s", directionStrings[dir]);
}

void onCButtonUp() {

}

//Z Button changes modes.
void onZButtonDown() {
  switch (mode) {
    case ANGLE:
      mode = CIRCLE;
      break;
    case CIRCLE:
      mode = EIGHT;
      break;
    case EIGHT:
      mode = SQUARE;
      break;
    case SQUARE:
      mode = SPIRAL;
      break;
    case SPIRAL:
      mode = MIDDLE;
      break;
    case MIDDLE:
      mode = ANGLE;
    default:

      break;
  }

  Serial.printf("Mode=%s", modeStrings[mode]);
}

void onZButtonUp() {

}

#endif
