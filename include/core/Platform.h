#pragma once
/**
 * @file Platform.h
 * @brief Core Stewart platform kinematics and control
 *
 * This file contains the core functionality for the Stewart platform,
 * including inverse kinematics calculations and platform movement control.
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

#include <ArduinoLog.h>
#include "core/PlatformGeometry.h"

namespace stewy
{
  namespace core
  {

    /**
     * @class Platform
     * @brief Stewart platform control class
     *
     * This class handles the inverse kinematics and movement control
     * for a 6DOF Stewart platform. It calculates servo angles based on
     * desired platform position and orientation using inverse kinematics.
     * The class maintains internal state of the current platform setpoints
     * and provides methods to move the platform to new positions.
     */
    class Platform
    {
    private:
      int _servo_min_angle; ///< Minimum allowed servo angle in degrees
      int _servo_max_angle; ///< Maximum allowed servo angle in degrees

      // Setpoints (internal state)
      int _sp_sway = 0;  ///< Current sway (x-axis translation) in mm
      int _sp_surge = 0; ///< Current surge (y-axis translation) in mm
      int _sp_heave = 0; ///< Current heave (z-axis translation) in mm

      float _sp_pitch = 0; ///< Current pitch (x-axis rotation) in degrees
      float _sp_roll = 0;  ///< Current roll (y-axis rotation) in degrees
      float _sp_yaw = 0;   ///< Current yaw (z-axis rotation) in degrees

    public:
      /**
       * @brief Construct a new Platform object
       *
       * Initializes a new Platform object with the specified servo angle limits.
       * These limits define the physical constraints of the servos used in the platform.
       *
       * @param servo_min_angle Minimum servo angle in degrees (typically 0)
       * @param servo_max_angle Maximum servo angle in degrees (typically 180)
       */
      Platform(int servo_min_angle, int servo_max_angle);

      /**
       * @brief Destructor for Platform object
       *
       * No dynamic memory to clean up in this class.
       */
      ~Platform() = default;

      /**
       * @brief Move platform to home position
       *
       * Moves the platform to its neutral home position with no translation or rotation.
       * This is equivalent to calling moveTo(servoValues, 0, 0, 0, 0, 0, 0).
       *
       * @param servoValues Pointer to an array of 6 floats where the calculated servo angles will be stored
       * @return true if the movement is physically possible and the servo values were calculated successfully
       * @return false if the movement is not possible due to physical constraints
       *
       * @see moveTo
       */
      bool home(float *servoValues);

      /**
       * @brief Move platform to specified position and orientation
       *
       * Calculates servo angles for the specified 6DOF position and orientation using inverse kinematics.
       * The calculated angles are stored in the servoValues array. If the movement is not physically
       * possible (e.g., beyond the platform's range of motion), the function returns false and
       * the servoValues array remains unchanged.
       *
       * @param servoValues Pointer to an array of 6 floats where the calculated servo angles will be stored
       * @param sway Translation along X axis in mm, constrained by SWAY_MIN and SWAY_MAX
       * @param surge Translation along Y axis in mm, constrained by SURGE_MIN and SURGE_MAX
       * @param heave Translation along Z axis in mm, constrained by HEAVE_MIN and HEAVE_MAX
       * @param pitch Rotation around X axis in degrees, constrained by PITCH_MIN and PITCH_MAX
       * @param roll Rotation around Y axis in degrees, constrained by ROLL_MIN and ROLL_MAX
       * @param yaw Rotation around Z axis in degrees, constrained by YAW_MIN and YAW_MAX
       * @return true if the movement is physically possible and the servo values were calculated successfully
       * @return false if the movement is not possible due to physical constraints or parameter limits
       *
       * @note If the function returns false, the servoValues array will not be modified.
       * @note The function performs boundary checking on all parameters and logs errors for out-of-range values.
       * @note The AGGRO scaling factor is applied to the calculated servo angles to increase the range of motion.
       */
      bool moveTo(float *servoValues, int sway, int surge, int heave, float pitch, float roll, float yaw);

      /**
       * @brief Move platform to specified pitch and roll angles
       *
       * Convenience method that maintains the current sway, surge, heave, and yaw values
       * while changing only the pitch and roll angles. This is useful for simple tilting
       * operations without changing the platform's position.
       *
       * @param servoValues Pointer to an array of 6 floats where the calculated servo angles will be stored
       * @param pitch Pitch angle in degrees, constrained by PITCH_MIN and PITCH_MAX
       * @param roll Roll angle in degrees, constrained by ROLL_MIN and ROLL_MAX
       * @return true if the movement is physically possible and the servo values were calculated successfully
       * @return false if the movement is not possible due to physical constraints or parameter limits
       *
       * @note This method calls the full moveTo method with the current values for sway, surge, heave, and yaw.
       * @see moveTo
       */
      bool moveTo(float *servoValues, float pitch, float roll);

      /**
       * @brief Get current sway value
       *
       * @return Current sway (x-axis translation) in mm
       */
      int getSway();

      /**
       * @brief Get current surge value
       *
       * @return Current surge (y-axis translation) in mm
       */
      int getSurge();

      /**
       * @brief Get current heave value
       *
       * @return Current heave (z-axis translation) in mm
       */
      int getHeave();

      /**
       * @brief Get current pitch value
       *
       * @return Current pitch (x-axis rotation) in degrees
       */
      float getPitch();

      /**
       * @brief Get current roll value
       *
       * @return Current roll (y-axis rotation) in degrees
       */
      float getRoll();

      /**
       * @brief Get current yaw value
       *
       * @return Current yaw (z-axis rotation) in degrees
       */
      float getYaw();
    };

  } // namespace core
} // namespace stewy
