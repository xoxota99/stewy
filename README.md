# 6dof-stewduino

Arduino project for a 6DOF, PID-controlled Stewart platform, running on Teensy. Inspired by https://www.youtube.com/watch?v=j4OmVLc_oDw.

## Goal

Teach myself embedded development, using something a little more unique/less trivial than a typical "Blinky" app. This project incorporates:
  * Bi-directional Serial I/O (Command shell)
  * Wiimote nunchuck control (over I2C)
  * Resistive touchscreen (Analog 4-wire)
  * Multiple servos (Digital IO)

## Project Description

A ball bearing sits on the touchscreen, which reports the X/Y coordinates of the weight. The X/Y coordinates are used to generate an "error" value against the current set point, and this error is used in a PID feedback loop to control six servos. The servos drive the angle of the platform in an inverse-kinematic solution, to drive the ball bearing toward the setpoint.

## Serial Command Interface

The project includes a command-line interface over USB serial that allows you to:
  * Calibrate the touchscreen
  * Tune PID parameters
  * Change the current X/Y setpoint
  * Adjust touchscreen filtering and deadband
  * Control individual servos directly
  * Run demo sequences
  * View system information

Available commands include:
  * `help` - Display available commands
  * `set` - Set a single servo angle
  * `moveto` - Move platform to a specific position
  * `calibrate` - Start touchscreen calibration
  * `px`, `py`, `ix`, `iy`, `dx`, `dy` - Set PID parameters
  * `dump` - Display system information
  * `demo` - Run a demonstration sequence

## Touchscreen

The project uses a [4-wire resistive touchscreen](https://tinyurl.com/ybsr2pmk) and the [Adafruit Touchscreen library](https://github.com/adafruit/Touch-Screen-Library) to determine the X/Y coordinates of the ball bearing. The touchscreen driver includes:
  * Calibration routine with EEPROM storage
  * Moving average filter to reduce noise
  * Deadband filter to prevent jitter

## PID Control Loop

The project uses a Proportional/Integral/Derivative (PID) feedback loop to determine the error position between the ball bearing's current position and the setpoint position. This is used to determine the target orientation of the platform, in order to best return the ball to the setpoint position. Features include:
  * Separate PID controllers for X and Y axes
  * Configurable PID parameters via serial commands
  * Safety limits to prevent unstable behavior

## Wiimote Nunchuck Control

The project uses a Wiimote [nunchuck](https://en.wikipedia.org/wiki/Wii_Remote#Nunchuk) to change "modes" and control various aspects of the platform in real time. Mode changes are indicated by the LED blinking. The various modes can be cycled through in order by clicking the C button:

  * 1 blink: **SETPOINT** - The platform will attempt to keep the ball at a given X/Y setpoint. By default, this is in the middle of the plate. **This is the default mode at startup.** In this mode:
    * The joystick can be used to move the setpoint, with the platform correcting the position of the ball in real time.
    * Clicking the Z button (or resetting the Teensy) resets the setpoint to the middle of the plate.
  
  * 2 blinks: **CONTROL** - Direct control of the platform using the joystick. Input from the touchscreen sensor is ignored completely. In this mode:
    * Clicking the Z button cycles between three sub-modes:
      * **PITCH_ROLL** - (default) The joystick controls the pitch/roll of the platform.
      * **HEAVE_YAW** - The joystick controls the up/down and rotation of the platform.
      * **SWAY_SURGE** - The joystick controls the forward/rear and left/right translation of the platform.
  
  * 3 blinks: **CIRCLE** - *(planned)* The setpoint will move in a circular pattern, around the center of the platform.
  
  * 4 blinks: **EIGHT** - *(planned)* Similar to *CIRCLE*, the setpoint will move in a [Lemniscate of Bernoulli](https://en.wikipedia.org/wiki/Lemniscate_of_Bernoulli), centered on the center of the platform.
  
  * 5 blinks: **SQUARE** - *(planned)* The setpoint will be set to the current location of the ball on the plate.

## Project Structure

The project is organized into the following directory structure:

- `include/`: Header files
  - `core/`: Core functionality and common definitions
  - `drivers/`: Hardware driver interfaces
  - `ui/`: User interface related headers
  - `platform/`: Platform-specific code

- `src/`: Source files
  - `core/`: Core functionality implementation (Platform kinematics)
  - `drivers/`: Hardware driver implementations (TouchScreen, Nunchuck)
  - `ui/`: User interface implementations (CommandLine)
  - `platform/`: Platform-specific implementations

- `examples/`: Example applications
  - `BasicMovement/`: Basic platform movement example
  - `TouchscreenCalibration/`: Touchscreen calibration example

- `doc/`: Documentation
  - `TODO.md`: Project tasks and improvements
  - `CAD files/`: CAD designs for the platform

- `lib/`: External libraries

## Technical Details

- **Platform**: [Teensy 3.x](https://www.pjrc.com/store/teensy32.html)
- **Build System**: PlatformIO
- **Servo Control**: Direct via Arduino Servo library
- **Inverse Kinematics**: Optimized algorithm with boundary checking
- **Touchscreen**: 4-wire resistive with filtering and calibration
- **Nunchuck**: I2C communication with mode management

## Building the Project

To build the project, you can use the provided build script:

```bash
./build.sh
```

Or run PlatformIO directly:

```bash
platformio run
```

## Current Status and Future Improvements

The project currently has:
- ✅ Working inverse kinematics with optimized calculations
- ✅ Touchscreen input with filtering and calibration
- ✅ PID control for ball balancing
- ✅ Basic nunchuck control with mode switching
- ✅ Command-line interface for configuration and control
- ✅ Proper resource management and error handling

Planned improvements:
- Complete the CIRCLE, EIGHT, and SQUARE modes
- Improve mechanical design for easier assembly and lower cost
- Enhance power management to prevent brownouts
- Add unit tests and improve code portability
- Implement auto-tuning for PID controllers

See the [TODO.md](doc/TODO.md) file for a complete list of planned improvements and current status.

## Caveats

* The project is built for Teensy 3.x. Other platforms may require modifications.
* The folder structure is designed for PlatformIO and is incompatible with Arduino IDE.
* Servo control is handled directly in main.cpp rather than through a dedicated driver class.

## License

This project is licensed under the GPL-3.0 License - see the LICENSE file for details.
