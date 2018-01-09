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

/*
* Shamelessly stolen from the work of Daniel Waters, https://www.youtube.com/watch?v=1jrP3_1ML9M
*/

#include <math.h>

#ifndef degrees
#define degrees(a) (a * 180.0 / M_PI)
#endif

#ifndef radians
#define radians(a) (a * M_PI / 180.0)
#endif

#ifndef CONFIG_H
#define CONFIG_H

//Taken from Daniel Waters, https://www.youtube.com/watch?v=1jrP3_1ML9M
//Angles are defined sort of relative to a polar coordinate system. Weird, but it works.

//macros
#define THETA_P_DEG     15      //Platform joint angle (degrees) offset from AXIS[1|2|3]. A value of zero puts these joints directly on the axes
#define THETA_B_DEG     10      //Base Servo pinion angle (degrees) offset from AXIS[1|2|3]. A value of zero puts the servo pinion directly on the axes
#define P_RAD           40      //Platform radius (mm). The distance from the center of the platform to the center of one platform / pushrod "joint". This should be the same for all six pushrods.
#define B_RAD           70      //Base radius (mm). Distance from the center of the base plate to the center of one servo pinion gear. Again, this should be the same for all six servos.
#define ARM_LENGTH      25      //Servo arm length (mm). Distance from the center of the servo pivot to the center of the pushrod pivot on the servo arm.
#define ROD_LENGTH      140     //Push rod length (mm). Distance between pushrod ball joints (servo to platform).
#define Z_HOME          135     //Default Z height of the platform (above the base), with servo arms horizontal. Formally, the distance from the plane described by the collection of servo pinion gear centers, to the plane described by the collection of platform / pushrod joints.

//constants

/*
  There are three axes of symmetry (AXIS1, AXIS2, AXIS3). Looking down on the
  platform from above (along the Y axis), with 0 degrees being the X-positive line, and traveling
  in a CC direction, these are at 30 degrees, 120 degrees, and 240 degrees. All
  the polar coordinates of pivot points, servo centers, etc. are calculated based on
  an axis, and an offset angle (positive or negative theta) from the axis.
*/
const double AXIS1 =      M_PI / 6;  //30 degrees.
const double AXIS2 =      -M_PI / 2; //-90 degrees.
//AXIS 3 is just a reflected AXIS1, so we account for it in calculations.

/*
 * Absolute angle that the servo arm plane of rotation is at (degrees).
 */
const double THETA_S_DEG[6] = {
  180,
  -120,
  60,
  120,
  -60,
  0
};

//Which servos are reversed. 1 = normal, -1 = reversed.
const int SERVO_REVERSE[6] = {1,1,1,1,1,1};

#endif
