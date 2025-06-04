/**
 * @file CommandLine.cpp
 * @brief Implementation of the command line interface
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

#include <Shell.h> // Include the Shell.h header first to get the full definition
#include "ui/CommandLine.h"
#include "core/Platform.h"
#include "platform/TeensyHardware.h"
#include <Servo.h>

namespace stewy
{
  namespace ui
  {

    // Static instance for command handlers
    static CommandLine *instance = nullptr;

    CommandLine::CommandLine(drivers::TouchScreenDriver *touchscreen, drivers::NunchuckDriver *nunchuck, float *servoValues)
    {
      this->touchscreen = touchscreen;
      this->nunchuck = nunchuck;
      this->servoValues = servoValues;

      // Store instance for static handlers
      instance = this;
    }

    void CommandLine::init()
    {
      // Initialize shell
      if (shell_init(shellReader, shellWriter, (char *)"Stewy Command Line Interface"))
      {

        // Register commands
        shell_register(handleHelp, "help");
        shell_register(handleHelp, "?");
        shell_register(handleDemo, "demo");
        shell_register(handleDump, "dump");
        shell_register(handleLog, "log");
        shell_register(handleMoveTo, "moveto");
        shell_register(handleMSet, "mset");
        shell_register(handleMSetAll, "msetall");
        shell_register(handleReset, "reset");
        shell_register(handleSet, "set");
        shell_register(handleSetAll, "setall");

#ifdef ENABLE_TOUCHSCREEN
        shell_register(handlePID, "px");
        shell_register(handlePID, "ix");
        shell_register(handlePID, "dx");
        shell_register(handlePID, "py");
        shell_register(handlePID, "iy");
        shell_register(handlePID, "dy");
        shell_register(handleCalibrateTouchscreen, "calibrate");
        shell_register(handleResetPID, "reset-pid");
#endif

        Log.info("Command line interface initialized");
      }
      else
      {
        Log.error("Failed to initialize command line interface");
        return;
      }
    }

    void CommandLine::process()
    {
      // Process shell commands
      shell_task();
    }

    // Shell I/O functions
    int CommandLine::shellReader(char *data)
    {
      // Read from Serial
      if (Serial.available())
      {
        *data = Serial.read();
        return 1;
      }
      return 0;
    }

    void CommandLine::shellWriter(char data)
    {
      // Write to Serial
      Serial.write(data);
    }

    // Command handlers
    int CommandLine::handleHelp(int argc, char **argv)
    {
      // Display help message
      Log.info("Available commands:");

      // This would normally list all commands
      // For now, just print a message
      Log.info("  help, ?, demo, dump, log, moveto, mset, msetall, reset, set, setall");

#ifdef ENABLE_TOUCHSCREEN
      Log.info("  px, ix, dx, py, iy, dy, calibrate");
#endif

      return SHELL_RET_SUCCESS;
    }

    int CommandLine::handleSet(int argc, char **argv)
    {
      if (argc != 3)
      {
        Log.error("Usage: set <servo> <angle>");
        return SHELL_RET_FAILURE;
      }

      int servo = atoi(argv[1]);
      int angle = atoi(argv[2]);

      if (servo < 0 || servo > 5)
      {
        Log.error("Invalid servo number. Must be 0-5.");
        return SHELL_RET_FAILURE;
      }

      if (angle < SERVO_MIN_ANGLE || angle > SERVO_MAX_ANGLE)
      {
        Log.error("Invalid angle. Must be %d-%d.", SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
        return SHELL_RET_FAILURE;
      }

      // Set servo angle
      instance->servoValues[servo] = angle;
      Log.info("Set servo %d to %d degrees", servo, angle);

      return SHELL_RET_SUCCESS;
    }

    int CommandLine::handleMSet(int argc, char **argv)
    {
      if (argc != 3)
      {
        Log.error("Usage: mset <servo> <microseconds>");
        return SHELL_RET_FAILURE;
      }

      int servo = atoi(argv[1]);
      int micros = atoi(argv[2]);

      if (servo < 0 || servo > 5)
      {
        Log.error("Invalid servo number. Must be 0-5.");
        return SHELL_RET_FAILURE;
      }

      if (micros < SERVO_MIN_US || micros > SERVO_MAX_US)
      {
        Log.error("Invalid microseconds. Must be %d-%d.", SERVO_MIN_US, SERVO_MAX_US);
        return SHELL_RET_FAILURE;
      }

      // Convert microseconds to angle
      float angle = map(micros, SERVO_MIN_US, SERVO_MAX_US, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
      instance->servoValues[servo] = angle;
      Log.info("Set servo %d to %d microseconds (%.2f degrees)", servo, micros, angle);

      return SHELL_RET_SUCCESS;
    }

    int CommandLine::handleSetAll(int argc, char **argv)
    {
      if (argc != 2)
      {
        Log.error("Usage: setall <angle>");
        return SHELL_RET_FAILURE;
      }

      int angle = atoi(argv[1]);

      if (angle < SERVO_MIN_ANGLE || angle > SERVO_MAX_ANGLE)
      {
        Log.error("Invalid angle. Must be %d-%d.", SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
        return SHELL_RET_FAILURE;
      }

      // Set all servos to the same angle
      for (int i = 0; i < 6; i++)
      {
        instance->servoValues[i] = angle;
      }

      Log.info("Set all servos to %d degrees", angle);

      return SHELL_RET_SUCCESS;
    }

    int CommandLine::handleMSetAll(int argc, char **argv)
    {
      if (argc != 2)
      {
        Log.error("Usage: msetall <microseconds>");
        return SHELL_RET_FAILURE;
      }

      int micros = atoi(argv[1]);

      if (micros < SERVO_MIN_US || micros > SERVO_MAX_US)
      {
        Log.error("Invalid microseconds. Must be %d-%d.", SERVO_MIN_US, SERVO_MAX_US);
        return SHELL_RET_FAILURE;
      }

      // Convert microseconds to angle
      float angle = map(micros, SERVO_MIN_US, SERVO_MAX_US, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

      // Set all servos to the same angle
      for (int i = 0; i < 6; i++)
      {
        instance->servoValues[i] = angle;
      }

      Log.info("Set all servos to %d microseconds (%.2f degrees)", micros, angle);

      return SHELL_RET_SUCCESS;
    }

    int CommandLine::handleDump(int argc, char **argv)
    {
      // Display system information
      Log.info("System Information:");
      Log.info("  Platform: Teensy");
      Log.info("  Log Level: %d", Log.getLevel());

      // Display servo values
      Log.info("Servo Values:");
      for (int i = 0; i < 6; i++)
      {
        Log.info("  Servo %d: %.2f degrees", i, instance->servoValues[i]);
      }

      // Display platform state
      core::Platform platform(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
      Log.info("Platform State:");
      Log.info("  Sway: %d", platform.getSway());
      Log.info("  Surge: %d", platform.getSurge());
      Log.info("  Heave: %d", platform.getHeave());
      Log.info("  Pitch: %.2f", platform.getPitch());
      Log.info("  Roll: %.2f", platform.getRoll());
      Log.info("  Yaw: %.2f", platform.getYaw());

#ifdef ENABLE_NUNCHUCK
      // Display nunchuck state
      Log.info("Nunchuck State:");
      Log.info("  Mode: %s", instance->nunchuck->getModeString(instance->nunchuck->getMode()));
      Log.info("  SubMode: %s", instance->nunchuck->getSubModeString(instance->nunchuck->getSubMode()));
      Log.info("  Direction: %s", instance->nunchuck->getDirectionString(instance->nunchuck->getDirection()));
#endif

      return SHELL_RET_SUCCESS;
    }

    int CommandLine::handleReset(int argc, char **argv)
    {
      Log.info("Restarting system...");
      delay(100); // Give time for the message to be sent

      // Restart the system
      platform::TeensyHardware::restart();

      return SHELL_RET_SUCCESS; // This will never be reached
    }

    int CommandLine::handleDemo(int argc, char **argv)
    {
      Log.info("Running demo sequence...");

      core::Platform platform(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

      // Home position
      platform.home(instance->servoValues);
      delay(1000);

      // Pitch forward
      Log.info("Pitching forward...");
      platform.moveTo(instance->servoValues, 0, 0, 0, 15, 0, 0);
      delay(1000);

      // Return to home
      platform.home(instance->servoValues);
      delay(500);

      // Roll right
      Log.info("Rolling right...");
      platform.moveTo(instance->servoValues, 0, 0, 0, 0, 15, 0);
      delay(1000);

      // Return to home
      platform.home(instance->servoValues);
      delay(500);

      // Combined pitch and roll
      Log.info("Combined pitch and roll...");
      platform.moveTo(instance->servoValues, 0, 0, 0, 10, 10, 0);
      delay(1000);

      // Return to home
      platform.home(instance->servoValues);
      delay(500);

      // Heave up
      Log.info("Heaving up...");
      platform.moveTo(instance->servoValues, 0, 0, 20, 0, 0, 0);
      delay(1000);

      // Return to home
      platform.home(instance->servoValues);

      Log.info("Demo complete");

      return SHELL_RET_SUCCESS;
    }

    int CommandLine::handleMoveTo(int argc, char **argv)
    {
      if (argc < 3 || argc > 7)
      {
        Log.error("Usage: moveto <pitch> <roll> [sway] [surge] [heave] [yaw]");
        return SHELL_RET_FAILURE;
      }

      float pitch = atof(argv[1]);
      float roll = atof(argv[2]);
      int sway = (argc > 3) ? atoi(argv[3]) : 0;
      int surge = (argc > 4) ? atoi(argv[4]) : 0;
      int heave = (argc > 5) ? atoi(argv[5]) : 0;
      float yaw = (argc > 6) ? atof(argv[6]) : 0;

      core::Platform platform(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

      if (argc <= 3)
      {
        // Just pitch and roll
        if (!platform.moveTo(instance->servoValues, pitch, roll))
        {
          Log.error("Failed to move platform");
          return SHELL_RET_FAILURE;
        }
      }
      else
      {
        // Full 6DOF movement
        if (!platform.moveTo(instance->servoValues, sway, surge, heave, pitch, roll, yaw))
        {
          Log.error("Failed to move platform");
          return SHELL_RET_FAILURE;
        }
      }

      Log.info("Platform moved to pitch=%.2f, roll=%.2f, sway=%d, surge=%d, heave=%d, yaw=%.2f",
               pitch, roll, sway, surge, heave, yaw);

      return SHELL_RET_SUCCESS;
    }

    int CommandLine::handleLog(int argc, char **argv)
    {
      if (argc != 2)
      {
        Log.info("Usage: log [SILENT | VERBOSE | TRACE | INFO | WARNING | ERROR | FATAL]");
        return SHELL_RET_FAILURE;
      }

      char *level = argv[1];

      if (strcmp(level, "SILENT") == 0)
      {
        Log.setLevel(LOG_LEVEL_SILENT);
      }
      else if (strcmp(level, "VERBOSE") == 0)
      {
        Log.setLevel(LOG_LEVEL_VERBOSE);
      }
      else if (strcmp(level, "TRACE") == 0)
      {
        Log.setLevel(LOG_LEVEL_TRACE);
      }
      else if (strcmp(level, "INFO") == 0)
      {
        Log.setLevel(LOG_LEVEL_INFO);
      }
      else if (strcmp(level, "WARNING") == 0)
      {
        Log.setLevel(LOG_LEVEL_WARNING);
      }
      else if (strcmp(level, "ERROR") == 0)
      {
        Log.setLevel(LOG_LEVEL_ERROR);
      }
      else if (strcmp(level, "FATAL") == 0)
      {
        Log.setLevel(LOG_LEVEL_FATAL);
      }
      else
      {
        Log.error("Invalid log level: %s", level);
        return SHELL_RET_FAILURE;
      }

      Log.info("Log level set to %s", level);

      return SHELL_RET_SUCCESS;
    }

    int CommandLine::handlePID(int argc, char **argv)
    {
#ifdef ENABLE_TOUCHSCREEN
      if (argc != 2)
      {
        Log.error("Usage: %s <value>", argv[0]);
        return SHELL_RET_FAILURE;
      }

      float value = atof(argv[1]);
      char axis = 'x';  // Default to X axis
      char param = 'p'; // Default to P parameter

      // Parse command to determine axis and parameter
      if (strlen(argv[0]) == 2)
      {
        param = argv[0][0]; // p, i, or d
        axis = argv[0][1];  // x or y
      }

      // Get current PID values
      double p, i, d;
      instance->touchscreen->getPID(axis, p, i, d);

      // Update the appropriate parameter
      switch (param)
      {
      case 'p':
        p = value;
        break;
      case 'i':
        i = value;
        break;
      case 'd':
        d = value;
        break;
      default:
        Log.error("Invalid parameter: %c", param);
        return SHELL_RET_FAILURE;
      }

      // Set the new PID values
      instance->touchscreen->setPID(axis, p, i, d);

      Log.info("%c-axis PID values: P=%.2f, I=%.2f, D=%.2f", toupper(axis), p, i, d);

      return SHELL_RET_SUCCESS;
#else
      Log.error("Touchscreen support is not enabled");
      return SHELL_RET_FAILURE;
#endif
    }

    int CommandLine::handleCalibrateTouchscreen(int argc, char **argv)
    {
#ifdef ENABLE_TOUCHSCREEN
      Log.info("Starting touchscreen calibration...");
      instance->touchscreen->startCalibration();
      return SHELL_RET_SUCCESS;
#else
      Log.error("Touchscreen support is not enabled");
      return SHELL_RET_FAILURE;
#endif
    }

    int CommandLine::handleResetPID(int argc, char **argv)
    {
#ifdef ENABLE_TOUCHSCREEN
      Log.info("Resetting PID controllers to default values...");
      instance->touchscreen->resetPID();
      return SHELL_RET_SUCCESS;
#else
      Log.error("Touchscreen support is not enabled");
      return SHELL_RET_FAILURE;
#endif
    }

  } // namespace ui
} // namespace stewy
