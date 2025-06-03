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
     * and configuring the Stewart platform.
     */
    class CommandLine
    {
    private:
      drivers::TouchScreenDriver *touchscreen;
      drivers::NunchuckDriver *nunchuck;
      float *servoValues;

    public:
      /**
       * @brief Construct a new CommandLine object
       *
       * @param touchscreen Touchscreen driver
       * @param nunchuck Nunchuck driver
       * @param servoValues Servo values array
       */
      CommandLine(drivers::TouchScreenDriver *touchscreen, drivers::NunchuckDriver *nunchuck, float *servoValues);

      /**
       * @brief Initialize the command line interface
       */
      void init();

      /**
       * @brief Process command line input
       */
      void process();

    private:
      // Command handlers
      static int handleHelp(int argc, char **argv);
      static int handleSet(int argc, char **argv);
      static int handleMSet(int argc, char **argv);
      static int handleSetAll(int argc, char **argv);
      static int handleMSetAll(int argc, char **argv);
      static int handleDump(int argc, char **argv);
      static int handleReset(int argc, char **argv);
      static int handleDemo(int argc, char **argv);
      static int handleMoveTo(int argc, char **argv);
      static int handleLog(int argc, char **argv);
      static int handlePID(int argc, char **argv);
      static int handleCalibrateTouchscreen(int argc, char **argv);

      // Shell I/O functions
      static int shellReader(char *data);
      static void shellWriter(char data);
    };

  } // namespace ui
} // namespace stewy
