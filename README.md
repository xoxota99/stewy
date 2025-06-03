# 6dof-stewduino
Arduino project for a 6DOF, PID-controlled Stewart platform, running on Arduino. Inspired by https://www.youtube.com/watch?v=j4OmVLc_oDw .

## Goal
Teach myself embedded development, using something a little more unique / less trivial than a typical “Blinky” app. This project will incorporate:
  * Bi-directional Serial I/O (Command shell)
  * Wiimote nunchuck control (over I2C)
  * Resistive touchscreen (Analog 4-wire)
  * A bunch of servos (Digital IO)

## Project Description
A ball bearing sits on the touchscreen, which reports the X/Y coordinates of the weight. The X/Y coordinates are used to generate an “error” value against the current set point, and this error is used in a PID feedback loop to control six servos. The servos drive the angle of the platform in an inverse-kinematic solution, to drive the ball bearing toward the setpoint.

## Serial
We want to be able to control some aspects of the project from a laptop during initial configuration, and get some data out of the system over USB, using the Arduino Serial monitor:
  * Input: Send commands from the console:
    * Calibrate WiiMote
    * Tune PIDs
    * Change current X/Y SetPoint
    * Change touchscreen LPF / deadband
    * Direct individual servo control
    * etc.
  * Out: Receive logger data
    * WiiMote inputs
    * Touchscreen data
    * Error messages
    * Servo values
    * etc.

## Touchscreen
We use a [4-wire resistive touchscreen] (https://tinyurl.com/ybsr2pmk), and [Adafruit Touchscreen library] (https://github.com/adafruit/Touch-Screen-Library) to determine the X/Y coordinates of the ball bearing.

## PID control loop
We use a Proportional / Integral / Derivative (PID) feedback loop (similar to control loops used in Drone firmware) to determine the error position between the ball bearing’s current position, and the setpoint position. This is used to determine the target orientation of the platform, in order to best return the ball to the setpoint position.

## Wiimote Nunchuck
We use a wiimote [nunchuck] (https://en.wikipedia.org/wiki/Wii_Remote#Nunchuk) to change "modes" and control various aspects of the platform in real time. Mode changes are indicated by the LED blinking. The various modes can be cycled through in order by clicking the C button:
  * 1 blink: **SETPOINT** - The platform will attempt to keep the ball at a given X/Y setpoint. By default, this is in the middle of the plate. **This is the default mode at startup.** In this mode:
    * The joystick can be used to move the setpoint, with the platform correcting the position of the ball in real time.
    * Clicking the Z button (or resetting the Teensy) resets the setpoint to the middle of the plate.
  * 2 blinks: **CONTROL** - Direct control of the platform using the joystick. Input from the touchscreen sensor is ignored completely. In this mode:
    * Clicking the Z button cycles between three sub-modes:
      * **PITCH_ROLL** - (default) The joystick controls the pitch / roll of the platform.
      * **HEAVE_YAW** - The joystick controls the up/down and rotation of the platform.
      * **SWAY_SURGE** - The joystick controls the forward/rear and left/right translation of the platform.
  * 3 blinks: **CIRCLE** - *(coming soon)* The setpoint will move in a circular pattern, around the center of the platform. In this mode:
    * The joystick Y-axis controls the speed of the movement.
    * Clicking the Z button stops the setpoint in place. While the setpoint is stopped, the Joystick can be used to change the radius of the circular movement. Clicking the Z button again resumes the circular movement.
    * Double-clicking the C button reverses the direction of the movement.
  * 4 blinks: **EIGHT** - *(coming soon)* Similar to *CIRCLE*, the setpoint will move in a [Lemniscate of Bernoulli] (https://en.wikipedia.org/wiki/Lemniscate_of_Bernoulli), centered on the center of the platform. In this mode:
    * The joystick Y-axis controls the speed of the movement.
    * Clicking the Z button stops the setpoint in place. Clicking the Z button again resumes the figure-eight movement.
    * Double-clicking the C button reverses the direction of the movement.
  * 5 blinks: **SQUARE** - *(coming soon)* The setpoint will be set to the current location of the ball on the plate. The setpoint does not move independently in this mode. In this mode:
    * Clicking the Z button reflects the setpoint across the X or Y axis in a clockwise direction. The effect is to move the ball in a seeming rectangular pattern around the plate.
    * Double-clicking the C button reverses the direction, so that subsequent use of the Z button will reflect the setpoint across the X or Y axis in a *counter*-clockwise direction.
    * The joystick moves the setpoint within the ball's current quadrant of the plate.

## Caveats
* The project is built using [Teensy 3.x](https://www.pjrc.com/store/teensy32.html). I haven't tried building against other targets, but pull requests are always welcome!
* The project is developed using PlatformIO. This means (among other possible things):
  * The folder structure is incompatible with Arduino IDE.
  * Teensy floating-point support is not built automatically (See https://community.platformio.org/t/pio-doesnt-build-teensy-floating-point-support/3296 ). I guess that's a work in progress.

## Improvements
Some things I'd like to try:
* Optimize the length of the control rods, as well as the size ratio of the upper platform to the lower base. There's some [heavy math involved](https://ac.els-cdn.com/S1000936107600570/1-s2.0-S1000936107600570-main.pdf?_tid=eeb5d700-ffc7-11e7-a68f-00000aacb361&acdnat=1516662163_113697b3470f5b083bb550353edd4053), that I haven't gotten around to yet.
* Improve the IK math, to allow a greater range of movement. Right now, the algorithm attempts to rotate around the platform's "home" position. This means that there's a constraint on the maximum angle of pitch / roll. This is just because the IK solution is not specialized to this particular (Ball & Plate balancing act) application. It's a generalized [Stewart platform solution](https://www.youtube.com/watch?v=1jrP3_1ML9M). We compensate for this in part with the AGGRO prescalar for servo results.
* Improve the electrical design:
  * All servos, and the MCU, are powered off a single bus from the BEC. If one or more servos enter a high-current state (such as binding, or obstruction, etc.), this can cause the MCU to brown out and reset. Likewise, connecting the board to USB causes the servos to be powered as well. Since USB can't provide the necessary current, the board can brown out. There's should be a way to either cut the trace to USB power on the Teensy, or put a voltage divider on an analog pin, to detect whether we're on USB or battery power, and enable / disable the motors as a result.
* Improve the mechanical design:
  * The plastic control rod ends can bind on the servo arms, causing a high-current state in the servos. I solved this temporarily by adding standoffs between the servo arm and the control rod end. Of course, this throws off the geometry of the platform, and we'll have to update the value of B_RAD in Platform.h.
  * Right now, the design calls for 5 laser cut acrylic pieces, in 4 different thicknesses. That's expensive to get from an online CNC place. A better / cheaper design would use only one thickness of material.
  * The "ring" around the upper plate is large, which is expensive if you're ordering from a place (I use the very excellent [Ponoko](https://www.ponoko.com/)) that charges in part based on the surface area of the design. A better design would break up the upper ring into multiple smaller pieces that could be assembled later.
  * The design requires some tooling that can't be done on a laser cutter (such as drilling holes for the upper control rod joints). This is actually the hardest part of the whole project, getting those holes properly aligned and drilled straight. The design also calls for some counter-sunk holes (such as attaching the touchscreen mounting plate to the upper platform). It would be better if that counter-sinking wasn't necessary.
  * The design of the upper and lower base plates is very specific to the servo chosen (the very excellent [Hitec HS-5625MG](http://hitecrcd.com/products/servos/sport-servos/digital-sport-servos/hs-5625mg-high-speed-metal-gear-servo/product)). Other servos are not guaranteed to fit. A better design would allow for any servo to be used.
* Improve code quality:
  * Add doxygen-style documentation
  * Add unit tests
  * Refactor code to be more portable across Arduino IDEs and board targets. (e.g. Serial.printf);
# Project Structure

The project is organized into the following directory structure:

- `include/`: Header files
  - `core/`: Core functionality and common definitions
  - `drivers/`: Hardware driver interfaces
  - `ui/`: User interface related headers
  - `platform/`: Platform-specific code

- `src/`: Source files
  - `core/`: Core functionality implementation
  - `drivers/`: Hardware driver implementations
  - `ui/`: User interface implementations
  - `platform/`: Platform-specific implementations

- `examples/`: Example applications
  - `BasicMovement/`: Basic platform movement example
  - `TouchscreenCalibration/`: Touchscreen calibration example

- `doc/`: Documentation
  - `TODO.md`: Project tasks and improvements
  - `CAD files/`: CAD designs for the platform

- `lib/`: External libraries
  - `Blinker/`: LED blinking utility
  - `WiiChuck/`: Wii Nunchuck interface
# Building the Project

To build the project, you can use the provided build script:

```bash
./build.sh
```

Or run PlatformIO directly:

```bash
platformio run
```

## Migrating from the Old Structure

If you're working with code that still uses the old flat structure, please refer to the [Migration Guide](MIGRATION.md) for instructions on updating your code to use the new structure.
