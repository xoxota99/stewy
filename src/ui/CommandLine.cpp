/**
 * @file CommandLine.cpp
 * @brief Implementation of the command line interface
 * 
 * @author Philippe Desrosiers
 * @copyright Copyright (C) 2018 Philippe Desrosiers
 * @license GPL-3.0
 */

#include "ui/CommandLine.h"
#include "core/Platform.h"
#include "platform/TeensyHardware.h"
#include <Servo.h>

namespace stewy {
namespace ui {

// Static instance for command handlers
static CommandLine* instance = nullptr;

CommandLine::CommandLine(drivers::TouchScreenDriver* touchscreen, drivers::NunchuckDriver* nunchuck, float* servoValues) {
  this->touchscreen = touchscreen;
  this->nunchuck = nunchuck;
  this->servoValues = servoValues;
  
  // Create shell instance
  shell = new Shell();
  
  // Store instance for static handlers
  instance = this;
}

void CommandLine::init() {
  // Initialize shell
  shell->attach_shell(shellReader, shellWriter);
  
  // Register commands
  shell->register_command(handleHelp, "help", "This message.");
  shell->register_command(handleHelp, "?", "Synonym for 'help'");
  shell->register_command(handleDemo, "demo", "Do a little dance.");
  shell->register_command(handleDump, "dump", "Display information about the system.");
  shell->register_command(handleLog, "log", "Set the log level");
  shell->register_command(handleMoveTo, "moveto", "Move the platform to the specified pitch / roll (in degrees).");
  shell->register_command(handleMSet, "mset", "Set a specific servo to a specific angle (in microseconds).");
  shell->register_command(handleMSetAll, "msetall", "Set all servos to a specific angle (in microseconds).");
  shell->register_command(handleReset, "reset", "Restart the system.");
  shell->register_command(handleSet, "set", "Set a specific servo to a specific angle (in degrees).");
  shell->register_command(handleSetAll, "setall", "Set all servos to a specific angle (in degrees).");
  
#ifdef ENABLE_TOUCHSCREEN
  shell->register_command(handlePID, "px", "Set P value for X-axis (roll).");
  shell->register_command(handlePID, "ix", "Set I value for X-axis (roll).");
  shell->register_command(handlePID, "dx", "Set D value for X-axis (roll).");
  shell->register_command(handlePID, "py", "Set P value for Y-axis (pitch).");
  shell->register_command(handlePID, "iy", "Set I value for Y-axis (pitch).");
  shell->register_command(handlePID, "dy", "Set D value for Y-axis (pitch).");
  shell->register_command(handleCalibrateTouchscreen, "calibrate", "Start touchscreen calibration process.");
#endif
  
  Log.notice("Command line interface initialized");
}

void CommandLine::process() {
  // Process shell commands
  shell->task();
}

// Shell I/O functions
int CommandLine::shellReader(char* data) {
  // Read from Serial
  if (Serial.available()) {
    *data = Serial.read();
    return 1;
  }
  return 0;
}

void CommandLine::shellWriter(char data) {
  // Write to Serial
  Serial.write(data);
}

// Command handlers
int CommandLine::handleHelp(int argc, char** argv) {
  // Display help message
  Log.notice("Available commands:");
  
  // This would normally list all commands
  // For now, just print a message
  Log.notice("  help, ?, demo, dump, log, moveto, mset, msetall, reset, set, setall");
  
#ifdef ENABLE_TOUCHSCREEN
  Log.notice("  px, ix, dx, py, iy, dy, calibrate");
#endif
  
  return SHELL_RET_SUCCESS;
}

int CommandLine::handleSet(int argc, char** argv) {
  if (argc != 3) {
    Log.error("Usage: set <servo> <angle>");
    return SHELL_RET_FAILURE;
  }
  
  int servo = atoi(argv[1]);
  int angle = atoi(argv[2]);
  
  if (servo < 0 || servo > 5) {
    Log.error("Invalid servo number. Must be 0-5.");
    return SHELL_RET_FAILURE;
  }
  
  if (angle < SERVO_MIN_ANGLE || angle > SERVO_MAX_ANGLE) {
    Log.error("Invalid angle. Must be %d-%d.", SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
    return SHELL_RET_FAILURE;
  }
  
  // Set servo angle
  instance->servoValues[servo] = angle;
  Log.notice("Set servo %d to %d degrees", servo, angle);
  
  return SHELL_RET_SUCCESS;
}

int CommandLine::handleMSet(int argc, char** argv) {
  if (argc != 3) {
    Log.error("Usage: mset <servo> <microseconds>");
    return SHELL_RET_FAILURE;
  }
  
  int servo = atoi(argv[1]);
  int micros = atoi(argv[2]);
  
  if (servo < 0 || servo > 5) {
    Log.error("Invalid servo number. Must be 0-5.");
    return SHELL_RET_FAILURE;
  }
  
  if (micros < SERVO_MIN_US || micros > SERVO_MAX_US) {
    Log.error("Invalid microseconds. Must be %d-%d.", SERVO_MIN_US, SERVO_MAX_US);
    return SHELL_RET_FAILURE;
  }
  
  // Convert microseconds to angle
  float angle = map(micros, SERVO_MIN_US, SERVO_MAX_US, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
  instance->servoValues[servo] = angle;
  Log.notice("Set servo %d to %d microseconds (%.2f degrees)", servo, micros, angle);
  
  return SHELL_RET_SUCCESS;
}

int CommandLine::handleSetAll(int argc, char** argv) {
  if (argc != 2) {
    Log.error("Usage: setall <angle>");
    return SHELL_RET_FAILURE;
  }
  
  int angle = atoi(argv[1]);
  
  if (angle < SERVO_MIN_ANGLE || angle > SERVO_MAX_ANGLE) {
    Log.error("Invalid angle. Must be %d-%d.", SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
    return SHELL_RET_FAILURE;
  }
  
  // Set all servos to the same angle
  for (int i = 0; i < 6; i++) {
    instance->servoValues[i] = angle;
  }
  
  Log.notice("Set all servos to %d degrees", angle);
  
  return SHELL_RET_SUCCESS;
}

int CommandLine::handleMSetAll(int argc, char** argv) {
  if (argc != 2) {
    Log.error("Usage: msetall <microseconds>");
    return SHELL_RET_FAILURE;
  }
  
  int micros = atoi(argv[1]);
  
  if (micros < SERVO_MIN_US || micros > SERVO_MAX_US) {
    Log.error("Invalid microseconds. Must be %d-%d.", SERVO_MIN_US, SERVO_MAX_US);
    return SHELL_RET_FAILURE;
  }
  
  // Convert microseconds to angle
  float angle = map(micros, SERVO_MIN_US, SERVO_MAX_US, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
  
  // Set all servos to the same angle
  for (int i = 0; i < 6; i++) {
    instance->servoValues[i] = angle;
  }
  
  Log.notice("Set all servos to %d microseconds (%.2f degrees)", micros, angle);
  
  return SHELL_RET_SUCCESS;
}

int CommandLine::handleDump(int argc, char** argv) {
  // Display system information
  Log.notice("System Information:");
  Log.notice("  Platform: Teensy");
  Log.notice("  Log Level: %d", Log.getLevel());
  
  // Display servo values
  Log.notice("Servo Values:");
  for (int i = 0; i < 6; i++) {
    Log.notice("  Servo %d: %.2f degrees", i, instance->servoValues[i]);
  }
  
  // Display platform state
  core::Platform platform(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
  Log.notice("Platform State:");
  Log.notice("  Sway: %d", platform.getSway());
  Log.notice("  Surge: %d", platform.getSurge());
  Log.notice("  Heave: %d", platform.getHeave());
  Log.notice("  Pitch: %.2f", platform.getPitch());
  Log.notice("  Roll: %.2f", platform.getRoll());
  Log.notice("  Yaw: %.2f", platform.getYaw());
  
#ifdef ENABLE_NUNCHUCK
  // Display nunchuck state
  Log.notice("Nunchuck State:");
  Log.notice("  Mode: %s", instance->nunchuck->getModeString(instance->nunchuck->getMode()));
  Log.notice("  SubMode: %s", instance->nunchuck->getSubModeString(instance->nunchuck->getSubMode()));
  Log.notice("  Direction: %s", instance->nunchuck->getDirectionString(instance->nunchuck->getDirection()));
#endif
  
  return SHELL_RET_SUCCESS;
}

int CommandLine::handleReset(int argc, char** argv) {
  Log.notice("Restarting system...");
  delay(100); // Give time for the message to be sent
  
  // Restart the system
  platform::TeensyHardware::restart();
  
  return SHELL_RET_SUCCESS; // This will never be reached
}

int CommandLine::handleDemo(int argc, char** argv) {
  Log.notice("Running demo sequence...");
  
  core::Platform platform(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
  
  // Home position
  platform.home(instance->servoValues);
  delay(1000);
  
  // Pitch forward
  Log.notice("Pitching forward...");
  platform.moveTo(instance->servoValues, 0, 0, 0, 15, 0, 0);
  delay(1000);
  
  // Return to home
  platform.home(instance->servoValues);
  delay(500);
  
  // Roll right
  Log.notice("Rolling right...");
  platform.moveTo(instance->servoValues, 0, 0, 0, 0, 15, 0);
  delay(1000);
  
  // Return to home
  platform.home(instance->servoValues);
  delay(500);
  
  // Combined pitch and roll
  Log.notice("Combined pitch and roll...");
  platform.moveTo(instance->servoValues, 0, 0, 0, 10, 10, 0);
  delay(1000);
  
  // Return to home
  platform.home(instance->servoValues);
  delay(500);
  
  // Heave up
  Log.notice("Heaving up...");
  platform.moveTo(instance->servoValues, 0, 0, 20, 0, 0, 0);
  delay(1000);
  
  // Return to home
  platform.home(instance->servoValues);
  
  Log.notice("Demo complete");
  
  return SHELL_RET_SUCCESS;
}

int CommandLine::handleMoveTo(int argc, char** argv) {
  if (argc < 3 || argc > 7) {
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
  
  if (argc <= 3) {
    // Just pitch and roll
    if (!platform.moveTo(instance->servoValues, pitch, roll)) {
      Log.error("Failed to move platform");
      return SHELL_RET_FAILURE;
    }
  } else {
    // Full 6DOF movement
    if (!platform.moveTo(instance->servoValues, sway, surge, heave, pitch, roll, yaw)) {
      Log.error("Failed to move platform");
      return SHELL_RET_FAILURE;
    }
  }
  
  Log.notice("Platform moved to pitch=%.2f, roll=%.2f, sway=%d, surge=%d, heave=%d, yaw=%.2f",
             pitch, roll, sway, surge, heave, yaw);
  
  return SHELL_RET_SUCCESS;
}

int CommandLine::handleLog(int argc, char** argv) {
  if (argc != 2) {
    Log.notice("Usage: log [SILENT | VERBOSE | TRACE | INFO | WARNING | ERROR | FATAL]");
    return SHELL_RET_FAILURE;
  }
  
  char* level = argv[1];
  
  if (strcmp(level, "SILENT") == 0) {
    Log.setLevel(LOG_LEVEL_SILENT);
  } else if (strcmp(level, "VERBOSE") == 0) {
    Log.setLevel(LOG_LEVEL_VERBOSE);
  } else if (strcmp(level, "TRACE") == 0) {
    Log.setLevel(LOG_LEVEL_TRACE);
  } else if (strcmp(level, "INFO") == 0) {
    Log.setLevel(LOG_LEVEL_INFO);
  } else if (strcmp(level, "WARNING") == 0) {
    Log.setLevel(LOG_LEVEL_WARNING);
  } else if (strcmp(level, "ERROR") == 0) {
    Log.setLevel(LOG_LEVEL_ERROR);
  } else if (strcmp(level, "FATAL") == 0) {
    Log.setLevel(LOG_LEVEL_FATAL);
  } else {
    Log.error("Invalid log level: %s", level);
    return SHELL_RET_FAILURE;
  }
  
  Log.notice("Log level set to %s", level);
  
  return SHELL_RET_SUCCESS;
}

int CommandLine::handlePID(int argc, char** argv) {
#ifdef ENABLE_TOUCHSCREEN
  if (argc != 2) {
    Log.error("Usage: %s <value>", argv[0]);
    return SHELL_RET_FAILURE;
  }
  
  float value = atof(argv[1]);
  char axis = 'x'; // Default to X axis
  char param = 'p'; // Default to P parameter
  
  // Parse command to determine axis and parameter
  if (strlen(argv[0]) == 2) {
    param = argv[0][0]; // p, i, or d
    axis = argv[0][1]; // x or y
  }
  
  // Get current PID values
  double p, i, d;
  instance->touchscreen->getPID(axis, p, i, d);
  
  // Update the appropriate parameter
  switch (param) {
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
  
  Log.notice("%c-axis PID values: P=%.2f, I=%.2f, D=%.2f", toupper(axis), p, i, d);
  
  return SHELL_RET_SUCCESS;
#else
  Log.error("Touchscreen support is not enabled");
  return SHELL_RET_FAILURE;
#endif
}

int CommandLine::handleCalibrateTouchscreen(int argc, char** argv) {
#ifdef ENABLE_TOUCHSCREEN
  Log.notice("Starting touchscreen calibration...");
  instance->touchscreen->startCalibration();
  return SHELL_RET_SUCCESS;
#else
  Log.error("Touchscreen support is not enabled");
  return SHELL_RET_FAILURE;
#endif
}

} // namespace ui
} // namespace stewy
