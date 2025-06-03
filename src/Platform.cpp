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

/*
   Derived from the work of Daniel Waters, https://www.youtube.com/watch?v=1jrP3_1ML9M
*/

// #include "config.h"
#include "Arduino.h"
#include "Platform.h"

bool Platform::home(float *servoValues)
{
  return moveTo(servoValues, 0, 0, 0, 0, 0, 0); // HOME position. No rotation, no translation.
}

/*
 * Move to a given sway, surge, heave, pitch, roll and yaw values.
 *
 * @param servoValues Array of servo values to be modified
 * @param sway Translation along X axis in mm
 * @param surge Translation along Y axis in mm
 * @param heave Translation along Z axis in mm
 * @param pitch Rotation around X axis in degrees
 * @param roll Rotation around Y axis in degrees
 * @param yaw Rotation around Z axis in degrees
 * @return true if movement is possible, false otherwise
 */
bool Platform::moveTo(float *servoValues, int sway, int surge, int heave, float pitch, float roll, float yaw)
{
  // Check if parameters are within allowed boundaries
  if (sway < SWAY_MIN || sway > SWAY_MAX)
  {
    Log.error("Sway value %d is outside allowed range [%d, %d]", sway, SWAY_MIN, SWAY_MAX);
    return false;
  }

  if (surge < SURGE_MIN || surge > SURGE_MAX)
  {
    Log.error("Surge value %d is outside allowed range [%d, %d]", surge, SURGE_MIN, SURGE_MAX);
    return false;
  }

  if (heave < HEAVE_MIN || heave > HEAVE_MAX)
  {
    Log.error("Heave value %d is outside allowed range [%d, %d]", heave, HEAVE_MIN, HEAVE_MAX);
    return false;
  }

  if (pitch < PITCH_MIN || pitch > PITCH_MAX)
  {
    Log.error("Pitch value %.2f is outside allowed range [%d, %d]", pitch, PITCH_MIN, PITCH_MAX);
    return false;
  }

  if (roll < ROLL_MIN || roll > ROLL_MAX)
  {
    Log.error("Roll value %.2f is outside allowed range [%d, %d]", roll, ROLL_MIN, ROLL_MAX);
    return false;
  }

  if (yaw < YAW_MIN || yaw > YAW_MAX)
  {
    Log.error("Yaw value %.2f is outside allowed range [%d, %d]", yaw, YAW_MIN, YAW_MAX);
    return false;
  }

  // Early exit if we're already at the desired position
  if (_sp_sway == sway && _sp_surge == surge && _sp_heave == heave &&
      _sp_pitch == pitch && _sp_roll == roll && _sp_yaw == yaw)
  {
    return true;
  }

  // Store old values in case we need to revert
  float oldValues[6];
  memcpy(oldValues, servoValues, 6 * sizeof(float));

  // Pre-compute trigonometric values (only calculate once)
  const double cr = cos(radians(roll));
  const double cp = cos(radians(pitch));
  const double cy = cos(radians(yaw));
  const double sr = sin(radians(roll));
  const double sp = sin(radians(pitch));
  const double sy = sin(radians(yaw));

  // Pre-compute common products used in the loop
  const double sp_sr = sp * sr;
  const double cr_cy = cr * cy;
  const double cr_sy = cr * sy;
  const double sp_cr = sp * cr;

  // Pre-compute Z offset with rotation point adjustment
  const double z_offset = Z_HOME + heave;

  // Pre-compute servo angle mapping constants
  const double angle_range = _servo_max_angle - _servo_min_angle;
  const double mid_angle = _servo_min_angle + (angle_range / 2);

  // Pre-compute squared rod length for distance comparison
  const double rod_length_sq = pow(ROD_LENGTH, 2);
  const double arm_length_sq = pow(ARM_LENGTH, 2);
  const double max_reach_sq = pow(ARM_LENGTH + ROD_LENGTH, 2);

  bool bOk = true;

#ifdef USE_ENHANCED_IK
  // Enhanced IK algorithm with adjustable rotation point and translation/rotation order
  for (int i = 0; i < 6 && bOk; i++)
  {
    double pivot_x, pivot_y, pivot_z;

    // Get platform and base coordinates
    const double px = P_COORDS[i][0];
    const double py = P_COORDS[i][1];
    const double bx = B_COORDS[i][0];
    const double by = B_COORDS[i][1];

    // Apply rotation around adjustable point
    if (TRANSLATION_FIRST)
    {
      // Apply translation first, then rotation
      pivot_x = px;
      pivot_y = py;
      pivot_z = ROTATION_POINT_OFFSET;

      // Apply rotation
      double rotated_x = pivot_x * cr_cy + pivot_y * (sp_sr * cr - cp * sy) - pivot_z * (cp * sr + sp * sy * cr);
      double rotated_y = pivot_x * cr_sy + pivot_y * (cp * cy + sp_sr * sy) - pivot_z * (sp * cy - cp * sr * sy);
      double rotated_z = -pivot_x * sr + pivot_y * sp_cr + pivot_z * cp * cr;

      // Apply translation
      pivot_x = rotated_x + sway;
      pivot_y = rotated_y + surge;
      pivot_z = rotated_z + z_offset;
    }
    else
    {
      // Apply rotation first, then translation
      pivot_x = px * cr_cy + py * (sp_sr * cr - cp * sy);
      pivot_y = px * cr_sy + py * (cp * cy + sp_sr * sy);
      pivot_z = -px * sr + py * sp_cr + z_offset + ROTATION_POINT_OFFSET;

      // Apply translation
      pivot_x += sway;
      pivot_y += surge;
      pivot_z += heave - ROTATION_POINT_OFFSET; // Adjust for rotation point offset
    }

    // Calculate squared distance (avoid sqrt until necessary)
    const double dx = pivot_x - bx;
    const double dy = pivot_y - by;
    const double d2 = dx * dx + dy * dy + pivot_z * pivot_z;

    // Early exit if distance is physically impossible
    if (d2 > max_reach_sq)
    {
      Log.error("Distance too great at servo %d: %.2f > %.2f", i, sqrt(d2), ARM_LENGTH + ROD_LENGTH);
      bOk = false;
      break;
    }

    // Geometry calculations
    const double k = d2 - (rod_length_sq - arm_length_sq);
    const double l = 2 * ARM_LENGTH * pivot_z;
    const double m = 2 * ARM_LENGTH * (cos(THETA_S[i]) * dx + sin(THETA_S[i]) * dy);

    // Check for asymptotic condition
    const double divisor = sqrt(l * l + m * m);
    const double k_ratio = k / divisor;

    if (abs(k_ratio) >= 1)
    {
      Log.error("Asymptotic condition at servo %d: |%.2f| >= 1", i, k_ratio);
#ifdef SLAM
      servoValues[i] = (k_ratio > 0) ? _servo_max_angle : _servo_min_angle;
#else
      bOk = false;
#endif
      break;
    }

    // Calculate servo angle
    const double servo_rad = asin(k_ratio) - atan2(m, l); // Using atan2 is more robust
    double servo_deg = map(degrees(servo_rad), -90, 90, _servo_min_angle, _servo_max_angle);

    // Constrain to valid range
    servo_deg = constrain(servo_deg, _servo_min_angle, _servo_max_angle);
    servoValues[i] = servo_deg;
  }
#else
  // Original IK algorithm
  for (int i = 0; i < 6 && bOk; i++)
  {
    // Calculate platform pivot coordinates more efficiently
    const double px = P_COORDS[i][0];
    const double py = P_COORDS[i][1];
    const double bx = B_COORDS[i][0];
    const double by = B_COORDS[i][1];

    // Calculate pivot coordinates with optimized expressions
    const double pivot_x = px * cr_cy + py * (sp_sr * cr - cp * sy) + sway;
    const double pivot_y = px * cr_sy + py * (cp * cy + sp_sr * sy) + surge;
    const double pivot_z = -px * sr + py * sp_cr + z_offset;

    // Calculate squared distance (avoid sqrt until necessary)
    const double dx = pivot_x - bx;                          // x-distance
    const double dy = pivot_y - by;                          // y-distance
    const double d2 = dx * dx + dy * dy + pivot_z * pivot_z; // squared distance

    // Early exit if distance is physically impossible
    if (d2 > max_reach_sq)
    { // (actually comparing the squared distance)
      Log.error("Distance too great at servo %d: %.2f > %.2f", i, sqrt(d2), ARM_LENGTH + ROD_LENGTH);
      bOk = false;
      break;
    }

    // Geometry calculations
    const double k = d2 - (rod_length_sq - arm_length_sq);
    const double l = 2 * ARM_LENGTH * pivot_z;
    const double m = 2 * ARM_LENGTH * (cos(THETA_S[i]) * dx + sin(THETA_S[i]) * dy);

    // Check for asymptotic condition
    const double divisor = sqrt(l * l + m * m); // Avoid division by zero
    const double k_ratio = k / divisor;         // Ratio for asin calculation

    if (abs(k_ratio) >= 1) // Is the ratio in the valid range for asin? If not, this is an "asymptotic condition".
    {
      Log.error("Asymptotic condition at servo %d: |%.2f| >= 1", i, k_ratio);
#ifdef SLAM
      servoValues[i] = (k_ratio > 0) ? _servo_max_angle : _servo_min_angle;
#else
      bOk = false;
#endif
      break;
    }

    // Calculate servo angle
    const double servo_rad = asin(k_ratio) - atan2(m, l); // Using atan2 is more robust
    double servo_deg = map(degrees(servo_rad), -90, 90, _servo_min_angle, _servo_max_angle);

    // Constrain to valid range
    servo_deg = constrain(servo_deg, _servo_min_angle, _servo_max_angle);
    servoValues[i] = servo_deg;
  }
#endif

  if (bOk)
  {
    // Update setpoints
    _sp_sway = sway;
    _sp_surge = surge;
    _sp_heave = heave;
    _sp_pitch = pitch;
    _sp_roll = roll;
    _sp_yaw = yaw;

    // Apply AGGRO scaling more efficiently
    for (int i = 0; i < 6; i++)
    {
      const double diff = servoValues[i] - mid_angle;
      servoValues[i] = mid_angle + (diff * AGGRO);
      servoValues[i] = constrain(servoValues[i], _servo_min_angle, _servo_max_angle);
    }
  }
  else
  {
    // Restore old values
    memcpy(servoValues, oldValues, 6 * sizeof(float));
  }

  return bOk;
}

/*
 * Convenience function: Move to a given pitch / roll angle (in degrees). Since the majority
 * of the use-case is to roll/pitch to keep the ball on the plate, most of the time we'll be
 * calling this version of the moveTo function.
 *
 * @param servoValues Array of servo values to be modified
 * @param pitch Pitch angle in degrees
 * @param roll Roll angle in degrees
 * @return true if movement is possible, false otherwise
 */
bool Platform::moveTo(float *servoValues, float pitch, float roll)
{
  // Check if parameters are within allowed boundaries
  if (pitch < PITCH_MIN || pitch > PITCH_MAX)
  {
    Log.error("Pitch value %.2f is outside allowed range [%d, %d]", pitch, PITCH_MIN, PITCH_MAX);
    return false;
  }

  if (roll < ROLL_MIN || roll > ROLL_MAX)
  {
    Log.error("Roll value %.2f is outside allowed range [%d, %d]", roll, ROLL_MIN, ROLL_MAX);
    return false;
  }

  return moveTo(servoValues, _sp_sway, _sp_surge, _sp_heave, pitch, roll, _sp_yaw);
}

int Platform::getSway()
{
  return _sp_sway;
}

int Platform::getSurge()
{
  return _sp_surge;
}

int Platform::getHeave()
{
  return _sp_heave;
}

float Platform::getPitch()
{
  return _sp_pitch;
}

float Platform::getRoll()
{
  return _sp_roll;
}

float Platform::getYaw()
{
  return _sp_yaw;
}
