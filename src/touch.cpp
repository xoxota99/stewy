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

// Initialize global variables
PID rollPID(&inputX, &outputX, &setpointX, PX, IX, DX, P_ON_E, DIRECT);
PID pitchPID(&inputY, &outputY, &setpointY, PY, IY, DY, P_ON_E, DIRECT);
TouchScreen ts(XP, YP, XM, YM, TS_OHMS);
TouchFilter touchFilter;
TouchCalibration touchCalibration;

// Calibration state variables
bool isCalibrating = false;
int calibrationStep = 0;
unsigned long calibrationStartTime = 0;
int calibrationSamples[CALIBRATION_POINTS][2][CALIBRATION_SAMPLES]; // [point][x/y][sample]
int calibrationSampleCount = 0;

// TouchFilter implementation
TouchFilter::TouchFilter()
{
  reset();
}

void TouchFilter::reset()
{
  for (int i = 0; i < TOUCH_FILTER_SAMPLES; i++)
  {
    xValues[i] = 0;
    yValues[i] = 0;
  }
  currentIndex = 0;
  filled = false;
}

void TouchFilter::addSample(float x, float y)
{
  // Add new sample
  xValues[currentIndex] = x;
  yValues[currentIndex] = y;

  // Update index
  currentIndex = (currentIndex + 1) % TOUCH_FILTER_SAMPLES;
  if (currentIndex == 0)
  {
    filled = true;
  }
}

float TouchFilter::getFilteredX()
{
  if (!filled && currentIndex == 0)
    return 0;

  float sum = 0;
  int count = filled ? TOUCH_FILTER_SAMPLES : currentIndex;

  for (int i = 0; i < count; i++)
  {
    sum += xValues[i];
  }

  return sum / count;
}

float TouchFilter::getFilteredY()
{
  if (!filled && currentIndex == 0)
    return 0;

  float sum = 0;
  int count = filled ? TOUCH_FILTER_SAMPLES : currentIndex;

  for (int i = 0; i < count; i++)
  {
    sum += yValues[i];
  }

  return sum / count;
}

// Initialize touchscreen and load calibration if available
void initTouchscreen()
{
  // Set up PID controllers
  rollPID.SetOutputLimits(ROLL_PID_LIMIT_MIN, ROLL_PID_LIMIT_MAX);
  pitchPID.SetOutputLimits(PITCH_PID_LIMIT_MIN, PITCH_PID_LIMIT_MAX);
  rollPID.SetSampleTime(ROLL_PID_SAMPLE_TIME);
  pitchPID.SetSampleTime(PITCH_PID_SAMPLE_TIME);
  rollPID.SetMode(AUTOMATIC);
  pitchPID.SetMode(AUTOMATIC);

  // Try to load calibration data
  if (!loadCalibration())
  {
    // If no calibration data, use defaults
    touchCalibration.minX = TS_DEFAULT_MIN_X;
    touchCalibration.maxX = TS_DEFAULT_MAX_X;
    touchCalibration.minY = TS_DEFAULT_MIN_Y;
    touchCalibration.maxY = TS_DEFAULT_MAX_Y;
    touchCalibration.isCalibrated = false;

    Log.warning("No touchscreen calibration data found. Using defaults.");
  }

  // Initialize setpoints to center of screen
  setpointX = touchCalibration.minX + (touchCalibration.maxX - touchCalibration.minX) / 2;
  setpointY = touchCalibration.minY + (touchCalibration.maxY - touchCalibration.minY) / 2;

  Log.notice("Touchscreen initialized. Calibrated: %s", touchCalibration.isCalibrated ? "Yes" : "No");
}

// Load calibration data from EEPROM
bool loadCalibration()
{
  // Read calibration data from EEPROM
  EEPROM.get(TOUCH_CALIBRATION_ADDR, touchCalibration);

  // Validate calibration data
  if (touchCalibration.isCalibrated &&
      touchCalibration.minX >= 0 && touchCalibration.minX < touchCalibration.maxX && touchCalibration.maxX <= 1023 &&
      touchCalibration.minY >= 0 && touchCalibration.minY < touchCalibration.maxY && touchCalibration.maxY <= 1023)
  {
    Log.notice("Loaded touchscreen calibration: X[%d-%d], Y[%d-%d]",
               touchCalibration.minX, touchCalibration.maxX,
               touchCalibration.minY, touchCalibration.maxY);
    return true;
  }

  return false;
}

// Save calibration data to EEPROM
bool saveCalibration()
{
  EEPROM.put(TOUCH_CALIBRATION_ADDR, touchCalibration);
  Log.notice("Saved touchscreen calibration: X[%d-%d], Y[%d-%d]",
             touchCalibration.minX, touchCalibration.maxX,
             touchCalibration.minY, touchCalibration.maxY);
  return true;
}

// Start the calibration process
void startCalibration()
{
  Log.notice("Starting touchscreen calibration...");
  isCalibrating = true;
  calibrationStep = 0;
  calibrationSampleCount = 0;
  calibrationStartTime = millis();

  // Disable PID during calibration
  rollPID.SetMode(MANUAL);
  pitchPID.SetMode(MANUAL);

  // Reset the platform to home position
  stu.home(sp_servo);

  Log.notice("Place ball in top-left corner and wait...");
}

// Process a calibration point
void processCalibrationPoint(int step, TSPoint p)
{
  // Store the sample
  if (p.z > 0 && calibrationSampleCount < CALIBRATION_SAMPLES)
  {
    calibrationSamples[step][0][calibrationSampleCount] = p.x;
    calibrationSamples[step][1][calibrationSampleCount] = p.y;
    calibrationSampleCount++;

    if (calibrationSampleCount == CALIBRATION_SAMPLES)
    {
      // Calculate average for this point
      int sumX = 0, sumY = 0;
      for (int i = 0; i < CALIBRATION_SAMPLES; i++)
      {
        sumX += calibrationSamples[step][0][i];
        sumY += calibrationSamples[step][1][i];
      }

      int avgX = sumX / CALIBRATION_SAMPLES;
      int avgY = sumY / CALIBRATION_SAMPLES;

      Log.notice("Calibration point %d: (%d, %d)", step + 1, avgX, avgY);

      // Move to next step
      calibrationStep++;
      calibrationSampleCount = 0;
      calibrationStartTime = millis();

      if (calibrationStep < CALIBRATION_POINTS)
      {
        // Instructions for next point
        switch (calibrationStep)
        {
        case 1:
          Log.notice("Place ball in top-right corner and wait...");
          break;
        case 2:
          Log.notice("Place ball in bottom-right corner and wait...");
          break;
        case 3:
          Log.notice("Place ball in bottom-left corner and wait...");
          break;
        }
      }
      else
      {
        // Calibration complete
        finishCalibration();
      }
    }
  }
}

// Finish the calibration process
void finishCalibration()
{
  // Calculate min/max values from the calibration points
  int minX = 1023, maxX = 0, minY = 1023, maxY = 0;

  for (int i = 0; i < CALIBRATION_POINTS; i++)
  {
    int avgX = 0, avgY = 0;

    for (int j = 0; j < CALIBRATION_SAMPLES; j++)
    {
      avgX += calibrationSamples[i][0][j];
      avgY += calibrationSamples[i][1][j];
    }

    avgX /= CALIBRATION_SAMPLES;
    avgY /= CALIBRATION_SAMPLES;

    minX = min(minX, avgX);
    maxX = max(maxX, avgX);
    minY = min(minY, avgY);
    maxY = max(maxY, avgY);
  }

  // Add a small margin
  minX = max(0, minX - 10);
  maxX = min(1023, maxX + 10);
  minY = max(0, minY - 10);
  maxY = min(1023, maxY + 10);

  // Save calibration
  touchCalibration.minX = minX;
  touchCalibration.maxX = maxX;
  touchCalibration.minY = minY;
  touchCalibration.maxY = maxY;
  touchCalibration.isCalibrated = true;

  saveCalibration();

  // Reset setpoints to center of screen
  setpointX = touchCalibration.minX + (touchCalibration.maxX - touchCalibration.minX) / 2;
  setpointY = touchCalibration.minY + (touchCalibration.maxY - touchCalibration.minY) / 2;

  // Re-enable PID
  rollPID.SetMode(AUTOMATIC);
  pitchPID.SetMode(AUTOMATIC);

  isCalibrating = false;
  Log.notice("Touchscreen calibration complete!");
}

void processTouchscreen()
{
  static unsigned long ballLastSeen;
  static float lastInputX = 0, lastInputY = 0;

  // Get a point from the touchscreen
  TSPoint p = ts.getPoint();

  // Handle calibration if in progress
  if (isCalibrating)
  {
    processCalibrationPoint(calibrationStep, p);
    return;
  }

  // we should NOT process the touchscreen output if the PID is in MANUAL mode.
  if (pitchPID.GetMode() != MANUAL && rollPID.GetMode() != MANUAL)
  {
    // Apply deadzone filter to avoid jitter
    if (abs(p.x - lastInputX) < TOUCH_DEADZONE)
    {
      p.x = lastInputX;
    }

    if (abs(p.y - lastInputY) < TOUCH_DEADZONE)
    {
      p.y = lastInputY;
    }

    // Add to filter
    if (p.z > 0)
    { // Only add valid points (when pressure is detected)
      touchFilter.addSample(p.x, p.y);
      lastInputX = p.x;
      lastInputY = p.y;
    }

    // Get filtered values
    inputX = touchFilter.getFilteredX();
    inputY = touchFilter.getFilteredY();

    // Check if the ball is within the calibrated area
    if (p.z > 0 &&
        inputX >= touchCalibration.minX && inputX <= touchCalibration.maxX &&
        inputY >= touchCalibration.minY && inputY <= touchCalibration.maxY)
    {

      ballLastSeen = millis();

      // setpoint may have changed. setpoint is on a scale of -1.0 to +1.0, in both axes.
      int width = touchCalibration.maxX - touchCalibration.minX;
      int height = touchCalibration.maxY - touchCalibration.minY;

      setpointX = map(setpoint.x, -1.0, 1.0, touchCalibration.minX, touchCalibration.maxX);
      setpointY = map(setpoint.y, -1.0, 1.0, touchCalibration.minY, touchCalibration.maxY);

      double newOutX = rollPID.Compute(inputX, setpointX);
      double newOutY = pitchPID.Compute(inputY, setpointY);

      if (newOutX != outputX || newOutY != outputY)
      {
        outputX = newOutX;
        outputY = newOutY;

        Log.trace("TOUCH: %d\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f",
                  rollPID.GetMode(), millis(), inputX, inputY,
                  setpoint.x, setpoint.y, setpointX, setpointY, outputX, outputY);

        float roll = map(outputX, ROLL_PID_LIMIT_MIN, ROLL_PID_LIMIT_MAX, MIN_ROLL, MAX_ROLL);
        float pitch = map(outputY, PITCH_PID_LIMIT_MIN, PITCH_PID_LIMIT_MAX, MIN_PITCH, MAX_PITCH);

        unsigned long m = millis();

        Log.trace("Time/InX/InY/OutX/OutY/roll/pitch:\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f",
                  m, inputX, inputY, outputX, outputY, roll, pitch);

        stu.moveTo(sp_servo, pitch, roll);
      }
    }
    else
    {
      // The ball has disappeared. Start a countdown.
      unsigned long m = millis();
      if (m - ballLastSeen >= LOST_BALL_TIMEOUT)
      {
        // start a countdown. Until you hit zero, do nothing. This is to avoid sudden
        // movement to home, while the ball is near the edge of the platform, and
        // possibly still recoverable.
        stu.home(sp_servo);
      }
    }
  }
}

#endif
