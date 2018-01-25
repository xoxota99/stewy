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


#ifndef __STU_CONFIG_H__
#define __STU_CONFIG_H__

#define ENABLE_SERIAL_COMMANDS    //Comment out, to omit Command shell code.
#define ENABLE_NUNCHUCK           //Comment out, to omit Nunchuck code.
#define ENABLE_TOUCHSCREEN        //Comment out, to omit Touchscreen code.

/*
  Comment out, to disable Servos. Servos can get hot if you don't set
  them to a "safe" value. Commenting this line out allows to test things that
  are not servo-related, without killing the battery, or overheating the servos.
*/
#define ENABLE_SERVOS

#define LOG_LEVEL       Logger::DEBUG

//Which servos are reversed. 1 = reversed, 0 = normal.
const int SERVO_REVERSE[6] = {0, 1, 0, 1, 0, 1};

const int SERVO_MIN_ANGLE = 0;
const int SERVO_MAX_ANGLE = 360;
const int SERVO_MID_ANGLE = SERVO_MIN_ANGLE + (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) / 2;
const int SERVO_MIN_US = 755;
const int SERVO_MAX_US = 2250;
const int SERVO_MID_US = SERVO_MIN_US + (SERVO_MAX_US - SERVO_MIN_US) / 2;
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

const int SERVO_PINS[] = {  //pin numbers for each servo signal.
  0,
  1,
  2,
  3,
  4,
  5
};

#ifdef ENABLE_NUNCHUCK
//Different "modes" for the platform.
enum Mode {
  SETPOINT,   // setpoint position is in the middle of the platform.
  CONTROL,    // X/Y position of the wiichuck directly controls the position of the platform.
  CIRCLE,     // setpoint position moves in a circle. Direction (CW / CCW) is controlled by the "direction" field.
  EIGHT,      // setpoint position moves in a figure eight. Direction (CW / CCW) is controlled by the "direction" field
  SQUARE      // setpoint cycles through corners of a square (manually controlled by the Z button).
};

char* modeStrings[] = {
  "SETPOINT",
  "CONTROL",
  "CIRCLE",
  "EIGHT",
  "SQUARE"
};

enum Direction {
  CW,
  CCW
};

char* directionStrings[] = {
  "CW",
  "CCW"
};


Mode mode = MIDDLE;
Direction dir = CW;
#endif    //ENABLE_NUNCHUCK

#endif    //__STU_CONFIG_H__
