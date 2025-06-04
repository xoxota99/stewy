/**
 * @file Nunchuck.cpp
 * @brief Implementation of the Wii Nunchuck driver
 *
 * @author Philippe Desrosiers
 * @copyright Copyright (C) 2018 Philippe Desrosiers
 * @license GPL-3.0
 */

#include "drivers/Nunchuck.h"
#include "core/Platform.h"
#include <Blinker.h>

namespace stewy
{
  namespace drivers
  {
    // Initialize the global Blinker instance
    Blinker modeBlinker = Blinker::attach(LED_PIN, false, 200, 200);

    // Mode, submode, and direction string arrays
    const char *MODE_STRINGS[] = {
        "SETPOINT",
        "CONTROL",
        "CIRCLE",
        "EIGHT",
        "SQUARE"};

    const char *SUBMODE_STRINGS[] = {
        "PITCH_ROLL",
        "HEAVE_YAW",
        "SWAY_SURGE"};

    const char *DIRECTION_STRINGS[] = {
        "CW",
        "CCW"};

    NunchuckDriver::NunchuckDriver()
    {
      nunchuck = new Accessory();
      mode = SETPOINT;
      subMode = PITCH_ROLL;
      direction = CW;
      speed = 0.2f;
      radius = 0.0f;
      lastButtonTime = 0;
      zPressed = false;
      cPressed = false;

      // Set deadband for joystick
      deadBand.x = 2;
      deadBand.y = 2;

      // Initialize setpoint to center
      setpoint.x = 0;
      setpoint.y = 0;
    }

    NunchuckDriver::~NunchuckDriver()
    {
      // Clean up dynamically allocated objects
      if (nunchuck != nullptr)
      {
        delete nunchuck;
        nunchuck = nullptr;
      }

      Log.trace("NunchuckDriver destroyed");
    }

    void NunchuckDriver::init()
    {
      // Initialize nunchuck
      nunchuck->begin();

      if (nunchuck->type == UnknownChuck)
      {
        Log.error("Nunchuck not detected!");
      }
      else
      {
        Log.info("Nunchuck initialized");
      }
    }

    core::xy_coordf NunchuckDriver::process(float *servoValues)
    {
      // Update nunchuck data
      nunchuck->readData();

      // Handle button presses
      handleButtons();

      // Update setpoint based on mode
      updateSetpoint();

      // Apply platform movement based on mode
      core::Platform platform(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

      switch (mode)
      {
      case SETPOINT:
        // In SETPOINT mode, the platform tries to keep the ball at the setpoint
        // The actual movement is handled by the touchscreen driver
        break;

      case CONTROL:
        // In CONTROL mode, the joystick directly controls the platform
        if (abs(nunchuck->getJoyX()) > deadBand.x || abs(nunchuck->getJoyY()) > deadBand.y)
        {
          float joyX = nunchuck->getJoyX() / 127.0f; // Normalize to -1.0 to 1.0
          float joyY = nunchuck->getJoyY() / 127.0f; // Normalize to -1.0 to 1.0

          switch (subMode)
          {
          case PITCH_ROLL:
            platform.moveTo(servoValues, joyY * MAX_PITCH, joyX * MAX_ROLL);
            break;

          case HEAVE_YAW:
            platform.moveTo(servoValues, 0, 0, joyY * MAX_HEAVE, 0, 0, joyX * MAX_YAW);
            break;

          case SWAY_SURGE:
            platform.moveTo(servoValues, joyX * MAX_SWAY, joyY * MAX_SURGE, 0, 0, 0, 0);
            break;
          }
        }
        else
        {
          // If joystick is in deadband, return to home position
          platform.home(servoValues);
        }
        break;

      case CIRCLE:
        // In CIRCLE mode, the setpoint moves in a circle
        // This is a placeholder for the actual implementation
        break;

      case EIGHT:
        // In EIGHT mode, the setpoint moves in a figure eight
        // This is a placeholder for the actual implementation
        break;

      case SQUARE:
        // In SQUARE mode, the setpoint cycles through corners of a square
        // This is a placeholder for the actual implementation
        break;
      }

      return setpoint;
    }

    void NunchuckDriver::handleButtons()
    {
      // Check for button presses
      bool zNow = nunchuck->getButtonZ();
      bool cNow = nunchuck->getButtonC();

      // Z button handling
      if (zNow && !zPressed)
      {
        // Z button just pressed
        unsigned long now = millis();

        switch (mode)
        {
        case SETPOINT:
          // In SETPOINT mode, Z button resets the setpoint to center
          setpoint.x = 0;
          setpoint.y = 0;
          break;

        case CONTROL:
          // In CONTROL mode, Z button cycles through submodes
          subMode = static_cast<ControlSubMode>((subMode + 1) % 3);
          Log.info("Control submode: %s", getSubModeString(subMode));

          // Blink LED to indicate submode
          modeBlinker.blink(subMode + 1);
          break;

        case CIRCLE:
        case EIGHT:
          // In CIRCLE/EIGHT mode, Z button stops/starts the movement
          // This is a placeholder for the actual implementation
          break;

        case SQUARE:
          // In SQUARE mode, Z button moves to the next corner
          // This is a placeholder for the actual implementation
          break;
        }

        lastButtonTime = now;
      }

      // C button handling
      if (cNow && !cPressed)
      {
        // C button just pressed
        unsigned long now = millis();

        if (isDoubleClick(now))
        {
          // Double-click handling
          switch (mode)
          {
          case CIRCLE:
          case EIGHT:
          case SQUARE:
            // In CIRCLE/EIGHT/SQUARE mode, double-click reverses direction
            direction = (direction == CW) ? CCW : CW;
            Log.info("Direction: %s", getDirectionString(direction));
            break;

          default:
            // No double-click action for other modes
            break;
          }
        }
        else
        {
          // Single-click handling - cycle through modes
          mode = static_cast<ControlMode>((mode + 1) % 5);
          Log.info("Mode: %s", getModeString(mode));

          // Blink LED to indicate mode
          modeBlinker.blink(mode + 1);
        }

        lastButtonTime = now;
      }

      // Update button states
      zPressed = zNow;
      cPressed = cNow;
    }

    void NunchuckDriver::updateSetpoint()
    {
      // Update setpoint based on mode
      switch (mode)
      {
      case SETPOINT:
        // In SETPOINT mode, the joystick moves the setpoint
        if (abs(nunchuck->getJoyX()) > deadBand.x)
        {
          setpoint.x += (nunchuck->getJoyX() / 127.0f) * 0.001f; // Slow movement
          setpoint.x = constrain(setpoint.x, -1.0f, 1.0f);
        }

        if (abs(nunchuck->getJoyY()) > deadBand.y)
        {
          setpoint.y += (nunchuck->getJoyY() / 127.0f) * 0.001f; // Slow movement
          setpoint.y = constrain(setpoint.y, -1.0f, 1.0f);
        }
        break;

      case CIRCLE:
        // In CIRCLE mode, the setpoint moves in a circle
        // The joystick Y-axis controls the speed
        if (abs(nunchuck->getJoyY()) > deadBand.y)
        {
          speed = map(nunchuck->getJoyY(), -127, 127, 0.05f, 0.5f);
        }

        // This is a placeholder for the actual implementation
        break;

      case EIGHT:
        // In EIGHT mode, the setpoint moves in a figure eight
        // The joystick Y-axis controls the speed
        if (abs(nunchuck->getJoyY()) > deadBand.y)
        {
          speed = map(nunchuck->getJoyY(), -127, 127, 0.05f, 0.5f);
        }

        // This is a placeholder for the actual implementation
        break;

      case SQUARE:
        // In SQUARE mode, the setpoint cycles through corners of a square
        // This is a placeholder for the actual implementation
        break;

      default:
        // No setpoint update for other modes
        break;
      }
    }

    bool NunchuckDriver::isDoubleClick(unsigned long time)
    {
      return (time - lastButtonTime < NUNCHUCK_DBLCLICK_THRESHOLD_MS && lastButtonTime > 0);
    }

    ControlMode NunchuckDriver::getMode()
    {
      return mode;
    }

    ControlSubMode NunchuckDriver::getSubMode()
    {
      return subMode;
    }

    Direction NunchuckDriver::getDirection()
    {
      return direction;
    }

    const char *NunchuckDriver::getModeString(ControlMode mode)
    {
      return MODE_STRINGS[mode];
    }

    const char *NunchuckDriver::getSubModeString(ControlSubMode subMode)
    {
      return SUBMODE_STRINGS[subMode];
    }

    const char *NunchuckDriver::getDirectionString(Direction dir)
    {
      return DIRECTION_STRINGS[dir];
    }

  } // namespace drivers
} // namespace stewy
