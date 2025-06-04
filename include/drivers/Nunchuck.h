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
     * 
     * Defines the different operating modes for the platform,
     * each with different behavior for the setpoint and controls.
     */
    enum ControlMode
    {
      SETPOINT, ///< Setpoint position is in the middle of the platform, joystick moves the setpoint
      CONTROL,  ///< X/Y position of the wiichuck directly controls the position of the platform
      CIRCLE,   ///< Setpoint position moves in a circle, joystick controls speed
      EIGHT,    ///< Setpoint position moves in a figure eight, joystick controls speed
      SQUARE    ///< Setpoint cycles through corners of a square, joystick moves within current quadrant
    };

    /**
     * @enum ControlSubMode
     * @brief Sub-modes for CONTROL mode
     * 
     * Defines the different sub-modes available in CONTROL mode,
     * each mapping the joystick to different platform movements.
     */
    enum ControlSubMode
    {
      PITCH_ROLL, ///< Joystick controls the pitch and roll angles of the platform
      HEAVE_YAW,  ///< Joystick Y axis controls the up-down movement, X axis controls rotation
      SWAY_SURGE  ///< Joystick X axis controls sway (left-right), Y axis controls surge (forward-backward)
    };

    /**
     * @enum Direction
     * @brief Direction for movement modes
     * 
     * Defines the direction of movement for modes that involve
     * continuous movement (CIRCLE, EIGHT).
     */
    enum Direction
    {
      CW,  ///< Clockwise movement
      CCW  ///< Counter-clockwise movement
    };

    /**
     * @class NunchuckDriver
     * @brief Driver for Wii Nunchuck controller
     *
     * This class provides an interface to the Wii Nunchuck controller,
     * including button handling, mode management, and platform control.
     * It interprets input from the Nunchuck and translates it into
     * appropriate platform movements based on the current mode.
     */
    class NunchuckDriver
    {
    private:
      Accessory *nunchuck;      ///< Pointer to the Nunchuck accessory object
      ControlMode mode;         ///< Current control mode
      ControlSubMode subMode;   ///< Current sub-mode (for CONTROL mode)
      Direction direction;      ///< Current movement direction (for CIRCLE, EIGHT modes)

      float speed;              ///< Movement speed for CIRCLE and EIGHT modes
      float radius;             ///< Circle radius for CIRCLE mode

      unsigned long lastButtonTime; ///< Timestamp of the last button press (for double-click detection)
      bool zPressed;            ///< Flag indicating if Z button is currently pressed
      bool cPressed;            ///< Flag indicating if C button is currently pressed

      core::xy_coordf deadBand; ///< Joystick deadband to prevent drift
      core::xy_coordf setpoint; ///< Current setpoint position

    public:
      /**
       * @brief Construct a new NunchuckDriver object
       * 
       * Initializes the Nunchuck driver with default settings:
       * - Mode: SETPOINT
       * - Sub-mode: PITCH_ROLL
       * - Direction: CW (clockwise)
       * - Speed: 0.2
       * - Joystick deadband: 2 units in both axes
       * - Setpoint: (0,0) (center of platform)
       */
      NunchuckDriver();

      /**
       * @brief Destructor for NunchuckDriver
       * 
       * Properly cleans up dynamically allocated objects (Accessory).
       */
      ~NunchuckDriver();

      /**
       * @brief Initialize the Nunchuck
       * 
       * Initializes communication with the Nunchuck controller.
       * Logs an error if the Nunchuck is not detected.
       */
      void init();

      /**
       * @brief Process Nunchuck input
       *
       * Reads input from the Nunchuck, handles button presses,
       * updates the setpoint based on the current mode, and
       * applies platform movements accordingly.
       *
       * @param servoValues Pointer to an array of 6 floats where servo values will be updated
       * @return core::xy_coordf Current setpoint position (-1.0 to 1.0 in both axes)
       * 
       * @note In SETPOINT mode, the touchscreen driver uses the returned setpoint to control the platform.
       * @note In CONTROL mode, this method directly updates the servo values based on joystick input.
       */
      core::xy_coordf process(float *servoValues);

      /**
       * @brief Get current control mode
       *
       * @return ControlMode Current mode (SETPOINT, CONTROL, CIRCLE, EIGHT, or SQUARE)
       */
      ControlMode getMode();

      /**
       * @brief Get current control sub-mode
       *
       * @return ControlSubMode Current sub-mode (PITCH_ROLL, HEAVE_YAW, or SWAY_SURGE)
       */
      ControlSubMode getSubMode();

      /**
       * @brief Get current direction
       *
       * @return Direction Current direction (CW or CCW)
       */
      Direction getDirection();

      /**
       * @brief Get mode name as string
       *
       * @param mode Mode to get name for
       * @return const char* Mode name as a string ("SETPOINT", "CONTROL", etc.)
       */
      const char *getModeString(ControlMode mode);

      /**
       * @brief Get sub-mode name as string
       *
       * @param subMode Sub-mode to get name for
       * @return const char* Sub-mode name as a string ("PITCH_ROLL", "HEAVE_YAW", etc.)
       */
      const char *getSubModeString(ControlSubMode subMode);

      /**
       * @brief Get direction name as string
       *
       * @param dir Direction to get name for
       * @return const char* Direction name as a string ("CW" or "CCW")
       */
      const char *getDirectionString(Direction dir);

    private:
      /**
       * @brief Handle button presses
       * 
       * Processes Z and C button presses based on the current mode:
       * - Z button: Reset setpoint (SETPOINT), cycle sub-modes (CONTROL), stop/start movement (CIRCLE/EIGHT), move to next corner (SQUARE)
       * - C button: Cycle through modes, double-click to reverse direction (CIRCLE/EIGHT/SQUARE)
       * 
       * Also handles LED blinking to indicate the current mode.
       */
      void handleButtons();
      
      /**
       * @brief Update setpoint based on current mode
       * 
       * Updates the setpoint position based on the current mode and joystick input:
       * - SETPOINT: Joystick moves the setpoint
       * - CIRCLE: Setpoint moves in a circle, joystick controls speed
       * - EIGHT: Setpoint moves in a figure eight, joystick controls speed
       * - SQUARE: Joystick moves within current quadrant
       */
      void updateSetpoint();
      
      /**
       * @brief Check for double-click
       * 
       * Determines if a button press is part of a double-click based on timing.
       * 
       * @param time Current timestamp in milliseconds
       * @return true if the current press is the second press of a double-click
       * @return false if the current press is not part of a double-click
       */
      bool isDoubleClick(unsigned long time);
    };

  } // namespace drivers
} // namespace stewy
