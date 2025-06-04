#pragma once
/**
 * @file CommandLine.h
 * @brief Command line interface
 *
 * This file contains the command line interface functionality
 * for the Stewart platform.
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

// Forward declare the shell functions from the GeekFactory Shell Library
extern "C"
{
  bool shell_init(int (*reader)(char *), void (*writer)(char), char *msg);
  bool shell_register(int (*program)(int, char **), const char *string);
  void shell_task(void);
}

#include "core/Config.h"
#include "drivers/TouchScreen.h"
#include "drivers/Nunchuck.h"

namespace stewy
{
  namespace ui
  {

    /**
     * @class CommandLine
     * @brief Command line interface handler
     *
     * This class provides a command line interface for controlling
     * and configuring the Stewart platform. It uses the GeekFactory Shell
     * Library to process commands from the serial interface and execute
     * the appropriate actions.
     */
    class CommandLine
    {
    private:
      drivers::TouchScreenDriver *touchscreen; ///< Pointer to the touchscreen driver
      drivers::NunchuckDriver *nunchuck;       ///< Pointer to the nunchuck driver
      float *servoValues;                      ///< Pointer to the servo values array

    public:
      /**
       * @brief Construct a new CommandLine object
       *
       * Initializes the command line interface with references to the
       * touchscreen driver, nunchuck driver, and servo values array.
       * Also sets the static instance pointer for use in command handlers.
       *
       * @param touchscreen Pointer to the touchscreen driver
       * @param nunchuck Pointer to the nunchuck driver
       * @param servoValues Pointer to the servo values array (6 elements)
       */
      CommandLine(drivers::TouchScreenDriver *touchscreen, drivers::NunchuckDriver *nunchuck, float *servoValues);

      /**
       * @brief Destructor for CommandLine object
       *
       * Note: This class doesn't own the objects passed to it, so it doesn't delete them.
       * The pointers are managed by the main application.
       */
      ~CommandLine() = default;

      /**
       * @brief Initialize the command line interface
       *
       * Initializes the shell and registers all command handlers.
       * If initialization fails, an error is logged.
       */
      void init();

      /**
       * @brief Process command line input
       *
       * Processes any pending command line input by calling the shell_task function.
       * This method should be called regularly in the main loop.
       */
      void process();

    private:
      /**
       * @brief Display help information
       *
       * Displays a list of available commands.
       *
       * @param argc Number of arguments
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success
       */
      static int handleHelp(int argc, char **argv);

      /**
       * @brief Set a single servo angle
       *
       * Sets the angle of a specific servo.
       * Usage: set <servo> <angle>
       *
       * @param argc Number of arguments (must be 3)
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success, SHELL_RET_FAILURE on failure
       */
      static int handleSet(int argc, char **argv);

      /**
       * @brief Set a single servo microseconds
       *
       * Sets the pulse width in microseconds for a specific servo.
       * Usage: mset <servo> <microseconds>
       *
       * @param argc Number of arguments (must be 3)
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success, SHELL_RET_FAILURE on failure
       */
      static int handleMSet(int argc, char **argv);

      /**
       * @brief Set all servos to the same angle
       *
       * Sets all servos to the specified angle.
       * Usage: setall <angle>
       *
       * @param argc Number of arguments (must be 2)
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success, SHELL_RET_FAILURE on failure
       */
      static int handleSetAll(int argc, char **argv);

      /**
       * @brief Set all servos to the same microseconds
       *
       * Sets all servos to the specified pulse width in microseconds.
       * Usage: msetall <microseconds>
       *
       * @param argc Number of arguments (must be 2)
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success, SHELL_RET_FAILURE on failure
       */
      static int handleMSetAll(int argc, char **argv);

      /**
       * @brief Display system information
       *
       * Displays information about the system state, including servo values,
       * platform position, and nunchuck state.
       *
       * @param argc Number of arguments
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success
       */
      static int handleDump(int argc, char **argv);

      /**
       * @brief Restart the system
       *
       * Restarts the Teensy microcontroller.
       *
       * @param argc Number of arguments
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success (never reached due to restart)
       */
      static int handleReset(int argc, char **argv);

      /**
       * @brief Run a demo sequence
       *
       * Runs a demonstration sequence of platform movements.
       *
       * @param argc Number of arguments
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success
       */
      static int handleDemo(int argc, char **argv);

      /**
       * @brief Move platform to specified position
       *
       * Moves the platform to the specified position and orientation.
       * Usage: moveto <pitch> <roll> [sway] [surge] [heave] [yaw]
       *
       * @param argc Number of arguments (3-7)
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success, SHELL_RET_FAILURE on failure
       */
      static int handleMoveTo(int argc, char **argv);

      /**
       * @brief Set log level
       *
       * Sets the log level for the ArduinoLog library.
       * Usage: log [SILENT | VERBOSE | TRACE | INFO | WARNING | ERROR | FATAL]
       *
       * @param argc Number of arguments (must be 2)
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success, SHELL_RET_FAILURE on failure
       */
      static int handleLog(int argc, char **argv);

      /**
       * @brief Set PID parameters
       *
       * Sets the PID parameters for the touchscreen controller.
       * Usage: px|ix|dx|py|iy|dy <value>
       *
       * @param argc Number of arguments (must be 2)
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success, SHELL_RET_FAILURE on failure
       */
      static int handlePID(int argc, char **argv);

      /**
       * @brief Start touchscreen calibration
       *
       * Starts the touchscreen calibration process.
       *
       * @param argc Number of arguments
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success, SHELL_RET_FAILURE on failure
       */
      static int handleCalibrateTouchscreen(int argc, char **argv);

      /**
       * @brief Reset PID controllers
       *
       * Resets the PID controllers to their default values.
       *
       * @param argc Number of arguments
       * @param argv Array of argument strings
       * @return SHELL_RET_SUCCESS on success, SHELL_RET_FAILURE on failure
       */
      static int handleResetPID(int argc, char **argv);

      /**
       * @brief Read a character from the serial interface
       *
       * Callback function for the shell library to read input.
       *
       * @param data Pointer to store the read character
       * @return 1 if a character was read, 0 if no character is available
       */
      static int shellReader(char *data);

      /**
       * @brief Write a character to the serial interface
       *
       * Callback function for the shell library to write output.
       *
       * @param data Character to write
       */
      static void shellWriter(char data);
    };

  } // namespace ui
} // namespace stewy
