#ifdef ENABLE_TOUCHSCREEN
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

#include "touch.h"

void processTouchscreen() {
  static unsigned long ballLastSeen;

  //we should NOT process the touchscreen output if the PID is in MANUAL mode.
  if(pitchPID.GetMode()!=MANUAL &&
     rollPID.GetMode()!=MANUAL) {
    // a point object holds x y and z coordinates
    TSPoint p = ts.getPoint();

    inputX = p.x; //1023 * (p.x-TS_MIN_X)/TS_WIDTH;
    inputY = p.y; //-(TS_SLOPE_Y*(TS_MAX_X-p.x));

    if(p.x > TS_MIN_X
      && p.x < TS_MAX_X
      && p.y > TS_MIN_Y
      && p.y < TS_MAX_Y) {
        ballLastSeen=millis();

        //setpoint may have changed. setpoint is on a scale of -1.0 to +1.0, in both axes.
        setpointX = map(setpoint.x, -1.0, 1.0, TS_MIN_X, TS_MAX_X);
        setpointY = map(setpoint.y, -1.0, 1.0, TS_MIN_Y, TS_MAX_Y);

        // Logger::debug("setpointY=%f",setpointY);
        double newOutX = rollPID.Compute(inputX, setpointX);
        double newOutY = pitchPID.Compute(inputY, setpointY);

        if(newOutX != outputX || newOutY != outputY){
          outputX = newOutX;
          outputY=newOutY;

          Logger::trace("TOUCH: %d\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f", rollPID.GetMode(), millis(), inputX, inputY, setpoint.x, setpoint.y, setpointX, setpointY, outputX, outputY);

          float roll = map(outputX, ROLL_PID_LIMIT_MIN, ROLL_PID_LIMIT_MAX, MIN_ROLL, MAX_ROLL);
          float pitch = map(outputY, PITCH_PID_LIMIT_MIN, PITCH_PID_LIMIT_MAX, MIN_PITCH, MAX_PITCH);

          unsigned long m = millis();

          Logger::debug("Time/InX/InY/OutX/OutY/roll/pitch:\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f", m, inputX, inputY, outputX, outputY, roll, pitch);

          stu.moveTo(sp_servo, pitch, roll);
          // stu.moveTo(sp_servo, 0, roll);  //isolate to X-axis only, while we tune pids.
        }
    } else {
      //The ball has disappeared. Start a countdown.
      unsigned long m = millis();
      if(m-ballLastSeen >= LOST_BALL_TIMEOUT){
        //start a countdown. Until you hit zero, do nothing. This is to avoid sudden
        //movement to home, while the ball is near the edge of the platform, and
        //possibly still recoverable.
        stu.home(sp_servo);
      }
    }
  }
}

#endif
