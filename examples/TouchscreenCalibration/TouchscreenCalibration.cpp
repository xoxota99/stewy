/**
 * @file TouchscreenCalibration.cpp
 * @brief Touchscreen calibration example for the Stewart platform
 * 
 * This example demonstrates how to calibrate the touchscreen
 * for the Stewart platform.
 * 
 * @author Philippe Desrosiers
 * @copyright Copyright (C) 2018 Philippe Desrosiers
 * @license GPL-3.0
 */

#include <Arduino.h>
#include <ArduinoLog.h>
#include <Servo.h>
#include "core/Platform.h"
#include "core/Config.h"
#include "drivers/TouchScreen.h"

using namespace stewy::core;
using namespace stewy::drivers;

// Create platform object
Platform platform(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

// Create touchscreen driver
TouchScreenDriver touchscreen(XP, YP, XM, YM, TS_OHMS);

// Create servo objects
Servo servos[6];

// Servo setpoints in degrees
float servoValues[6];

// Function to convert angle to microseconds
float toMicroseconds(int angle) {
  return map(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, SERVO_MIN_US, SERVO_MAX_US);
}

// Update servo positions
void updateServos() {
  for (int i = 0; i < 6; i++) {
    // Apply reverse if needed
    float val = servoValues[i];
    if (SERVO_REVERSE[i]) {
      val = SERVO_MIN_ANGLE + (SERVO_MAX_ANGLE - val);
    }
    
    // Convert to microseconds and apply trim
    val = toMicroseconds(val) + SERVO_TRIM[i];
    
    // Write to servo
    servos[i].writeMicroseconds(constrain(val, SERVO_MIN_US, SERVO_MAX_US));
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Log.begin(LOG_LEVEL, &Serial);
  Log.notice("Touchscreen Calibration Example Starting...");
  
  // Initialize servos
  for (int i = 0; i < 6; i++) {
    servos[i].attach(SERVO_PINS[i]);
  }
  
  // Initialize touchscreen
  touchscreen.init();
  
  // Move to home position
  platform.home(servoValues);
  updateServos();
  delay(2000);
  
  // Start calibration
  Log.notice("Starting touchscreen calibration...");
  touchscreen.startCalibration();
}

void loop() {
  // Process touchscreen input
  if (touchscreen.isCalibrationInProgress()) {
    // During calibration, just process the touchscreen
    touchscreen.process(0, 0, servoValues);
    updateServos();
  } else {
    // After calibration, demonstrate using the touchscreen
    // Move the setpoint in a circle
    static unsigned long lastMove = 0;
    static float angle = 0;
    
    unsigned long now = millis();
    if (now - lastMove > 50) {
      lastMove = now;
      angle += 0.05;
      if (angle > TWO_PI) {
        angle -= TWO_PI;
      }
      
      float setpoint_x = 0.5 * sin(angle);
      float setpoint_y = 0.5 * cos(angle);
      
      touchscreen.process(setpoint_x, setpoint_y, servoValues);
      updateServos();
      
      Log.trace("Setpoint: (%.2f, %.2f)", setpoint_x, setpoint_y);
    }
  }
}
