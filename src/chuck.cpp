#ifdef ENABLE_NUNCHUCK
/*
   6dof-stewduino
   Copyright (C) 2018  Philippe Desrosiers

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "nunchuck.h"

void processNunchuck()
{
  // Read the current state
  nc.readData();

  if (nc.isOk())
  {

    // de-bounce C
    if (nc.getButtonC())
    {
      if (!chuckData.buttonC)
      {                  // the button was up.
        onCButtonDown(); // and now it's down.
      }
    }
    else
    {
      if (chuckData.buttonC)
      {                // the button was down
        onCButtonUp(); // and now it's up.
      }
    }

    // de-bounce Z
    if (nc.getButtonZ())
    {
      if (!chuckData.buttonZ)
      {
        onZButtonDown();
      }
    }
    else
    {
      if (chuckData.buttonZ)
      {
        onZButtonUp();
      }
    }

    switch (mode)
    {
    case SETPOINT:
    {
      // Joystick moves the setpoint.
      // joystick is on a scale of -100 to 100
      // setpoint is on  ascale of -1 to 1.
      // at max stick, the setpoint should move at a rate of SETPOINT_MOVE_RATE.
      if (abs(nc.getJoyX()) > deadBand.x ||
          abs(nc.getJoyY()) > deadBand.y)
      {
        float x = setpoint.x, y = setpoint.y;

        // TODO: Magic numbers.
        x += (SETPOINT_MOVE_RATE * nc.getJoyX() / 100.0);
        y += (SETPOINT_MOVE_RATE * nc.getJoyY() / 100.0);

        x = constrain(x, -1, 1);
        y = constrain(y, -1, 1);

        setpoint = {x, y};
        Log.trace("SP: %.2f\t%.2f", setpoint.x, setpoint.y);
      }

      // NOTE: Actual platform behavior in this mode is handled in touch.ino

      break;
    }
    case CONTROL:
      if (abs(nc.getJoyX()) > deadBand.x ||
          abs(nc.getJoyY()) > deadBand.y)
      {
        switch (controlSubMode)
        {
        case PITCH_ROLL:
        {
          double pitch = map(nc.getJoyY(), -100, 100, MIN_PITCH, MAX_PITCH);
          double roll = map(nc.getJoyX(), -100, 100, MIN_ROLL, MAX_ROLL);

          Log.trace("PITCH_ROLL moving to %.2f , %.2f", pitch, roll);
          stu.moveTo(sp_servo, pitch, roll);

          break;
        }
        case SWAY_SURGE:
        {
          double surge = map(nc.getJoyY(), -100, 100, MIN_SURGE, MAX_SURGE);
          double sway = map(nc.getJoyX(), -100, 100, MIN_SWAY, MAX_SWAY);

          Log.trace("SWAY_SURGE moving to %.2f , %.2f", sway, surge);
          stu.moveTo(sp_servo, sway, surge, 0, 0, 0, 0);

          break;
        }
        case HEAVE_YAW:
        {
          double heave = map(nc.getJoyY(), -100, 100, MIN_HEAVE, MAX_HEAVE);
          double yaw = map(nc.getJoyX(), -100, 100, MIN_YAW, MAX_YAW);

          Log.trace("HEAVE_YAW moving to %.2f , %.2f", heave, yaw);
          stu.moveTo(sp_servo, 0, 0, heave, 0, 0, yaw);
        }
        break;
        }
      }
      else
      {
        // move to center.
        Log.trace("Joystick in deadband. (%d, %d) vs. (%d, %d)", nc.getJoyX(), nc.getJoyY(), deadBand.x, deadBand.y);
        stu.home(sp_servo);
      }
      break;
    case CIRCLE:
    {
      // Y-axis joystick controls the speed of the circle.
      //  TODO: Magic numbers.
      double inc = (float)nc.getJoyY() / 1000; // a number between -0.1 and 0.1
      if (abs(nc.getJoyY()) > deadBand.y)
      {
        sp_speed += inc;
        sp_speed = constrain(sp_speed, 0.001, 1);
      }

      // max speed is, say, 1 degree. So at max speed, we want to move 1 degree around a circle of radius sp_radius.
      float step = radians(1) * sp_speed;
      float x = cos(step) * setpoint.x - sin(step) * setpoint.y;
      float y = sin(step) * setpoint.x + cos(step) * setpoint.y;
      setpoint = {x, y};

      Log.trace("SP: %.2f\t%.2f", setpoint.x, setpoint.y);
      break;
    }
    case EIGHT:
    {
      // Lemniscate of Bernoulli (https://en.wikipedia.org/wiki/Lemniscate_of_Bernoulli)
      unsigned long t = millis();
      float scale = 2 / (3 - cos(2 * t));
      float x = scale * cos(t);
      float y = scale * sin(2 * t) / 2;
      setpoint = {x, y};

      Log.trace("SP: %.2f\t%.2f", setpoint.x, setpoint.y);
      break;
    }
    case SQUARE:
      if (millis() - lastSquareShiftTime > SQUARE_DELAY_MS)
      {
        if ((setpoint.x > 0 && setpoint.y > 0 && dir == CCW) ||
            (setpoint.x > 0 && setpoint.y < 0 && dir == CW) ||
            (setpoint.x < 0 && setpoint.y > 0 && dir == CW) ||
            (setpoint.x < 0 && setpoint.y < 0 && dir == CCW))
        {
          setpoint.x = -setpoint.x;
        }
        else
        {
          setpoint.y = -setpoint.y;
        }
        Log.trace("SP: %.2f\t%.2f", setpoint.x, setpoint.y);
      }
      break;
    default:
      break;
    }

    chuckData.buttonC = nc.getButtonC();
    chuckData.buttonZ = nc.getButtonZ();
    chuckData.joyX = nc.getJoyX();
    chuckData.joyY = nc.getJoyY();
    chuckData.tiltX = nc.getTiltX();
    chuckData.tiltY = nc.getTiltY();
    chuckData.tiltZ = nc.getTiltZ();
    chuckData.accelX = nc.getAccelX();
    chuckData.accelY = nc.getAccelY();
    chuckData.accelZ = nc.getAccelZ();

    // Log.trace("Buttons (C/Z): %s/%s\tJoystick (X/Y): %d/%d\tTilt (X/Y/Z): %.2f/%.2f/%.2f\tAccel (X/Y/Z): %d/%d/%d",
    // (chuckData.buttonC ? "true" : "false"), (chuckData.buttonZ ? "true" : "false"),
    // chuckData.joyX, chuckData.joyY,
    // chuckData.tiltX, chuckData.tiltY, chuckData.tiltZ,
    // chuckData.accelX, chuckData.accelY, chuckData.accelZ);
  }
  else
  {
    mode = SETPOINT; // Nunchuck is on the fritz / disconnected. Default back to the center setpoint.
    dir = CW;
    Log.trace("No nunchuck.");
  }

  // Wait a short while
  //  delay(50);
}

// C Button changes modes.
void onCButtonDown()
{
  if (millis() - chuckData.lastCButtonDown < NUNCHUCK_DBLCLICK_THRESHOLD_MS)
  {
    chuckData.lastCButtonDown = millis(); // NOTE: This could get dicey, if stewduino is left running for more than 49 days, according to https://www.arduino.cc/reference/en/language/functions/time/millis/

    // Treat this as a double-click, instead of a single-click.
    return onCButtonDblClick();
  }
  else
  {
    Log.trace("CButtonDown");
    chuckData.lastCButtonDown = millis();
    setMode(Mode((mode + 1) % 5));
  }
}

void setMode(Mode newMode)
{

  if (mode != newMode)
  {

#ifdef ENABLE_TOUCHSCREEN
    Mode oldMode = mode;
    if ((oldMode == CONTROL) != (newMode == CONTROL))
    {
      // Turn off the PIDs if we're moving to CONTROL mode.
      // Turn them on if we're moving out of CONTROL mode.
      int onOff = newMode == CONTROL ? MANUAL : AUTOMATIC;
      Log.trace("setting PID mode to %s", onOff ? "AUTOMATIC" : "MANUAL");
      rollPID.SetMode(onOff);
      pitchPID.SetMode(onOff);
    }
#endif

    mode = newMode;

    Log.trace("Mode = %s", modeStrings[mode]);
    blinker.blink(int(mode) + 1);

    // initialize the mode
    sp_speed = DEFAULT_SPEED;          // reset to default speed.
    controlSubMode = DEFAULT_SUB_MODE; // reset to default subMode.
    stu.home(sp_servo);                // reset the platform.

    // TODO: Make sure that the radius never goes outside the actual plate.
    sp_radius = sqrt(pow(setpoint.x - DEFAULT_SETPOINT.x, 2) + pow(setpoint.y - DEFAULT_SETPOINT.y, 2)); // radius is based on wherever the setpoint is right now.
  }
}

void onCButtonUp()
{
  Log.trace("CButtonUp");
}

void onCButtonDblClick()
{
  Log.trace("CButtonDblClick");
  setMode(DEFAULT_MODE);
}

// Z Button changes direction.
void onZButtonDown()
{
  if (millis() - chuckData.lastZButtonDown < NUNCHUCK_DBLCLICK_THRESHOLD_MS)
  {
    chuckData.lastZButtonDown = millis(); // NOTE: This could get dicey, if stewduino is left running for more than 49 days, according to https://www.arduino.cc/reference/en/language/functions/time/millis/

    // Treat this as a double-click, instead of a single-click.
    return onZButtonDblClick();
  }
  else
  {
    Log.trace("ZButtonDown");
    switch (mode)
    {
    case CIRCLE:
    case EIGHT:
    case SQUARE:
      // We are in a mode that supports the concept of "direction"

      chuckData.lastZButtonDown = millis();
      dir = Direction((dir + 1) % 2);

      Log.trace("Direction = %s", directionStrings[dir]);
      break;
    case CONTROL:
      // change subMode.
      controlSubMode = ControlSubMode((controlSubMode + 1) % 3);

      Log.trace("ControlSubMode = %s", subModeStrings[controlSubMode]);
    case SETPOINT:
      // reset the setpoint to the default.
      setpoint = DEFAULT_SETPOINT;
      Log.trace("%.3f\t%.3f", setpoint.x, setpoint.y);
    default:
      break;
    }
  }
}

void onZButtonUp()
{
  Log.trace("ZButtonUp");
}

void onZButtonDblClick()
{
  Log.trace("ZButtonDblClick");
}
#endif
