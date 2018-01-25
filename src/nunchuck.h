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
#include "config.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

/*
  Specifies the maximum time between button clicks that are interpreted as a
  double-click. If the time between clicks exceeds this value, the clicks are
  interpreted as single clicks.
*/
#define DBLCLICK_THRESHOLD_MS  500

Nunchuk nc;

struct _chuck_data_entry {
  bool buttonC;
  bool buttonZ;
  int joyX;
  int joyY;
  int accelX;
  int accelY;
  int accelZ;
  float tiltX;
  float tiltY;
  float tiltZ;
  long lastCButtonDown;         //to help with "double-clicking"
  long lastZButtonDown;
};

typedef struct _chuck_data_entry ChuckData;

ChuckData chuckData;

//calibration / trim data.
ChuckData chuckTrim = {
  false,    //Not used.
  false,    //Not used.
  1,
  -2,
  29,
  48,
  -210,
  0.0F,    //Not used.
  0.0F,    //Not used.
  0.0F    //Not used.
};

// bool zDown, cDown;

void processNunchuck();

void onCButtonDown();
void onCButtonUp();

void onZButtonDown();
void onZButtonUp();

void onZButtonDblClick();
void onCButtonDblClick();

#endif
#endif    //__STU_NUNCHUCK_H__
