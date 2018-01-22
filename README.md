# 6dof-stewduino
Arduino project for a 6DOF, PID-controlled Stewart platform, running on Arduino. Inspired by https://www.youtube.com/watch?v=j4OmVLc_oDw .

## Goal
Teach myself embedded development, using something a little more unique / less trivial than a typical “Blinky” app. This project will incorporate:
  * Bi-directional Serial I/O
  * Wiimote nunchuck control (over I2C)
  * Resistive touchscreen (Analog 4-wire)
  * A bunch of servos (Digital IO)

## Project Description
A ball bearing sits on the touchscreen, which reports the X/Y coordinates of the weight. A high-pass filter lets in valid coordinates, and ignores “jitter” (when e.g. there is no weight on the touchscreen). The X/Y coordinates are used to generate an “error” value against the current set point, and this error is used in a PID feedback loop to control six servos. The servos drive the angle of the platform in an invers-kinematic solution, to drive the ball bearing toward the setpoint.

## Serial
We want to be able to control some aspects of the project from a laptop during initial configuration, and get some data out of the system over USB, using the Arduino Serial monitor:
  * Input: Send commands from the console:
    * Calibrate WiiMote
    * Tune PIDs
    * Change current X/Y SetPoint
    * Change touchscreen LPF / deadband
    * Direct individual servo control
  * Out: Receive logger data
    * WiiMote inputs
    * Touchscreen data
    * Error messages
    * Servo values

## Touchscreen
We use a 4-wire resistive touchscreen (e.g. https://tinyurl.com/ybsr2pmk ) to determine the X/Y coordinates of the ball bearing. The touchscreen is likely to report a certain amount of “noise” when no pressure is applied. We will need to either debounce or else apply a filter, to prevent the noise from being interpreted as valid input. Teensy Analog inputs are only 3.3V tolerant (see https://forum.pjrc.com/threads/40830-Teensy-3-2-ADC-amp-AREF?p=127431&viewfull=1#post127431 ), so we'll power the screen from pin

## PID control loop
We use a Proportional / Integral / Derivative (PID) feedback loop (similar to control loops used in Drone firmware) to determine the error position between the ball bearing’s current position, and the desired – or setpoint – position. This is used to determine the target orientation of the platform, in order to best return the ball to the setpoint position.

## Servos
We use an interactive shell-like interface over Serial monitor to set / query Servo values. Commands are:
set <servo_number> <us_value> - servo_number: The number of the servo (from 1-6). us_value: The PWM value to send to the servo, in microseconds (usually 1000-2000 us).
setall <us_value> - set all servos to the same us_value.
dump [<servo_number>] - Dump the current PWM value for the given servo. If servo_number is ommitted, dump values for all servos.

## Caveats
* The project is built using [Teensy 3.x](https://www.pjrc.com/store/teensy32.html). I haven't tried building against other targets, but pull requests are always welcome!
* The project is developed using PlatformIO. This means (among other possible things):
  * The folder structure is incompatible with Arduino IDE.
  * Teensy floating-point support is not built automatically (See https://community.platformio.org/t/pio-doesnt-build-teensy-floating-point-support/3296 ). I guess that's a work in progress.

## Improvements
Some things I'd like to try:
* Optimize the length of the control rods, as well as the size ratio of the upper platform to the lower base. There's some [heavy math involved](https://ac.els-cdn.com/S1000936107600570/1-s2.0-S1000936107600570-main.pdf?_tid=eeb5d700-ffc7-11e7-a68f-00000aacb361&acdnat=1516662163_113697b3470f5b083bb550353edd4053), that I haven't gotten around to yet.
* Improve the IK math, to allow a greater range of movement. Right now, the algorithm attempts to rotate around the platform's "home" position. This means that there's a constraint on the maximum angle of pitch / roll. This is just because the IK solution is not specialized to this particular (Ball & Plate balancing act) application. It's a generalized [Stewart platform solution](https://www.youtube.com/watch?v=1jrP3_1ML9M). I haven't yet had the heart to "optimize" this (by, say, *removing* 4 degrees of freedom). It would be like lobotomizing a friend. We'll see how I feel after some initial testing.
* Improve the mechanical design:
  * Right now, the design calls for 5 laser cut acrylic pieces, in 4 different thicknesses. That's expensive to get from an online CNC place. A better / cheaper design would use only one thickness of material.
  * The "ring" around the upper plate is large, which is expensive if you're ordering from a place (I use the very excellent [Ponoko](https://www.ponoko.com/)) that charges based on the surface area of the part. A better design would break up the upper ring into multiple smaller pieces that could be assembled later.
  * The design requires some tooling that can't be done on a laser cutter (such as drilling holes for the upper control rod joints). This is actually the hardest part of the whole project, getting those holes properly aligned and drilled straight. The design also calls for some counter-sunk holes (such as attaching the touchscreen mounting plate to the upper platform). It would be better if that counter-sinking wasn't necessary.
  * The design of the upper and lower base plates is very specific to the servo chosen. Other servos are not guaranteed to fit. A better design would allow for any servo to be used.
