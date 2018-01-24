
#ifndef __STU_TOUCH_H__
#define __STU_TOUCH_H__
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

#ifdef ENABLE_TOUCHSCREEN
#include <TouchScreen.h> //from https://github.com/adafruit/Touch-Screen-Library

#define XP A7  // YELLOW / XRT. can be a digital pin.
#define XM A6  // WHITE / XLE. must be an analog pin, use "An" notation!

#define YP A9  // BLACK / YUP. must be an analog pin, use "An" notation!
#define YM A8  // RED / YLO. can be a digital pin.

#define TS_OHMS 711 //resistance between X+ and X-


//The Adafruit touchscreen library returns raw values from the ADC (between 0-1024).
//Here, we adjust for our specific touchscreen part.

//X and Y values are not completely independent. This defines the linear
//fall-off rate of the Y value, in terms of a slope constant.
#define TS_SLOPE_Y            -0.325

//Min / max values of X and Y.
#define TS_MIN_X              5         //this is just to protect against the "no-touch" X value of zero.
#define TS_MAX_X              800
#define TS_MIN_Y              100
#define TS_MAX_Y              800

void processTouchscreen();

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 711 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, TS_OHMS);

#endif  //ENABLE_TOUCHSCREEN
#endif  //__STU_TOUCH_H__
