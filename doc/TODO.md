# Stewy Project TODO List

This document tracks improvements and tasks for the Stewy project. Check off items as they are completed.

## 1. Code Quality Improvements

- [ ] Add Documentation:
  - [ ] Implement Doxygen-style documentation as mentioned in the README's "Improvements" section
  - [ ] Add function header comments to explain parameters, return values, and behavior

- [ ] Implement Unit Tests:
  - [ ] Create a testing framework for the platform's inverse kinematics
  - [ ] Add tests for the PID controller functionality
  - [ ] Test edge cases for servo movement limits

- [ ] Code Organization:
  - [ ] Refactor the main.cpp file which is quite large (7098 lines) into smaller, more focused modules
  - [ ] Create separate classes for different functionalities (e.g., ServoController, PlatformController)

- [ ] Error Handling:
  - [ ] Add more robust error handling throughout the codebase
  - [ ] Implement a proper error reporting system instead of just logging

- [ ] Configuration Management:
  - [ ] Move hardcoded values from config.h to a configuration file that can be loaded at runtime
  - [ ] Consider using EEPROM to store calibration values

## 2. Feature Implementations

- [ ] Complete Planned Features:
  - [ ] Implement the "coming soon" features mentioned in the README:
    - [ ] CIRCLE mode
    - [ ] EIGHT mode
    - [ ] SQUARE mode
  - [ ] Add the ability to save and load different configurations

- [ ] Calibration Improvements:
  - [ ] Create an automated calibration routine for the touchscreen
  - [ ] Implement a servo calibration procedure to fine-tune the SERVO_TRIM values

- [ ] User Interface:
  - [ ] Develop a more user-friendly serial command interface
  - [ ] Add a web interface using ESP8266/ESP32 for remote control and monitoring

## 3. Hardware Design Improvements

- [ ] Power Management:
  - [ ] Implement the power management improvements mentioned in the README
  - [ ] Add detection for USB vs. battery power and adjust servo behavior accordingly
  - [ ] Consider adding a separate power supply for the servos

- [ ] Mechanical Design:
  - [ ] Redesign the platform to use a single thickness of material as suggested in the README
  - [ ] Break up the upper ring into smaller pieces for more cost-effective manufacturing
  - [ ] Design a more universal mounting system for different servo types

## 4. Performance Optimizations

- [ ] PID Controller Tuning:
  - [ ] Implement auto-tuning for the PID controllers
  - [ ] Add more sophisticated filtering for the touchscreen input

- [ ] Inverse Kinematics:
  - [ ] Optimize the IK algorithm as mentioned in the README
  - [ ] Implement the mathematical improvements for better range of movement

- [ ] Memory Usage:
  - [ ] Optimize memory usage, especially for the Teensy platform
  - [ ] Consider using fixed-point math instead of floating-point for performance

## 5. Build and Deployment

- [ ] Cross-Platform Compatibility:
  - [ ] Make the code more portable across different Arduino platforms
  - [ ] Replace Teensy-specific code (like Serial.printf) with more portable alternatives

- [ ] Continuous Integration:
  - [ ] Set up a proper CI/CD pipeline using the existing .travis.yml file
  - [ ] Add automated testing as part of the build process

- [ ] Documentation:
  - [ ] Create better user documentation with setup instructions
  - [ ] Add diagrams and illustrations for assembly

## 6. Specific Technical Improvements

- [ ] Platform.cpp:
  - [ ] Optimize the inverse kinematics calculations
  - [ ] Add boundary checking for all movement parameters

- [ ] Touchscreen Handling:
  - [ ] Implement better filtering for the touchscreen input
  - [ ] Add calibration routines for the touchscreen

- [ ] Servo Control:
  - [ ] Add acceleration/deceleration to servo movements for smoother operation
  - [ ] Implement a safety system to prevent servo overheating

- [ ] Nunchuck Interface:
  - [ ] Improve the responsiveness of the nunchuck controls
  - [ ] Add more intuitive control mappings