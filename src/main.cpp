/**
 * @file main.cpp
 * @brief Main entry point for the Stewart platform project
 *
 * This file contains the setup and loop functions required by Arduino/Teensy.
 *
 * @author Philippe Desrosiers
 * @copyright Copyright (C) 2018 Philippe Desrosiers
 * @license GPL-3.0
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
    // Apply reverse if needed
    float val = servoValues[i];
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
  Log.notice("Stewy Platform Starting...");
  Log.notice("Built %s, %s", __DATE__, __TIME__);

  // Initialize platform
  platform = new core::Platform(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

// Initialize servos
#ifdef ENABLE_SERVOS
  Log.notice("Initializing servos...");
  for (int i = 0; i < 6; i++)
  {
    servos[i].attach(core::SERVO_PINS[i]);
  }
#else
  Log.notice("Servo control is DISABLED");
#endif

// Initialize touchscreen
#ifdef ENABLE_TOUCHSCREEN
  Log.notice("Initializing touchscreen...");
  touchscreen = new drivers::TouchScreenDriver(XP, YP, XM, YM, TS_OHMS);
  touchscreen->init();
#else
  Log.notice("Touchscreen support is DISABLED");
#endif

// Initialize nunchuck
#ifdef ENABLE_NUNCHUCK
  Log.notice("Initializing nunchuck...");
  nunchuck = new drivers::NunchuckDriver();
  nunchuck->init();
#else
  Log.notice("Nunchuck support is DISABLED");
#endif

// Initialize command line
#ifdef ENABLE_SERIAL_COMMANDS
  Log.notice("Initializing command line interface...");
  commandLine = new ui::CommandLine(touchscreen, nunchuck, servoValues);
  commandLine->init();
#else
  Log.notice("Command line interface is DISABLED");
#endif

  // Do a litlle initialization sequence
  Log.notice("Sweeping servos to ensure they are working...");
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

  Log.notice("Initialization complete");
}

void loop()
{
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
}
