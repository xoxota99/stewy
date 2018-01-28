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
  // a point object holds x y and z coordinates
  TSPoint p = ts.getPoint();

  inputX = p.x; //1023 * (p.x-TS_MIN_X)/TS_WIDTH;
  inputY = p.y; //-(TS_SLOPE_Y*(TS_MAX_X-p.x));

  if(p.x > TS_MIN_X
    && p.x < TS_MAX_X
    && p.y > TS_MIN_Y
    && p.y < TS_MAX_Y) {

      //setpoint may have changed. setpoint is on a scale of -1.0 to +1.0, in both axes.
      setpointX = TS_MIN_X + ((setpoint.x+1)/2 *TS_WIDTH);
      setpointY = TS_MIN_Y + ((setpoint.y+1)/2 *TS_HEIGHT);

      if(rollPID.Compute() ||
        pitchPID.Compute()) {
        // PID controller will deliver an output in the range -128 to +128.
        Logger::trace("TOUCH: %d\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f", rollPID.GetMode(), millis(), inputX, inputY, setpoint.x, setpoint.y, setpointX, setpointY, outputX, outputY);

        //clamp pitch/roll
        // float pitch=MIN_PITCH + (float)outputY/128 * PITCH_BAND;
        float roll=(float)outputX/256 * ROLL_BAND;

        if(millis()%100<=5) {
          Logger::debug("OutX/roll: %.2f\t%.2f",outputX, roll);
        }
        // stu.moveTo(sp_servo, 0, roll);
      }
  } else {
    // stu.home(sp_servo);
  }
}

#endif
