# UI Directory

This directory contains user interface components for the Stewy project. These components handle user input and provide feedback to the user.

## Contents

- `CommandLine.cpp`: Implementation of the command-line interface
  - Processes commands from the serial interface
  - Provides a set of commands for controlling and configuring the platform
  - Uses the GeekFactory Shell Library for command parsing and execution

## Features

The command-line interface provides the following functionality:

- Direct servo control (`set`, `mset`, `setall`, `msetall`)
- Platform movement control (`moveto`, `home`)
- System information display (`dump`)
- PID controller tuning (`px`, `py`, `ix`, `iy`, `dx`, `dy`)
- Touchscreen calibration (`calibrate`)
- Log level control (`log`)
- Demo sequence execution (`demo`)
- System reset (`reset`)

## Architecture

The UI components follow these design principles:

1. Separation of concerns: UI components don't directly control hardware
2. Command handlers are implemented as static methods
3. The UI maintains references to other components (touchscreen, nunchuck) but doesn't own them
4. Error handling includes parameter validation and user feedback
