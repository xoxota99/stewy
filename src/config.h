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

#define ENABLE_SERIAL_COMMANDS    //Comment out, to omit Command shell code.
#define ENABLE_NUNCHUCK           //Comment out, to omit Nunchuck code.
#define ENABLE_TOUCHSCREEN        //Comment out, to omit Touchscreen code.

#define LOG_LEVEL       LogLevel.DEBUG

//Which servos are reversed. 1 = reversed, 0 = normal.
const int SERVO_REVERSE[6] = {0, 1, 0, 1, 0, 1};

const int SERVO_MIN_ANGLE = 0;
const int SERVO_MAX_ANGLE = 360;
const int SERVO_MID_ANGLE = SERVO_MIN_ANGLE + (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) / 2;
const int SERVO_MIN_US = 755;
const int SERVO_MAX_US = 2250;
//const int SERVO_MID_US=1500;
const int SERVO_MAX_DELAY = 500;  //milliseconds to transit from min to max. for HS-5625MG, this is 0.14 sec/60deg, or probably about 420ms. 500 to be safe.

const int SERVO_TRIM[] = {  //trim values, in microseconds, AFTER reversing
  0,
  20,
  0,
  135,
  0,
  120
};

#endif    //__STU_CONFIG_H__
