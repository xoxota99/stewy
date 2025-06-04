#pragma once
/**
 * @file Nunchuck.h
 * @brief Wii Nunchuck driver
 *
 * This file contains the Wii Nunchuck driver and control mode
 * functionality for the Stewart platform.
 *
 * @author Philippe Desrosiers
 * @copyright Copyright (C) 2018 Philippe Desrosiers
 * @license GPL-3.0
 */

#include <WiiChuck.h> // https://github.com/madhephaestus/WiiChuck.git
#include <Blinker.h>  // Blinker for LED indication
#include "core/Config.h"

namespace stewy
{
  namespace drivers
  {

    // Global Blinker instance for LED indication
    extern Blinker modeBlinker;

    /**
     * @enum ControlMode
     * @brief Control modes for the platform
     */
    enum ControlMode
    {
      SETPOINT, // setpoint position is in the middle of the platform
      CONTROL,  // X/Y position of the wiichuck directly controls the position of the platform
      CIRCLE,   // setpoint position moves in a circle
      EIGHT,    // setpoint position moves in a figure eight
      SQUARE    // setpoint cycles through corners of a square
    };

    /**
     * @enum ControlSubMode
     * @brief Sub-modes for CONTROL mode
     */
    enum ControlSubMode
    {
      PITCH_ROLL, // Joystick controls the angle of the platform
      HEAVE_YAW,  // Joystick Y axis controls the up-down movement, X axis controls rotation
      SWAY_SURGE  // Joystick X axis controls sway, Y axis controls surge
    };

    /**
     * @enum Direction
     * @brief Direction for movement modes
     */
    enum Direction
    {
      CW, // Clockwise
      CCW // Counter-clockwise
    };

    /**
     * @class NunchuckDriver
     * @brief Driver for Wii Nunchuck controller
     *
     * This class provides an interface to the Wii Nunchuck controller,
     * including button handling and mode management.
     */
    class NunchuckDriver
    {
    private:
      Accessory *nunchuck;
      ControlMode mode;
      ControlSubMode subMode;
      Direction direction;

      float speed;
      float radius;

      unsigned long lastButtonTime;
      bool zPressed;
      bool cPressed;

      core::xy_coordf deadBand;
      core::xy_coordf setpoint;

    public:
      /**
       * @brief Construct a new NunchuckDriver object
       */
      NunchuckDriver();

      /**
       * @brief Destructor for NunchuckDriver
       * 
       * Properly cleans up dynamically allocated objects
       */
      ~NunchuckDriver();

      /**
       * @brief Initialize the Nunchuck
       */
      void init();

      /**
       * @brief Process Nunchuck input
       *
       * @param servoValues Array to store calculated servo values
       * @return core::xy_coordf Current setpoint
       */
      core::xy_coordf process(float *servoValues);

      /**
       * @brief Get current control mode
       *
       * @return ControlMode Current mode
       */
      ControlMode getMode();

      /**
       * @brief Get current control sub-mode
       *
       * @return ControlSubMode Current sub-mode
       */
      ControlSubMode getSubMode();

      /**
       * @brief Get current direction
       *
       * @return Direction Current direction
       */
      Direction getDirection();

      /**
       * @brief Get mode name as string
       *
       * @param mode Mode to get name for
       * @return const char* Mode name
       */
      const char *getModeString(ControlMode mode);

      /**
       * @brief Get sub-mode name as string
       *
       * @param subMode Sub-mode to get name for
       * @return const char* Sub-mode name
       */
      const char *getSubModeString(ControlSubMode subMode);

      /**
       * @brief Get direction name as string
       *
       * @param dir Direction to get name for
       * @return const char* Direction name
       */
      const char *getDirectionString(Direction dir);

    private:
      void handleButtons();
      void updateSetpoint();
      bool isDoubleClick(unsigned long time);
    };

  } // namespace drivers
} // namespace stewy
