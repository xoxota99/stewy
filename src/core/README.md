# Core Directory

This directory contains the core functionality of the Stewy project. These components form the foundation of the system and are not tied to specific hardware implementations.

## Contents

- `Platform.cpp`: Implementation of the Stewart platform kinematics and control
  - Implements inverse kinematics to calculate servo angles from desired platform position
  - Provides methods for moving the platform to specific positions and orientations
  - Includes boundary checking and error handling for movement parameters

## Key Features

### Inverse Kinematics

The Platform class implements an optimized inverse kinematics algorithm that:
- Calculates servo angles based on desired platform position and orientation
- Supports 6 degrees of freedom (sway, surge, heave, pitch, roll, yaw)
- Includes boundary checking to prevent impossible movements
- Optimizes trigonometric calculations for better performance
- Supports an enhanced IK algorithm with adjustable rotation point

### Platform Control

The Platform class provides methods for:
- Moving the platform to a specific position and orientation
- Moving the platform to a home position
- Simplified control of just pitch and roll
- Getting current platform state

## Architecture

The core components follow these design principles:

1. Hardware independence: Core functionality doesn't depend on specific hardware
2. Clear separation of concerns: Platform kinematics are separate from hardware control
3. Robust error handling: All methods include parameter validation and error reporting
4. Efficient implementation: Optimized calculations for embedded systems
