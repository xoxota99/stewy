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
We use a 4-wire resistive touchscreen (e.g. https://tinyurl.com/ybsr2pmk ) to determine the X/Y coordinates of the ball bearing. The touchscreen is likely to report a certain amount of “noise” when no pressure is applied. We will need to either debounce or else apply a filter, to prevent the noise from being interpreted as valid input.

## PID control loop
We use a Proportional / Integral / Derivative (PID) feedback loop (similar to control loops used in Drone firmware) to determine the error position between the ball bearing’s current position, and the desired – or setpoint – position. This is used to determine the target orientation of the platform, in order to best return the ball to the setpoint position.

