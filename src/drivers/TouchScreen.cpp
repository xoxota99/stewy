/**
 * @file TouchScreen.cpp
 * @brief Implementation of the touchscreen driver
 *
 * @author Philippe Desrosiers
 * @copyright Copyright (C) 2018 Philippe Desrosiers
 * @license GPL-3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "drivers/TouchScreen.h"
#include "core/Platform.h"

namespace stewy
{
  namespace drivers
  {

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

    // TouchScreenDriver implementation
    TouchScreenDriver::TouchScreenDriver(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym, uint16_t ohms)
    {
      ts = new TouchScreen(xp, yp, xm, ym, ohms);

      // Initialize variables to safe defaults
      inputX = 0.0;
      inputY = 0.0;
      outputX = 0.0;
      outputY = 0.0;
      setpointX = 0.0;
      setpointY = 0.0;

      // Initialize PID controllers with pointers to our variables
      rollPID = new PID(&inputX, &outputX, &setpointX, 3, 0, 0, P_ON_E, DIRECT);
      pitchPID = new PID(&inputY, &outputY, &setpointY, 1, 0, 0, P_ON_E, DIRECT);

      isCalibrating = false;
      calibrationStep = 0;
      calibrationSampleCount = 0;
      ballLastSeen = 0;
    }

    TouchScreenDriver::~TouchScreenDriver()
    {
      // Clean up dynamically allocated objects
      if (ts != nullptr)
      {
        delete ts;
        ts = nullptr;
      }

      if (rollPID != nullptr)
      {
        delete rollPID;
        rollPID = nullptr;
      }

      if (pitchPID != nullptr)
      {
        delete pitchPID;
        pitchPID = nullptr;
      }

      Log.trace("TouchScreenDriver destroyed");
    }

    void TouchScreenDriver::init()
    {
      // Set up PID controllers with appropriate limits and sample times
      rollPID->SetOutputLimits(ROLL_PID_LIMIT_MIN, ROLL_PID_LIMIT_MAX);
      pitchPID->SetOutputLimits(PITCH_PID_LIMIT_MIN, PITCH_PID_LIMIT_MAX);
      rollPID->SetSampleTime(ROLL_PID_SAMPLE_TIME);
      pitchPID->SetSampleTime(PITCH_PID_SAMPLE_TIME);

      // Start with PID controllers in automatic mode
      rollPID->SetMode(AUTOMATIC);
      pitchPID->SetMode(AUTOMATIC);

      // Try to load calibration data
      if (!loadCalibration())
      {
        // If no calibration data, use defaults
        calibration.minX = TS_DEFAULT_MIN_X;
        calibration.maxX = TS_DEFAULT_MAX_X;
        calibration.minY = TS_DEFAULT_MIN_Y;
        calibration.maxY = TS_DEFAULT_MAX_Y;
        calibration.isCalibrated = false;

        Log.warning("No touchscreen calibration data found. Using defaults.");
      }

      // Initialize setpoints to center of screen
      setpointX = calibration.minX + (calibration.maxX - calibration.minX) / 2;
      setpointY = calibration.minY + (calibration.maxY - calibration.minY) / 2;

      Log.info("Touchscreen initialized. Calibrated: %s", calibration.isCalibrated ? "Yes" : "No");
    }

    bool TouchScreenDriver::loadCalibration()
    {
      // Read calibration data from EEPROM
      EEPROM.get(TOUCH_CALIBRATION_ADDR, calibration);

      // Validate calibration data
      if (calibration.isCalibrated &&
          calibration.minX >= 0 && calibration.minX < calibration.maxX && calibration.maxX <= 1023 &&
          calibration.minY >= 0 && calibration.minY < calibration.maxY && calibration.maxY <= 1023)
      {
        Log.info("Loaded touchscreen calibration: X[%d-%d], Y[%d-%d]",
                 calibration.minX, calibration.maxX,
                 calibration.minY, calibration.maxY);
        return true;
      }

      return false;
    }

    bool TouchScreenDriver::saveCalibration()
    {
      EEPROM.put(TOUCH_CALIBRATION_ADDR, calibration);
      Log.info("Saved touchscreen calibration: X[%d-%d], Y[%d-%d]",
               calibration.minX, calibration.maxX,
               calibration.minY, calibration.maxY);
      return true;
    }

    void TouchScreenDriver::startCalibration()
    {
      Log.info("Starting touchscreen calibration...");
      isCalibrating = true;
      calibrationStep = 0;
      calibrationSampleCount = 0;
      calibrationStartTime = millis();

      // Disable PID during calibration
      rollPID->SetMode(MANUAL);
      pitchPID->SetMode(MANUAL);

      // Reset the platform to home position
      // Note: This would normally call platform.home(sp_servo), but we're using a different approach

      Log.info("Place ball in top-left corner and wait...");
    }

    bool TouchScreenDriver::isCalibrationInProgress()
    {
      return isCalibrating;
    }

    void TouchScreenDriver::processCalibrationPoint(int step, TSPoint p)
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

          Log.info("Calibration point %d: (%d, %d)", step + 1, avgX, avgY);

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
              Log.info("Place ball in top-right corner and wait...");
              break;
            case 2:
              Log.info("Place ball in bottom-right corner and wait...");
              break;
            case 3:
              Log.info("Place ball in bottom-left corner and wait...");
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

    void TouchScreenDriver::finishCalibration()
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
      calibration.minX = minX;
      calibration.maxX = maxX;
      calibration.minY = minY;
      calibration.maxY = maxY;
      calibration.isCalibrated = true;

      saveCalibration();

      // Reset setpoints to center of screen
      setpointX = calibration.minX + (calibration.maxX - calibration.minX) / 2;
      setpointY = calibration.minY + (calibration.maxY - calibration.minY) / 2;

      // Re-enable PID
      rollPID->SetMode(AUTOMATIC);
      pitchPID->SetMode(AUTOMATIC);

      isCalibrating = false;
      Log.info("Touchscreen calibration complete!");
    }

    void TouchScreenDriver::process(float setpoint_x, float setpoint_y, float *servoValues)
    {
      static float lastInputX = 0, lastInputY = 0;

      // Get a point from the touchscreen
      TSPoint p = ts->getPoint();

      // Handle calibration if in progress
      if (isCalibrating)
      {
        processCalibrationPoint(calibrationStep, p);
        return;
      }

      // we should NOT process the touchscreen output if the PID is in MANUAL mode.
      if (pitchPID->GetMode() != MANUAL && rollPID->GetMode() != MANUAL)
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
          filter.addSample(p.x, p.y);
          lastInputX = p.x;
          lastInputY = p.y;
        }

        // Get filtered values
        inputX = filter.getFilteredX();
        inputY = filter.getFilteredY();

        // Check if the ball is within the calibrated area
        if (p.z > 0 &&
            inputX >= calibration.minX && inputX <= calibration.maxX &&
            inputY >= calibration.minY && inputY <= calibration.maxY)
        {

          ballLastSeen = millis();

          // setpoint may have changed. setpoint is on a scale of -1.0 to +1.0, in both axes.
          int width = calibration.maxX - calibration.minX;
          int height = calibration.maxY - calibration.minY;

          // Map the normalized setpoint (-1.0 to 1.0) to screen coordinates
          double newSetpointX = map(setpoint_x, -1.0, 1.0, calibration.minX, calibration.maxX);
          double newSetpointY = map(setpoint_y, -1.0, 1.0, calibration.minY, calibration.maxY);

          // Only update setpoints if they've changed significantly
          if (abs(newSetpointX - setpointX) > 0.5 || abs(newSetpointY - setpointY) > 0.5)
          {
            setpointX = newSetpointX;
            setpointY = newSetpointY;
            Log.trace("Setpoint updated to: %.2f, %.2f", setpointX, setpointY);
          }

          // Compute PID values - the PID library automatically uses the input, output, and setpoint
          // values that were provided during construction
          bool computedX = rollPID->Compute();
          bool computedY = pitchPID->Compute();

          // Only update platform position if PID values have changed
          if (computedX || computedY)
          {
            Log.trace("TOUCH: %d\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f",
                      rollPID->GetMode(), millis(), inputX, inputY,
                      setpoint_x, setpoint_y, setpointX, setpointY, outputX, outputY);

            float roll = map(outputX, ROLL_PID_LIMIT_MIN, ROLL_PID_LIMIT_MAX, MIN_ROLL, MAX_ROLL);
            float pitch = map(outputY, PITCH_PID_LIMIT_MIN, PITCH_PID_LIMIT_MAX, MIN_PITCH, MAX_PITCH);

            unsigned long m = millis();

            Log.trace("Time/InX/InY/OutX/OutY/roll/pitch:\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f",
                      m, inputX, inputY, outputX, outputY, roll, pitch);

            // Move the platform
            core::Platform platform(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
            platform.moveTo(servoValues, pitch, roll);
          }
        }
        else
        {
          // The ball has disappeared. Start a countdown.
          unsigned long m = millis();
          if (m - ballLastSeen >= LOST_BALL_TIMEOUT)
          {
            // Return to home position
            core::Platform platform(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
            platform.home(servoValues);
          }
        }
      }
    }

    void TouchScreenDriver::setPID(char axis, double p, double i, double d)
    {
      // Validate PID parameters to prevent unstable behavior
      if (p < 0 || i < 0 || d < 0)
      {
        Log.error("Invalid PID parameters: P=%.2f, I=%.2f, D=%.2f. All values must be non-negative.", p, i, d);
        return;
      }

      // Apply reasonable upper limits to prevent extreme behavior
      p = constrain(p, 0.0, 100.0);
      i = constrain(i, 0.0, 10.0);
      d = constrain(d, 0.0, 10.0);

      if (axis == 'x' || axis == 'X')
      {
        rollPID->SetTunings(p, i, d);
        Log.info("Roll PID parameters set to: P=%.2f, I=%.2f, D=%.2f", p, i, d);
      }
      else if (axis == 'y' || axis == 'Y')
      {
        pitchPID->SetTunings(p, i, d);
        Log.info("Pitch PID parameters set to: P=%.2f, I=%.2f, D=%.2f", p, i, d);
      }
    }

    void TouchScreenDriver::getPID(char axis, double &p, double &i, double &d)
    {
      PID *_pid = nullptr; // Initialize to nullptr to avoid the warning

      if (axis == 'x' || axis == 'X')
      {
        _pid = rollPID;
      }
      else if (axis == 'y' || axis == 'Y')
      {
        _pid = pitchPID;
      }

      if (_pid)
      {
        p = _pid->GetKp();
        i = _pid->GetKi();
        d = _pid->GetKd();
      }
      else
      {
        // Default values if axis is invalid
        p = 0.0;
        i = 0.0;
        d = 0.0;
      }
    }

    void TouchScreenDriver::resetPID()
    {
      // Reset to default PID values
      rollPID->SetTunings(3.0, 0.0, 0.0);
      pitchPID->SetTunings(1.0, 0.0, 0.0);

      // Reset output limits
      rollPID->SetOutputLimits(ROLL_PID_LIMIT_MIN, ROLL_PID_LIMIT_MAX);
      pitchPID->SetOutputLimits(PITCH_PID_LIMIT_MIN, PITCH_PID_LIMIT_MAX);

      // Reset sample times
      rollPID->SetSampleTime(ROLL_PID_SAMPLE_TIME);
      pitchPID->SetSampleTime(PITCH_PID_SAMPLE_TIME);

      // Reset to automatic mode
      rollPID->SetMode(AUTOMATIC);
      pitchPID->SetMode(AUTOMATIC);

      // Reset internal variables
      outputX = 0.0;
      outputY = 0.0;

      Log.info("PID controllers reset to default values");
    }

  } // namespace drivers
} // namespace stewy
