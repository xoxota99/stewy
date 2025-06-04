# Stewy Project TODO List

This document tracks improvements and tasks for the Stewy project. Check off items as they are completed.

## 1. Code Quality Improvements

- [x] Add Documentation:
  - [x] Add basic Doxygen-style documentation to class and function headers
  - [x] Complete documentation for all functions with parameters, return values, and behavior

- [ ] Implement Unit Tests:
  - [ ] Create a testing framework for the platform's inverse kinematics
  - [ ] Add tests for the PID controller functionality
  - [ ] Test edge cases for servo movement limits

- [ ] Code Organization:
  - [x] Organize code into proper directory structure with include/ and src/ folders
  - [ ] Further refactor main.cpp into smaller, more focused modules
  - [ ] Create separate classes for different functionalities (e.g., ServoController)

- [ ] Error Handling:
  - [ ] Add more robust error handling throughout the codebase
  - [ ] Implement a proper error reporting system instead of just logging

- [ ] Configuration Management:
  - [x] Use EEPROM to store calibration values
  - [ ] Move more hardcoded values from config.h to a configuration file that can be loaded at runtime

## 2. Feature Implementations

- [ ] Complete Planned Features:
  - [ ] Implement the "coming soon" features mentioned in the README:
    - [ ] CIRCLE mode
    - [ ] EIGHT mode
    - [ ] SQUARE mode
  - [ ] Add the ability to save and load different configurations

- [x] Calibration Improvements:
  - [x] Create an automated calibration routine for the touchscreen
  - [ ] Implement a servo calibration procedure to fine-tune the SERVO_TRIM values

- [ ] User Interface:
  - [x] Implement basic command line interface
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
  - [x] Implement configurable PID parameters
  - [ ] Implement auto-tuning for the PID controllers
  - [ ] Add more sophisticated filtering for the touchscreen input

- [x] Inverse Kinematics:
  - [x] Optimize the IK algorithm as mentioned in the README
  - [x] Implement the mathematical improvements for better range of movement
  - [x] Add boundary checking for all movement parameters

- [ ] Memory Usage:
  - [ ] Optimize memory usage, especially for the Teensy platform
  - [ ] Consider using fixed-point math instead of floating-point for performance

## 5. Build and Deployment

- [ ] Cross-Platform Compatibility:
  - [ ] Make the code more portable across different Arduino platforms
  - [ ] Replace Teensy-specific code (like Serial.printf) with more portable alternatives

- [ ] Continuous Integration:
  - [x] Add basic Travis CI configuration
  - [ ] Set up a proper CI/CD pipeline with automated testing

- [ ] Documentation:
  - [ ] Create better user documentation with setup instructions
  - [ ] Add diagrams and illustrations for assembly

## 6. Specific Technical Improvements

- [x] Platform.cpp:
  - [x] Optimize the inverse kinematics calculations
  - [x] Add boundary checking for all movement parameters
  - [x] Implement enhanced IK algorithm with adjustable rotation point

- [x] Touchscreen Handling:
  - [x] Implement filtering for the touchscreen input
  - [x] Add calibration routines for the touchscreen
  - [x] Implement deadzone filter to avoid jitter

- [ ] Servo Control:
  - [x] Add acceleration/deceleration to servo movements for smoother operation
  - [ ] Implement a safety system to prevent servo overheating

- [x] Nunchuck Interface:
  - [x] Implement basic mode switching and control
  - [ ] Improve the responsiveness of the nunchuck controls
  - [ ] Complete implementation of all planned modes

## 7. Logical Errors to Fix

- [x] **PID Controller Input/Output Handling**
  - [x] Review how PID values are updated and ensure proper input/output flow
  - [x] Add validation for PID parameters to prevent unstable behavior

- [x] **Touchscreen Calibration Persistence**
  - [x] Store calibration data in EEPROM
  - [x] Add validation for EEPROM calibration data
  - [x] Implement fallback defaults if calibration data is corrupted

- [ ] **Error Handling for Missing Hardware**
  - [x] Add detection for nunchuck connection
  - [ ] Implement more graceful handling for connected but non-responsive hardware
  - [ ] Implement timeout mechanisms for hardware communication

- [x] **Resource Management**
  - [x] Add proper cleanup for objects created with `new`
  - [ ] Consider using smart pointers or RAII patterns for better resource management

- [ ] **Concurrent Access to Servo Values**
  - [ ] Implement a synchronization mechanism for the servoValues array
  - [ ] Create a priority system for different components updating servo positions

- [x] **Timing Issues**
  - [x] Add timing control to the main loop for consistent execution
  - [x] Ensure PID loops run at consistent intervals for proper control

- [ ] **Initialization Order Dependencies**
  - [x] Implement proper initialization sequence
  - [ ] Add more explicit checks to ensure components are initialized before use

- [ ] **Lack of Bounds Checking**
  - [x] Add bounds checking for platform movement parameters
  - [ ] Add explicit bounds checking for all array accesses
  - [ ] Implement safe accessor methods for critical data structures

- [ ] **Incomplete Implementation of Planned Features**
  - [ ] Complete or disable advertised features that aren't fully implemented
  - [ ] Add clear status indicators for features in development

- [ ] **Potential Floating Point Precision Issues**
  - [x] Optimize floating point calculations in inverse kinematics
  - [ ] Consider fixed-point alternatives for critical calculations

- [ ] **Inconsistent Error Handling**
  - [ ] Standardize error handling approach across the codebase
  - [ ] Ensure all return values are properly checked

- [ ] **Potential for Servo Binding**
  - [ ] Implement protection against servo binding conditions
  - [ ] Add current monitoring if hardware supports it
  - [ ] Create safe recovery procedures for high-current states
