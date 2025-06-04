# Drivers Directory

This directory contains hardware driver implementations for the Stewy project. Each driver provides an interface to a specific hardware component.

## Contents

- `TouchScreen.cpp`: Implementation of the touchscreen driver for detecting ball position
- `Nunchuck.cpp`: Implementation of the Wii Nunchuck controller driver for user input

## Architecture

The drivers in this directory follow a consistent pattern:
- Each driver has a corresponding header file in the `include/drivers/` directory
- Drivers handle hardware initialization, data processing, and provide a clean interface to the rest of the application
- The touchscreen driver includes filtering, calibration, and PID control functionality
- The nunchuck driver handles button events, mode management, and joystick input processing

## Note on Servo Control

Servo control is handled directly in `main.cpp` using the standard Arduino Servo library rather than through a dedicated driver class. This approach was chosen because:
1. The Arduino Servo library already provides a clean interface
2. Servo control in this project is straightforward
3. Servos are tightly integrated with the Platform class's inverse kinematics
