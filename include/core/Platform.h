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
 */

#include <ArduinoLog.h>
#include "core/PlatformGeometry.h"

namespace stewy {
namespace core {

/**
 * @class Platform
 * @brief Stewart platform control class
 * 
 * This class handles the inverse kinematics and movement control
 * for a 6DOF Stewart platform.
 */
class Platform
{
private:
  int _servo_min_angle;
  int _servo_max_angle;

  // Setpoints (internal state)
  int _sp_sway = 0;  // sway (x) in mm
  int _sp_surge = 0; // surge (y) in mm
  int _sp_heave = 0; // heave (z) in mm

  float _sp_pitch = 0; // pitch (x) in degrees (input to moveTo is in degrees, converted to radians internally)
  float _sp_roll = 0;  // roll (y) in degrees (input to moveTo is in degrees, converted to radians internally)
  float _sp_yaw = 0;   // yaw (z) in degrees (input to moveTo is in degrees, converted to radians internally)

public:
  /**
   * @brief Construct a new Platform object
   * 
   * @param servo_min_angle Minimum servo angle in degrees
   * @param servo_max_angle Maximum servo angle in degrees
   */
  Platform(int servo_min_angle, int servo_max_angle);
  
  /**
   * @brief Destructor for Platform object
   * 
   * No dynamic memory to clean up in this class
   */
  ~Platform() = default;
  
  /**
   * @brief Move platform to home position
   * 
   * @param servoValues Array to store calculated servo values
   * @return true if movement is possible
   * @return false if movement is not possible
   */
  bool home(float *servoValues);
  
  /**
   * @brief Move platform to specified position and orientation
   * 
   * @param servoValues Array to store calculated servo values
   * @param sway Translation along X axis in mm
   * @param surge Translation along Y axis in mm
   * @param heave Translation along Z axis in mm
   * @param pitch Rotation around X axis in degrees
   * @param roll Rotation around Y axis in degrees
   * @param yaw Rotation around Z axis in degrees
   * @return true if movement is possible
   * @return false if movement is not possible
   */
  bool moveTo(float *servoValues, int sway, int surge, int heave, float pitch, float roll, float yaw);
  
  /**
   * @brief Move platform to specified pitch and roll angles
   * 
   * @param servoValues Array to store calculated servo values
   * @param pitch Pitch angle in degrees
   * @param roll Roll angle in degrees
   * @return true if movement is possible
   * @return false if movement is not possible
   */
  bool moveTo(float *servoValues, float pitch, float roll);

  // Getters for current platform state
  int getSway();
  int getSurge();
  int getHeave();
  float getPitch();
  float getRoll();
  float getYaw();
};

} // namespace core
} // namespace stewy
