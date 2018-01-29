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

    //TODO: Quite a few magic numbers that should be refactored out.
    switch (mode) {
      case SETPOINT: {
          // Joystick moves the setpoint.
          // joystick is on a scale of -100 to 100
          // setpoint is on  ascale of -1 to 1.
          // at max stick, the setpoint should move at a rate of SETPOINT_MOVE_RATE.
          if(abs(nc.getJoyX())>deadBand.x ||
            abs(nc.getJoyY())>deadBand.y) {
            float x=setpoint.x, y=setpoint.y;
            x+=(SETPOINT_MOVE_RATE * nc.getJoyX()/100.0);
            y+=(SETPOINT_MOVE_RATE * nc.getJoyY()/100.0);

            x=fmin(fmax(x,-1),1);
            y=fmin(fmax(y,-1),1);

            setpoint = {x,y};
            Logger::trace("SP: %.2f\t%.2f",setpoint.x,setpoint.y);
          }

          //NOTE: Actual platform behavior in this mode is handled in touch.ino

          break;
        }
      case CONTROL:
        if(abs(nc.getJoyX()) > deadBand.x ||
           abs(nc.getJoyY()) > deadBand.y) {
          switch (controlSubMode) {
            case PITCH_ROLL: {
              double pitch = -((((float)nc.getJoyY() + 100)/200 * PITCH_BAND) + MIN_PITCH);    //MAGIC: a number between -20 and 23
              double roll = (((float)nc.getJoyX() + 100)/200 * ROLL_BAND) + MIN_ROLL;        //MAGIC: a number between -23 and 20

              Logger::trace("PITCH_ROLL moving to %.2f , %.2f",pitch,roll);
              stu.moveTo(sp_servo, pitch, roll);

              break;
            }
            case SWAY_SURGE: {
              double surge = (((float)nc.getJoyY() + 100)/200 * SURGE_BAND) + MIN_SURGE;    //a number between -59 and 59
              double sway = (((float)nc.getJoyX() + 100)/200 * SWAY_BAND) + MIN_SWAY;        //a number between -71 and 56

              Logger::trace("SWAY_SURGE moving to %.2f , %.2f",sway,surge);
              stu.moveTo(sp_servo, sway, surge, 0, 0, 0, 0);

              break;
            }
            case HEAVE_YAW: {
              double heave = (((float)nc.getJoyY() + 100)/200 * HEAVE_BAND) + MIN_HEAVE;    //a number between -22 and 27
              double yaw = (((float)nc.getJoyX() + 100)/200 * YAW_BAND) + MIN_YAW;        //a number between -69 and 69

              Logger::trace("HEAVE_YAW moving to %.2f , %.2f",heave,yaw);
              stu.moveTo(sp_servo, 0, 0, heave, 0, 0, yaw);

            }
            break;
          }
        } else {
          //move to center.
          Logger::trace("Joystick in deadband. (%d, %d) vs. (%d, %d)",nc.getJoyX(), nc.getJoyY(),deadBand.x,deadBand.y);
          stu.home(sp_servo);
        }
        break;
    case CIRCLE: {
        //Y-axis joystick controls the speed of the circle.

        double inc = (float)nc.getJoyY()/1000;    //a number between -0.1 and 0.1
        if(abs(nc.getJoyY()) > deadBand.y){
          sp_speed += inc;
          sp_speed = fmin(fmax(sp_speed,0.001),1);
        }

        //max speed is, say, 1 degree. So at max speed, we want to move 1 degree around a circle of radius sp_radius.
        float step = radians(1) * sp_speed;
        float x = cos(step) * setpoint.x - sin(step) * setpoint.y;
        float y = sin(step) * setpoint.x + cos(step) * setpoint.y;
        setpoint = {x,y};

        Logger::trace("SP: %.2f\t%.2f",setpoint.x,setpoint.y);
        break;
      }
    case EIGHT: {
        //Lemniscate of Bernoulli (https://en.wikipedia.org/wiki/Lemniscate_of_Bernoulli)
        unsigned long t = millis();
        float scale = 2 / (3 - cos(2*t));
        float x = scale * cos(t);
        float y = scale * sin(2*t) / 2;
        setpoint = {x,y};

        Logger::trace("SP: %.2f\t%.2f",setpoint.x,setpoint.y);
        break;
      }
    case SQUARE:
      if(millis() - lastSquareShiftTime > SQUARE_DELAY_MS) {
        if((setpoint.x>0 && setpoint.y > 0 && dir==CCW) ||
          (setpoint.x>0 && setpoint.y < 0 && dir==CW) ||
          (setpoint.x<0 && setpoint.y > 0 && dir==CW) ||
          (setpoint.x<0 && setpoint.y < 0 && dir==CCW)){
          setpoint.x = -setpoint.x;
        } else {
          setpoint.y = -setpoint.y;
        }
        Logger::trace("SP: %.2f\t%.2f",setpoint.x,setpoint.y);
      }
      break;
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

    // Logger::trace("Buttons (C/Z): %s/%s\tJoystick (X/Y): %d/%d\tTilt (X/Y/Z): %.2f/%.2f/%.2f\tAccel (X/Y/Z): %d/%d/%d",
    // (chuckData.buttonC ? "true" : "false"), (chuckData.buttonZ ? "true" : "false"),
    // chuckData.joyX, chuckData.joyY,
    // chuckData.tiltX, chuckData.tiltY, chuckData.tiltZ,
    // chuckData.accelX, chuckData.accelY, chuckData.accelZ);

  } else {
    mode = SETPOINT; //Nunchuck is on the fritz / disconnected. Default back to the center setpoint.
    dir = CW;
    Logger::trace("No nunchuck.");
  }

  // Wait a short while
  //  delay(50);
}

//C Button changes modes.
void onCButtonDown() {
  if(millis()-chuckData.lastCButtonDown < NUNCHUCK_DBLCLICK_THRESHOLD_MS) {
    chuckData.lastCButtonDown=millis(); //NOTE: This could get dicey, if stewduino is left running for more than 49 days, according to https://www.arduino.cc/reference/en/language/functions/time/millis/

    //Treat this as a double-click, instead of a single-click.
    return onCButtonDblClick();
  } else {
    Logger::trace("CButtonDown");
    chuckData.lastCButtonDown = millis();
    setMode(Mode((mode+1) % 5));
  }
}

void setMode(Mode newMode){

  if(mode !=newMode){

#ifdef ENABLE_TOUCHSCREEN
    Mode oldMode = mode;
    if((oldMode == CONTROL) != (newMode == CONTROL)) {
      // Turn off the PIDs if we're moving to CONTROL mode.
      // Turn them on if we're moving out of CONTROL mode.
      int onOff = newMode == CONTROL ? MANUAL : AUTOMATIC;
      Logger::debug("setting PID mode to %s",onOff ? "AUTOMATIC" : "MANUAL");
      rollPID.SetMode(onOff);
      pitchPID.SetMode(onOff);
    }
#endif

    mode = newMode;

    Logger::debug("Mode = %s",modeStrings[mode]);
    blinker.blink(int(mode)+1);

    //initialize the mode
    sp_speed = DEFAULT_SPEED;           //reset to default speed.
    controlSubMode = DEFAULT_SUB_MODE;  //reset to default subMode.
    stu.home(sp_servo);                 //reset the platform.

    //TODO: Make sure that the radius never goes outside the actual plate.
    sp_radius = sqrt(pow(setpoint.x - DEFAULT_SETPOINT.x,2) + pow(setpoint.y - DEFAULT_SETPOINT.y,2));  //radius is based on wherever the setpoint is right now.
  }
}

void onCButtonUp() {
  Logger::trace("CButtonUp");
}

void onCButtonDblClick(){
  Logger::trace("CButtonDblClick");
  setMode(DEFAULT_MODE);
}

//Z Button changes direction.
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
        //change subMode.
        controlSubMode = ControlSubMode((controlSubMode+1) % 3);

        Logger::debug("ControlSubMode = %s",subModeStrings[controlSubMode]);
      case SETPOINT:
        //reset the setpoint to the default.
        setpoint = DEFAULT_SETPOINT;
        Logger::debug("%.3f\t%.3f",setpoint.x,setpoint.y);
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
