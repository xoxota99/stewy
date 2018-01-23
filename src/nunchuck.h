#ifndef __STU_NUNCHUCK_H__
#define __STU_NUNCHUCK_H__
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

#ifdef ENABLE_NUNCHUCK
#include <Nunchuk.h> //from https://github.com/hughpyle/machinesalem-arduino-libs/tree/master/nunchuk

#pragma GCC diagnostic ignored "-Wwrite-strings"

Nunchuk nc;

//Different "modes" for the platform.
enum Mode {
  MIDDLE,   // setpoint position is in the middle of the platform.
  ANGLE,    // X/Y position of the wiichuck directly controls the roll/pitch of the platform.
  CIRCLE,   // setpoint position moves in a circle. Direction (CW / CCW) is controlled by the "direction" field.
  EIGHT,    // setpoint position moves in a figure eight. Direction (CW / CCW) is controlled by the "direction" field
  SQUARE,   // setpoint cycles through corners of a square (manually controlled by the Z button).
  SPIRAL    // setpoint position moves outward from the middle in a spiral, then back in, then back out, etc. Direction (CW / CCW) is controlled by the "direction" field.
};

char* modeStrings[] = {
  "MIDDLE",
  "ANGLE",
  "CIRCLE",
  "EIGHT",
  "SQUARE",
  "SPIRAL"
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

bool zDown, cDown;

void processNunchuck();

void onCButtonDown();
void onCButtonUp();

void onZButtonDown();
void onZButtonUp();

#endif
#endif    //__STU_NUNCHUCK_H__
