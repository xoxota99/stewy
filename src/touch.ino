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
  static int xmin=800, xmax, ymin=2000, ymax;
  static int i;
  static TSPoint lastPoint = ts.getPoint();
  bool diff=false;

  TSPoint p = ts.getPoint();

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  // if (p.z > ts.pressureThreshhold) {

  // diff = (p.x < xmin ||
  //   p.x > xmax ||
  //   p.y < ymin ||
  //   p.y > ymax);
  // if(diff){
  //   xmin = min(xmin,p.x);
  //   xmax = max(xmax,p.x);
  //     ymin = min(ymin,p.y);
  //     ymax = max(ymax,p.y);
  //    Logger::debug("(%d, %d) - Xmin=%d\tXmax=%d\tYmin=%d\tYmax=%d\t",p.x,p.y,xmin,xmax,ymin,ymax);
  //    // delay(50);
  //    //TODO: start up the PID controller, and give it something to do.
  // }
  if(p.x > TS_MIN_X
    && p.x < TS_MAX_X
    && p.y > TS_MIN_Y
    && p.y < TS_MAX_Y) {
      int x = p.x;
      int y = p.y-(TS_SLOPE_Y*(TS_MAX_X-p.x));
    Serial.printf("%d\t%d\t%d\n",i++,x,y);
  }
  delay(10);
}

#endif
