/**
 * @file BasicMovement.cpp
 * @brief Basic movement example for the Stewart platform
 *
 * This example demonstrates basic movement of the Stewart platform,
 * including home position, pitch, roll, and combined movements.
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

#include <Arduino.h>
#include <ArduinoLog.h>
#include <Servo.h>
#include "core/Platform.h"
#include "core/Config.h"

using namespace stewy::core;

// Create platform object
Platform platform(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

// Create servo objects
Servo servos[6];

// Servo setpoints in degrees
float servoValues[6];

// Function to convert angle to microseconds
float toMicroseconds(int angle)
{
  return map(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, SERVO_MIN_US, SERVO_MAX_US);
}

// Update servo positions
void updateServos()
{
  for (int i = 0; i < 6; i++)
  {
    // Apply reverse if needed
    float val = servoValues[i];
    if (SERVO_REVERSE[i])
    {
      val = SERVO_MIN_ANGLE + (SERVO_MAX_ANGLE - val);
    }

    // Convert to microseconds and apply trim
    val = toMicroseconds(val) + SERVO_TRIM[i];

    // Write to servo
    servos[i].writeMicroseconds(constrain(val, SERVO_MIN_US, SERVO_MAX_US));
  }
}

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);
  Log.begin(LOG_LEVEL, &Serial);
  Log.info("Basic Movement Example Starting...");

  // Initialize servos
  for (int i = 0; i < 6; i++)
  {
    servos[i].attach(SERVO_PINS[i]);
  }

  // Move to home position
  platform.home(servoValues);
  updateServos();
  delay(2000);
}

void loop()
{
  // Pitch forward
  Log.info("Pitching forward...");
  platform.moveTo(servoValues, 0, 0, 0, 15, 0, 0);
  updateServos();
  delay(2000);

  // Return to home
  Log.info("Returning to home...");
  platform.home(servoValues);
  updateServos();
  delay(1000);

  // Roll right
  Log.info("Rolling right...");
  platform.moveTo(servoValues, 0, 0, 0, 0, 15, 0);
  updateServos();
  delay(2000);

  // Return to home
  Log.info("Returning to home...");
  platform.home(servoValues);
  updateServos();
  delay(1000);

  // Combined pitch and roll
  Log.info("Combined pitch and roll...");
  platform.moveTo(servoValues, 0, 0, 0, 10, 10, 0);
  updateServos();
  delay(2000);

  // Return to home
  Log.info("Returning to home...");
  platform.home(servoValues);
  updateServos();
  delay(1000);

  // Heave up
  Log.info("Heaving up...");
  platform.moveTo(servoValues, 0, 0, 20, 0, 0, 0);
  updateServos();
  delay(2000);

  // Return to home
  Log.info("Returning to home...");
  platform.home(servoValues);
  updateServos();
  delay(1000);

  // Yaw rotation
  Log.info("Yaw rotation...");
  platform.moveTo(servoValues, 0, 0, 0, 0, 0, 20);
  updateServos();
  delay(2000);

  // Return to home
  Log.info("Returning to home...");
  platform.home(servoValues);
  updateServos();
  delay(3000);
}
