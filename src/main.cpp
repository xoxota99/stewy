/**
 * @file main.cpp
 * @brief Main entry point for the Stewart platform project
 *
 * This file contains the setup and loop functions required by Arduino/Teensy.
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
#include "core/Config.h"
#include "core/Platform.h"
#ifdef ENABLE_TOUCHSCREEN
#include "drivers/TouchScreen.h"
#endif

#ifdef ENABLE_NUNCHUCK
#include "drivers/Nunchuck.h"
#endif

#ifdef ENABLE_SERIAL_COMMANDS
#include "ui/CommandLine.h"
#endif

// this is the magic trick for printf to support float
asm(".global _printf_float");
// this is the magic trick for scanf to support float
asm(".global _scanf_float");

// Define the namespace for the Stewart platform
using namespace stewy;

// Create platform object
core::Platform *platform;

#ifdef ENABLE_SERVOS
// Create servo objects
Servo servos[6];
#endif

// Servo setpoints in degrees
float servoValues[6];

// Current actual servo positions (for acceleration/deceleration)
float currentServoPositions[6];

// Current servo velocities (for acceleration/deceleration)
float servoVelocities[6];

// Create driver objects
drivers::TouchScreenDriver *touchscreen;
drivers::NunchuckDriver *nunchuck;

// Create UI objects
ui::CommandLine *commandLine;

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
    float targetPosition = servoValues[i];
    float currentPosition = currentServoPositions[i];

#ifdef SERVO_ACCELERATION_ENABLED

    // Calculate the distance to the target
    float distance = targetPosition - currentPosition;

    if (abs(distance) > 0.01f) // Small threshold to avoid tiny movements
    {
      // Determine the direction of movement
      float direction = (distance > 0) ? 1.0f : -1.0f;

      // Calculate the desired velocity based on distance
      float desiredVelocity = direction * min(abs(distance), SERVO_MAX_SPEED);

      // Apply acceleration/deceleration
      if (servoVelocities[i] < desiredVelocity)
      {
        // Accelerate
        servoVelocities[i] = min(servoVelocities[i] + SERVO_ACCELERATION, desiredVelocity);
      }
      else if (servoVelocities[i] > desiredVelocity)
      {
        // Decelerate
        servoVelocities[i] = max(servoVelocities[i] - SERVO_ACCELERATION, desiredVelocity);
      }

      // Apply velocity to position
      currentPosition += servoVelocities[i];

      // Check if we've reached or overshot the target
      if ((direction > 0 && currentPosition >= targetPosition) ||
          (direction < 0 && currentPosition <= targetPosition))
      {
        currentPosition = targetPosition;
        servoVelocities[i] = 0;
      }
    }
    else
    {
      // We're at the target, ensure velocity is zero
      servoVelocities[i] = 0;
    }

    // Update the current position
    currentServoPositions[i] = currentPosition;
#else

    // No acceleration/deceleration, just use the target position
    currentPosition = targetPosition;
    currentServoPositions[i] = currentPosition;
#endif // SERVO_ACCELERATION_ENABLED

    // Apply reverse if needed
    float val = currentPosition;
    if (core::SERVO_REVERSE[i])
    {
      val = SERVO_MIN_ANGLE + (SERVO_MAX_ANGLE - val);
    }

    // Convert to microseconds and apply trim
    val = toMicroseconds(val) + core::SERVO_TRIM[i];

// Write to servo
#ifdef ENABLE_SERVOS
    servos[i].writeMicroseconds(constrain(val, SERVO_MIN_US, SERVO_MAX_US));
#endif
  }
}

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);
  delay(100);

  Log.begin(LOG_LEVEL, &Serial);
  Log.info("Stewy Platform Starting...");
  Log.info("Built %s, %s", __DATE__, __TIME__);

  // Initialize platform
  platform = new core::Platform(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

  // Initialize servo position and velocity arrays
  for (int i = 0; i < 6; i++)
  {
    currentServoPositions[i] = core::SERVO_MID_ANGLE;
    servoVelocities[i] = 0.0f;
  }

// Initialize servos
#ifdef ENABLE_SERVOS
  Log.info("Initializing servos...");
  for (int i = 0; i < 6; i++)
  {
    servos[i].attach(core::SERVO_PINS[i]);
  }
#else
  Log.info("Servo control is DISABLED");
#endif

// Initialize touchscreen
#ifdef ENABLE_TOUCHSCREEN
  Log.info("Initializing touchscreen...");
  touchscreen = new drivers::TouchScreenDriver(XP, YP, XM, YM, TS_OHMS);
  touchscreen->init();
#else
  Log.info("Touchscreen support is DISABLED");
#endif

// Initialize nunchuck
#ifdef ENABLE_NUNCHUCK
  Log.info("Initializing nunchuck...");
  nunchuck = new drivers::NunchuckDriver();
  nunchuck->init();
#else
  Log.info("Nunchuck support is DISABLED");
#endif

// Initialize command line
#ifdef ENABLE_SERIAL_COMMANDS
  Log.info("Initializing command line interface...");
  commandLine = new ui::CommandLine(touchscreen, nunchuck, servoValues);
  commandLine->init();
#else
  Log.info("Command line interface is DISABLED");
#endif

  // Do a litlle initialization sequence
  Log.info("Sweeping servos to ensure they are working...");
  for (int pos = SERVO_MIN_ANGLE; pos < core::SERVO_MID_ANGLE; pos += 4)
  {
    for (int i = 0; i < 6; i++)
    {
      servoValues[i] = pos;
    }

    updateServos();
    delay(10);
  }

  // Move to home position
  platform->home(servoValues);
  updateServos();

  Log.info("Initialization complete");
}

void loop()
{
  // Record the start time of this loop iteration
  unsigned long loopStartTime = millis();

// Process command line
#ifdef ENABLE_SERIAL_COMMANDS
  commandLine->process();
#endif

// Process nunchuck input
#ifdef ENABLE_NUNCHUCK
  core::xy_coordf setpoint = nunchuck->process(servoValues);

  // Process the Blinker to handle LED blinking
  stewy::drivers::modeBlinker.loop();
#else
  core::xy_coordf setpoint = core::DEFAULT_SETPOINT;
#endif

// Process touchscreen
#ifdef ENABLE_TOUCHSCREEN
  touchscreen->process(setpoint.x, setpoint.y, servoValues);
#endif

  // Update servos
  updateServos();

  // Calculate how long this iteration took
  unsigned long loopDuration = millis() - loopStartTime;

  // If we completed faster than our target interval, delay the remaining time
  if (loopDuration < MAIN_LOOP_INTERVAL_MS)
  {
    delay(MAIN_LOOP_INTERVAL_MS - loopDuration);
  }
  else if (loopDuration > MAIN_LOOP_INTERVAL_MS)
  {
    // Log a warning if we're exceeding our target loop time
    Log.trace("Loop time exceeded target: %lu ms (target: %d ms)", loopDuration, MAIN_LOOP_INTERVAL_MS);
  }
}

// Clean up function to be called when the program exits
// This is not normally called in Arduino/Teensy, but we include it for completeness
void cleanup()
{
  // Clean up dynamically allocated objects
  if (platform != nullptr)
  {
    delete platform;
    platform = nullptr;
  }

#ifdef ENABLE_TOUCHSCREEN
  if (touchscreen != nullptr)
  {
    delete touchscreen;
    touchscreen = nullptr;
  }
#endif

#ifdef ENABLE_NUNCHUCK
  if (nunchuck != nullptr)
  {
    delete nunchuck;
    nunchuck = nullptr;
  }
#endif

#ifdef ENABLE_SERIAL_COMMANDS
  if (commandLine != nullptr)
  {
    delete commandLine;
    commandLine = nullptr;
  }
#endif

  Log.info("Cleanup complete");
}
