#pragma once
/**
 * @file TouchScreen.h
 * @brief Touchscreen driver and filtering
 *
 * This file contains the touchscreen driver, filtering, and calibration
 * functionality for the Stewart platform.
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

#include <TouchScreen.h> // from https://github.com/adafruit/Touch-Screen-Library
#include <PID_v1.h>      // https://github.com/br3ttb/Arduino-PID-Library
#include <EEPROM.h>      // for storing calibration data
#include "core/Config.h"

namespace stewy
{
  namespace drivers
  {

    /**
     * @class TouchFilter
     * @brief Filter for touchscreen input
     *
     * This class provides filtering for touchscreen input to reduce noise
     * and improve stability. It implements a simple moving average filter
     * that stores multiple samples and returns the average value.
     */
    class TouchFilter
    {
    private:
      float xValues[TOUCH_FILTER_SAMPLES]; ///< Array of X coordinate samples
      float yValues[TOUCH_FILTER_SAMPLES]; ///< Array of Y coordinate samples
      int currentIndex;                    ///< Current index in the sample arrays
      bool filled;                         ///< Flag indicating if the arrays are filled

    public:
      /**
       * @brief Construct a new TouchFilter object
       *
       * Initializes the filter with all samples set to zero.
       */
      TouchFilter();

      /**
       * @brief Add a new sample to the filter
       *
       * Adds a new X,Y coordinate sample to the filter's sample arrays.
       * Once the arrays are filled, new samples replace the oldest ones.
       *
       * @param x X coordinate value to add
       * @param y Y coordinate value to add
       */
      void addSample(float x, float y);

      /**
       * @brief Reset the filter
       *
       * Clears all samples and resets the filter state.
       */
      void reset();

      /**
       * @brief Get filtered X coordinate
       *
       * Calculates and returns the average X coordinate from all samples.
       *
       * @return Average X coordinate value, or 0 if no samples have been added
       */
      float getFilteredX();

      /**
       * @brief Get filtered Y coordinate
       *
       * Calculates and returns the average Y coordinate from all samples.
       *
       * @return Average Y coordinate value, or 0 if no samples have been added
       */
      float getFilteredY();
    };

    /**
     * @struct TouchCalibration
     * @brief Touchscreen calibration data
     *
     * This structure stores calibration data for the touchscreen,
     * including the minimum and maximum X and Y coordinates that
     * define the usable area of the touchscreen.
     */
    struct TouchCalibration
    {
      int minX;          ///< Minimum X coordinate value
      int maxX;          ///< Maximum X coordinate value
      int minY;          ///< Minimum Y coordinate value
      int maxY;          ///< Maximum Y coordinate value
      bool isCalibrated; ///< Flag indicating if calibration has been performed
    };

    /**
     * @class TouchScreenDriver
     * @brief Driver for the touchscreen
     *
     * This class provides an interface to the touchscreen hardware,
     * including filtering, calibration, and PID control. It uses two
     * PID controllers to maintain the ball's position at a specified
     * setpoint by adjusting the platform's pitch and roll.
     */
    class TouchScreenDriver
    {
    private:
      TouchScreen *ts;              ///< Pointer to the TouchScreen hardware interface
      TouchFilter filter;           ///< Filter for smoothing touchscreen input
      TouchCalibration calibration; ///< Calibration data for the touchscreen
      PID *rollPID;                 ///< PID controller for roll (X axis)
      PID *pitchPID;                ///< PID controller for pitch (Y axis)

      double inputX;    ///< Current X position input to the PID controller
      double inputY;    ///< Current Y position input to the PID controller
      double outputX;   ///< Roll output from the PID controller
      double outputY;   ///< Pitch output from the PID controller
      double setpointX; ///< Target X position for the PID controller
      double setpointY; ///< Target Y position for the PID controller

      unsigned long ballLastSeen;                                         ///< Timestamp when the ball was last detected
      bool isCalibrating;                                                 ///< Flag indicating if calibration is in progress
      int calibrationStep;                                                ///< Current step in the calibration process
      unsigned long calibrationStartTime;                                 ///< Timestamp when calibration started
      int calibrationSamples[CALIBRATION_POINTS][2][CALIBRATION_SAMPLES]; ///< Calibration samples [point][x/y][sample]
      int calibrationSampleCount;                                         ///< Number of samples collected for the current calibration point

    public:
      /**
       * @brief Construct a new TouchScreenDriver object
       *
       * Initializes the touchscreen driver with the specified pin configuration.
       * Creates the TouchScreen and PID controller objects.
       *
       * @param xp X+ pin number
       * @param yp Y+ pin number
       * @param xm X- pin number
       * @param ym Y- pin number
       * @param ohms Resistance between X+ and X- in ohms
       */
      TouchScreenDriver(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym, uint16_t ohms);

      /**
       * @brief Destructor for TouchScreenDriver
       *
       * Properly cleans up dynamically allocated objects (TouchScreen and PID controllers).
       */
      ~TouchScreenDriver();

      /**
       * @brief Initialize the touchscreen
       *
       * Sets up the PID controllers with appropriate limits and sample times,
       * loads calibration data from EEPROM if available, and initializes
       * the setpoint to the center of the touchscreen.
       */
      void init();

      /**
       * @brief Process touchscreen input
       *
       * Reads the current ball position from the touchscreen, applies filtering,
       * and uses PID control to calculate platform adjustments to move the ball
       * toward the setpoint. Updates the servo values accordingly.
       *
       * If the ball is not detected for a certain period (LOST_BALL_TIMEOUT),
       * the platform returns to the home position.
       *
       * @param setpoint_x Normalized X setpoint (-1.0 to 1.0)
       * @param setpoint_y Normalized Y setpoint (-1.0 to 1.0)
       * @param servoValues Array to store calculated servo values
       *
       * @note This method does nothing if calibration is in progress or if the PID controllers are in manual mode.
       */
      void process(float setpoint_x, float setpoint_y, float *servoValues);

      /**
       * @brief Start touchscreen calibration
       *
       * Begins the calibration process, which requires the user to place the ball
       * at specific points on the touchscreen. During calibration, the PID controllers
       * are disabled and the platform is set to the home position.
       *
       * The calibration process consists of collecting samples at four corners of the touchscreen.
       * Once complete, the calibration data is saved to EEPROM.
       */
      void startCalibration();

      /**
       * @brief Check if calibration is in progress
       *
       * @return true if calibration is currently in progress
       * @return false if calibration is not in progress
       */
      bool isCalibrationInProgress();

      /**
       * @brief Set PID parameters
       *
       * Updates the PID parameters for the specified axis. The parameters are validated
       * to ensure they are non-negative and within reasonable limits to prevent unstable behavior.
       *
       * @param axis 'x' or 'y' to specify which axis to update
       * @param p Proportional term (0.0 to 100.0)
       * @param i Integral term (0.0 to 10.0)
       * @param d Derivative term (0.0 to 10.0)
       *
       * @note If invalid parameters are provided, an error is logged and no changes are made.
       */
      void setPID(char axis, double p, double i, double d);

      /**
       * @brief Get current PID parameters
       *
       * Retrieves the current PID parameters for the specified axis.
       *
       * @param axis 'x' or 'y' to specify which axis to query
       * @param p Reference to store the proportional term
       * @param i Reference to store the integral term
       * @param d Reference to store the derivative term
       *
       * @note If an invalid axis is specified, all parameters are set to 0.0.
       */
      void getPID(char axis, double &p, double &i, double &d);

      /**
       * @brief Reset PID controllers to default values
       *
       * Resets both PID controllers to their default values and settings.
       * This is useful if the system becomes unstable due to poorly tuned PID parameters.
       *
       * Default values:
       * - Roll: P=3.0, I=0.0, D=0.0
       * - Pitch: P=1.0, I=0.0, D=0.0
       */
      void resetPID();

    private:
      /**
       * @brief Load calibration data from EEPROM
       *
       * Loads touchscreen calibration data from EEPROM and validates it.
       *
       * @return true if valid calibration data was loaded
       * @return false if no valid calibration data was found
       */
      bool loadCalibration();

      /**
       * @brief Save calibration data to EEPROM
       *
       * Saves the current touchscreen calibration data to EEPROM.
       *
       * @return true if the data was successfully saved
       * @return false if an error occurred
       */
      bool saveCalibration();

      /**
       * @brief Process a calibration point
       *
       * Collects samples for the current calibration point and advances to the next
       * point when enough samples have been collected.
       *
       * @param step Current calibration step (0-3, corresponding to the four corners)
       * @param p Touchscreen point data
       */
      void processCalibrationPoint(int step, TSPoint p);

      /**
       * @brief Finish the calibration process
       *
       * Calculates the final calibration values from the collected samples,
       * saves them to EEPROM, and re-enables the PID controllers.
       */
      void finishCalibration();
    };

  } // namespace drivers
} // namespace stewy
