# Include Directory

This directory contains all header files for the Stewy project. Header files are organized into subdirectories based on their functionality.

## Directory Structure

- `core/`: Core functionality and common definitions
  - `Config.h`: Project-wide configuration constants and settings
  - `Platform.h`: Stewart platform kinematics and control interface
  - `PlatformGeometry.h`: Geometric constants and calculations for the platform

- `drivers/`: Hardware driver interfaces
  - `TouchScreen.h`: Interface for the touchscreen driver with filtering and calibration
  - `Nunchuck.h`: Interface for the Wii Nunchuck controller with mode management

- `ui/`: User interface related headers
  - `CommandLine.h`: Serial command interface for controlling the platform

- `platform/`: Platform-specific code
  - `TeensyHardware.h`: Teensy-specific hardware abstractions and utilities

## Usage Guidelines

1. Use relative includes with proper paths (e.g., `#include "core/Config.h"`)
2. Keep header files minimal and focused on a single responsibility
3. Use forward declarations when possible to minimize dependencies
4. Document all public interfaces with Doxygen-style comments
5. Include appropriate copyright and license information in each file
