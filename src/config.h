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

#pragma once
#ifndef __STU_CONFIG_H__
#define __STU_CONFIG_H__

#include <math.h>

#define ENABLE_SERIAL_COMMANDS    //Comment out, to omit Command shell code.
#define ENABLE_NUNCHUCK           //Comment out, to omit Nunchuck code.
#define ENABLE_TOUCHSCREEN        //Comment out, to omit Touchscreen code.

//macros
#define THETA_P_DEG     45.25     //Platform joint angle (degrees) offset from AXIS[1|2|3]. A value of zero puts these joints directly on the axes
#define THETA_B_DEG     24.5      //Base Servo pinion angle (degrees) offset from AXIS[1|2|3]. A value of zero puts the servo pinion directly on the axes
#define P_RAD           50        //Platform radius (mm). The distance from the center of the platform to the center of one platform / pushrod "joint". This should be the same for all six pushrods.
#define B_RAD           80.2      //Base radius (mm). Distance from the center of the base plate to the center of one servo pinion gear. Again, this should be the same for all six servos.
#define ARM_LENGTH      25        //Servo arm length (mm). Distance from the center of the servo pivot to the center of the pushrod pivot on the servo arm.
#define ROD_LENGTH      155       //Push rod length (mm). Distance between pushrod ball joints (servo to platform).
#define Z_HOME          148       //Default Z height of the platform (above the base), with servo arms horizontal. Formally, the distance from the plane described by the collection of servo pinion gear centers, to the plane described by the collection of platform / pushrod joints.

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
   Absolute angle that the servo arm plane of rotation is at (degrees), from the world-X axis.
*/
const double THETA_S_DEG[6] = {
  -60,
  120,
  180,
  0,
  60,
  -120
};

//Which servos are reversed. 1 = reversed, 0 = normal.
const int SERVO_REVERSE[6] = {0, 1, 0, 1, 0, 1};

const int SERVO_MIN_ANGLE = 0;
const int SERVO_MAX_ANGLE = 360;
const int SERVO_MID_ANGLE = SERVO_MIN_ANGLE + (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) / 2;
const int SERVO_MIN_US = 755;
const int SERVO_MAX_US = 2250;
//const int SERVO_MID_US=1500;
const int SERVO_MAX_DELAY = 500;  //milliseconds to transit from min to max. for HS-5625MG, this is 0.14 sec/60deg, or probably about 420ms. 500 to be safe.

const int SERVO_TRIM[] = {  //trim values, AFTER reversing
  0,
  20,
  0,
  135,
  0,
  120
};

#endif    //__STU_CONFIG_H__
