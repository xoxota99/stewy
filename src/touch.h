#pragma once
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

#include "config.h"

#ifdef ENABLE_TOUCHSCREEN
#include <TouchScreen.h> //from https://github.com/adafruit/Touch-Screen-Library
#include <PID_v1.h>      //https://github.com/br3ttb/Arduino-PID-Library
#include <EEPROM.h>      //for storing calibration data

// Touchscreen filter class for smoothing input
class TouchFilter
{
private:
  float xValues[TOUCH_FILTER_SAMPLES];
  float yValues[TOUCH_FILTER_SAMPLES];
  int currentIndex;
  bool filled;

public:
  TouchFilter();
  void addSample(float x, float y);
  void reset();
  float getFilteredX();
  float getFilteredY();
};

// Touchscreen calibration structure
struct TouchCalibration
{
  int minX;
  int maxX;
  int minY;
  int maxY;
  bool isCalibrated;
};

extern PID rollPID;
extern PID pitchPID;
extern TouchScreen ts;
extern TouchFilter touchFilter;
extern TouchCalibration touchCalibration;

// Function declarations
void processTouchscreen();
void initTouchscreen();
bool loadCalibration();
bool saveCalibration();
void startCalibration();
void processCalibrationPoint(int step, TSPoint p);
void finishCalibration();

#endif // ENABLE_TOUCHSCREEN
