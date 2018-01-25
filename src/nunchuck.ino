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
#include "Logger.h"

void processNunchuck()
{
  // Read the current state
  nc.read();

  if (nc.isOk()) {
    if(chuckData.buttonC != nc.getButtonC() ||
    chuckData.buttonZ != nc.getButtonZ() ||
    chuckData.joyX != nc.getJoyX() + chuckTrim.joyX ||
    chuckData.joyY != nc.getJoyY() + chuckTrim.joyY ||
    abs(chuckData.tiltX - nc.getTiltX() + chuckTrim.tiltX) <= TILT_EPSILON ||
    abs(chuckData.tiltY - nc.getTiltY() + chuckTrim.tiltY) <= TILT_EPSILON ||
    abs(chuckData.tiltZ - nc.getTiltZ() + chuckTrim.tiltZ) <= TILT_EPSILON ||
    chuckData.accelX != nc.getAccelX() + chuckTrim.accelX ||
    chuckData.accelY != nc.getAccelY() + chuckTrim.accelY ||
    chuckData.accelZ != nc.getAccelZ() + chuckTrim.accelZ) {

      // de-bounce C
      if (nc.getButtonC()) {
        if (!chuckData.buttonC) { //the button was up.
          onCButtonDown();        //and now it's down.
        }
      } else {
        if (chuckData.buttonC) {  //the button was down
          onCButtonUp();          //and now it's up.
        }
      }

      // de-bounce Z
      if (nc.getButtonZ()) {
        if (!chuckData.buttonZ) {
          onZButtonDown();
        }
      } else {
        if (chuckData.buttonZ) {
          onZButtonUp();
        }
      }


      switch (mode) {
        case CONTROL:
          {
            //move the platform to the angle represented by the nunchuck joystick angle.
            double pitch = -((((float)nc.getJoyY() + 100)/200 * 43) - 23);    //a number between -20 and 23
            double roll = (((float)nc.getJoyX() + 100)/200 * 43) - 23;    //a number between -23 and 20

            Logger::trace("Moving to %.2f , %.2f",pitch,roll);
            stu.moveTo(sp_servo, pitch, roll);

            break;
          }
      case CIRCLE:
        break;
      case EIGHT:
        break;
      case SQUARE:
        break;
      case SETPOINT:
      default:
        break;
      }

      chuckData.buttonC = nc.getButtonC() ;
      chuckData.buttonZ = nc.getButtonZ() ;
      chuckData.joyX = nc.getJoyX() ;
      chuckData.joyY = nc.getJoyY() ;
      chuckData.tiltX = nc.getTiltX();
      chuckData.tiltY = nc.getTiltY();
      chuckData.tiltZ = nc.getTiltZ();
      chuckData.accelX = nc.getAccelX() ;
      chuckData.accelY = nc.getAccelY() ;
      chuckData.accelZ = nc.getAccelZ() ;

      Logger::trace("Buttons (C/Z): %s/%s\tJoystick (X/Y): %d/%d\tTilt (X/Y/Z): %.2f/%.2f/%.2f\tAccel (X/Y/Z): %d/%d/%d",
      (chuckData.buttonC ? "true" : "false"),
      (chuckData.buttonZ ? "true" : "false"),
      chuckData.joyX,
      chuckData.joyY,
      chuckData.tiltX,
      chuckData.tiltY,
      chuckData.tiltZ,
      chuckData.accelX,
      chuckData.accelY,
      chuckData.accelZ);

    } else {
      Logger::trace("No change in chuckData.");
    }

  } else {
    mode = SETPOINT; //Nunchuck is on the fritz / disconnected. Default back to the center setpoint.
    dir = CW;
    Logger::trace("WHOOPS, there was a problem reading the nunchuck!");
  }

  // Wait a short while
  //  delay(50);
}

//C Button changes direction.
void onCButtonDown() {
  if(millis()-chuckData.lastCButtonDown < NUNCHUCK_DBLCLICK_THRESHOLD_MS) {
    chuckData.lastCButtonDown=millis(); //NOTE: This could get dicey, if stewduino is left running for more than 49 days, according to https://www.arduino.cc/reference/en/language/functions/time/millis/

    //Treat this as a double-click, instead of a single-click.
    return onCButtonDblClick();
  } else {
    Logger::trace("CButtonDown");
    chuckData.lastCButtonDown = millis();
    setMode(Mode((mode+1) % 5));

    Logger::debug("Mode = %s",modeStrings[mode]);

  }
}

void setMode(Mode _mode){
  mode = _mode;
  blink(int(mode)+1);
}

void onCButtonUp() {
  Logger::trace("CButtonUp");
}

void onCButtonDblClick(){
  Logger::trace("CButtonDblClick");
}

//Z Button changes modes.
void onZButtonDown() {
  if(millis()-chuckData.lastZButtonDown < NUNCHUCK_DBLCLICK_THRESHOLD_MS) {
    chuckData.lastZButtonDown=millis(); //NOTE: This could get dicey, if stewduino is left running for more than 49 days, according to https://www.arduino.cc/reference/en/language/functions/time/millis/

    //Treat this as a double-click, instead of a single-click.
    return onZButtonDblClick();
  } else {
    Logger::trace("ZButtonDown");
    switch(mode)
    {
      case CIRCLE:
      case EIGHT:
      case SQUARE:
        //We are in a mode that supports the concept of "direction"

        chuckData.lastZButtonDown = millis();
        dir = Direction((dir+1) % 2 );

        Logger::debug("Direction = %s",directionStrings[dir]);
        break;
      case CONTROL:
        controlSubMode = ControlSubMode((controlSubMode+1) % 2);

        Logger::debug("ControlSubMode = %s",subModeStrings[controlSubMode]);
      default:
        break;
    }
  }
}

void onZButtonUp() {
  Logger::trace("ZButtonUp");
}

void onZButtonDblClick(){
  Logger::trace("ZButtonDblClick");
}
#endif
