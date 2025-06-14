#pragma once
/**
 * @file Config.h
 * @brief Global configuration settings
 *
 * This file contains global configuration settings for the Stewart platform.
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

namespace stewy
{
  namespace core
  {

// Feature enable/disable flags
#define ENABLE_SERIAL_COMMANDS // Comment out, to omit Command shell code
#define ENABLE_NUNCHUCK        // Comment out, to omit Nunchuck code
#define ENABLE_TOUCHSCREEN     // Comment out, to omit Touchscreen code

/*
  Comment out, to disable Servos. Servos can get hot if you don't set
  them to a "safe" value. Commenting this line out allows to test things that
  are not servo-related, without killing the battery, browning out the USB,
  or overheating the servos.
*/
#define ENABLE_SERVOS

// LED configuration
#define LED_PIN LED_BUILTIN // LED pin for status indication

// Logging configuration
#define LOG_LEVEL LOG_LEVEL_TRACE

// Main loop timing configuration
#define MAIN_LOOP_INTERVAL_MS 20 // Target time in milliseconds for each main loop iteration

// Servo movement configuration
#define SERVO_ACCELERATION_ENABLED // Enable/disable servo acceleration/deceleration
#define SERVO_MAX_SPEED 10.0f      // Maximum speed in degrees per loop iteration
#define SERVO_ACCELERATION 0.3f    // Acceleration/deceleration rate in degrees per loop iteration squared

// Servo configuration
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 360
    const int SERVO_MID_ANGLE = SERVO_MIN_ANGLE + (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) / 2;

#define SERVO_MIN_US 755
#define SERVO_MAX_US 2250

    const int SERVO_MID_US = SERVO_MIN_US + (SERVO_MAX_US - SERVO_MIN_US) / 2;

    // Which servos are reversed. 1 = reversed, 0 = normal.
    const int SERVO_REVERSE[6] = {0, 1, 0, 1, 0, 1};

    // Servo trim values, in microseconds, AFTER reversing
    const int SERVO_TRIM[] = {0, 20, 0, 135, 0, 120};

    // Servo pin assignments
    const int SERVO_PINS[] = {0, 1, 2, 3, 4, 5};

// Touchscreen configuration
#ifdef ENABLE_TOUCHSCREEN
#define XP A7       // YELLOW / XRT. can be a digital pin.
#define XM A6       // WHITE / XLE. must be an analog pin, use "An" notation!
#define YP A8       // RED / YLO. must be an analog pin, use "An" notation!
#define YM A9       // BLACK / YUP. can be a digital pin.
#define TS_OHMS 711 // resistance between X+ and X-

// Default Min / max values of X and Y (will be overridden by calibration)
#define TS_DEFAULT_MIN_X 1
#define TS_DEFAULT_MAX_X 950
#define TS_DEFAULT_MIN_Y 100
#define TS_DEFAULT_MAX_Y 930

// EEPROM address for storing touchscreen calibration data
#define TOUCH_CALIBRATION_ADDR 0

// Touchscreen filtering
#define TOUCH_FILTER_SAMPLES 5  // Number of samples to use in moving average filter
#define TOUCH_FILTER_WEIGHT 0.7 // Weight for exponential filter (0-1, higher = more smoothing)
#define TOUCH_DEADZONE 5        // Deadzone to ignore small movements (in raw ADC units)

// Calibration process
#define CALIBRATION_POINTS 4   // Number of points to use for calibration
#define CALIBRATION_DELAY 2000 // Delay between calibration points in ms
#define CALIBRATION_SAMPLES 10 // Number of samples to average for each calibration point

// Time (in millis) between the touch sensor "losing" the ball, and the platform
// getting a signal to go to the "home" position.
#define LOST_BALL_TIMEOUT 250

// PID configuration
#define ROLL_PID_SAMPLE_TIME 10
#define ROLL_PID_LIMIT_MIN -1024
#define ROLL_PID_LIMIT_MAX 1024

#define PITCH_PID_SAMPLE_TIME 10
#define PITCH_PID_LIMIT_MIN -1024
#define PITCH_PID_LIMIT_MAX 1024
#endif // ENABLE_TOUCHSCREEN

// Nunchuck configuration
#ifdef ENABLE_NUNCHUCK
// Delay between movements of the Setpoint, in SQUARE mode.
#define SQUARE_DELAY_MS 1000

// Specifies the maximum time between button clicks that are interpreted as a
// double-click. If the time between clicks exceeds this value, the clicks are
// interpreted as single clicks.
#define NUNCHUCK_DBLCLICK_THRESHOLD_MS 500
#endif // ENABLE_NUNCHUCK

// Platform movement limits
#define MIN_PITCH -20
#define MAX_PITCH 23
#define MIN_ROLL -23
#define MAX_ROLL 20
#define MIN_YAW -69
#define MAX_YAW 69
#define MIN_SWAY -55
#define MAX_SWAY 55
#define MIN_SURGE -70
#define MAX_SURGE 55
#define MIN_HEAVE -22
#define MAX_HEAVE 25

    /**
     * @struct xy_coordf
     * @brief 2D coordinate with floating point values
     */
    typedef struct xy_coordf
    {
      float x; // -1.0 to 1.0
      float y; // -1.0 to 1.0
    } xy_coordf;

    // Default setpoint (center of platform)
    const xy_coordf DEFAULT_SETPOINT = {0, 0};

  } // namespace core
} // namespace stewy
