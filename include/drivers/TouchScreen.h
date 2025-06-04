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
 */

#include <TouchScreen.h> // from https://github.com/adafruit/Touch-Screen-Library
#include <PID_v1.h>      // https://github.com/br3ttb/Arduino-PID-Library
#include <EEPROM.h>      // for storing calibration data
#include "core/Config.h"

namespace stewy {
namespace drivers {

/**
 * @class TouchFilter
 * @brief Filter for touchscreen input
 * 
 * This class provides filtering for touchscreen input to reduce noise
 * and improve stability.
 */
class TouchFilter {
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

/**
 * @struct TouchCalibration
 * @brief Touchscreen calibration data
 * 
 * This structure stores calibration data for the touchscreen.
 */
struct TouchCalibration {
  int minX;
  int maxX;
  int minY;
  int maxY;
  bool isCalibrated;
};

/**
 * @class TouchScreenDriver
 * @brief Driver for the touchscreen
 * 
 * This class provides an interface to the touchscreen hardware,
 * including filtering, calibration, and PID control.
 */
class TouchScreenDriver {
private:
  TouchScreen* ts;
  TouchFilter filter;
  TouchCalibration calibration;
  PID* rollPID;
  PID* pitchPID;
  
  double inputX;
  double inputY;
  double outputX;
  double outputY;
  double setpointX;
  double setpointY;
  
  unsigned long ballLastSeen;
  bool isCalibrating;
  int calibrationStep;
  unsigned long calibrationStartTime;
  int calibrationSamples[CALIBRATION_POINTS][2][CALIBRATION_SAMPLES]; // [point][x/y][sample]
  int calibrationSampleCount;

public:
  /**
   * @brief Construct a new TouchScreenDriver object
   * 
   * @param xp XP pin
   * @param yp YP pin
   * @param xm XM pin
   * @param ym YM pin
   * @param ohms Resistance between X+ and X-
   */
  TouchScreenDriver(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym, uint16_t ohms);
  
  /**
   * @brief Destructor for TouchScreenDriver
   * 
   * Properly cleans up dynamically allocated objects
   */
  ~TouchScreenDriver();
  
  /**
   * @brief Initialize the touchscreen
   */
  void init();
  
  /**
   * @brief Process touchscreen input
   * 
   * @param setpoint_x X setpoint (-1.0 to 1.0)
   * @param setpoint_y Y setpoint (-1.0 to 1.0)
   * @param servoValues Array to store calculated servo values
   */
  void process(float setpoint_x, float setpoint_y, float* servoValues);
  
  /**
   * @brief Start touchscreen calibration
   */
  void startCalibration();
  
  /**
   * @brief Check if calibration is in progress
   * 
   * @return true if calibration is in progress
   * @return false if calibration is not in progress
   */
  bool isCalibrationInProgress();
  
  /**
   * @brief Set PID parameters
   * 
   * @param axis 'x' or 'y'
   * @param p P value
   * @param i I value
   * @param d D value
   */
  void setPID(char axis, double p, double i, double d);
  
  /**
   * @brief Get current PID parameters
   * 
   * @param axis 'x' or 'y'
   * @param p P value output
   * @param i I value output
   * @param d D value output
   */
  void getPID(char axis, double& p, double& i, double& d);
  
  /**
   * @brief Reset PID controllers to default values
   * 
   * This method resets the PID controllers to their default values,
   * which can be useful if the system becomes unstable.
   */
  void resetPID();
  
private:
  bool loadCalibration();
  bool saveCalibration();
  void processCalibrationPoint(int step, TSPoint p);
  void finishCalibration();
};

} // namespace drivers
} // namespace stewy
