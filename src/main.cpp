/*
  6dof-stewduino
  Copyright (C) 2018  Philippe Desrosiers

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//=== Includes
#include <Arduino.h>
#include <Servo.h>
#include "config.h"
#include "Platform.h"

#ifdef ENABLE_NUNCHUCK
#include "nunchuck.h"
#endif

#ifdef ENABLE_SERIAL_COMMANDS
#include "commands.h"
#endif

#ifdef ENABLE_TOUCHSCREEN
#include "touch.h"
#endif

//=== Macros
#ifdef CORE_TEENSY
//Software reset macros / MMap FOR TEENSY ONLY
#define CPU_RESTART_VAL 0x5FA0004                           // write this magic number...
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C             // to this memory location...
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL)   // presto!
#else
#define CPU_RESTART asm volatile ("  jmp 0")                // close enough for arduino
#endif

//=== Actual code

Platform stu;            // Stewart platform object.
Servo servos[6];        // servo objects.
float sp_servo[6];      // servo setpoints in degrees, between SERVO_MIN_ANGLE and SERVO_MAX_ANGLE.
//
// float lerp(float t, int a, int b) {
//   return (int)(a + (t * (b - a)));
// }

float _toUs(int value) {
  return SERVO_MIN_US + value * (float)(SERVO_MAX_US - SERVO_MIN_US) / (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE); //Number of uS in one degree of angle. Roughly.
}

float _toAngle(float value) {
  return SERVO_MIN_ANGLE + value * (float)(SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) / (SERVO_MAX_US - SERVO_MIN_US); //Number of degrees per uS. Roughly.
}

//Set servo values to the angles represented by the setpoints in sp_servo[].
//DOES: Apply trim values.
//DOES: Automatically reverse signal for reversed servos.
//DOES: Write signals to the physical servos.
void updateServos() {
  static float sValues[6];

  for (int i = 0; i < 6; i++) {
    //sp_servo holds a value between SERVO_MIN_ANGLE and SERVO_MAX_ANGLE.
    //apply reverse.
    float val = sp_servo[i];
    if (SERVO_REVERSE[i]) {
      val = SERVO_MIN_ANGLE + (SERVO_MAX_ANGLE - val);
    }

    //translate angle to pulse width
    val = _toUs(val);

    if (val != sValues[i]) { //don't write to the servo if you don't have to.
      sValues[i] = val;
      Serial.printf("s%d = %.2f + %d (value + trim)\n", i, val, SERVO_TRIM[i]);
      servos[i].writeMicroseconds(min(SERVO_MAX_US, max(SERVO_MIN_US, (int)val + SERVO_TRIM[i])));
    }
  }
}

// Calculates and assigns values to sp_servo.
// DOES: Ignore out-of-range values. These will generate a warning on the serial monitor.
// DOES NOT: Apply servo trim values.
// DOES NOT: Automatically reverse signal for reversed servos.
// DOES NOT: digitally write a signal to any servo. Writing is done in updateServos();
void setServo(int i, int angle) {
  int val = angle;
  if (val >= SERVO_MIN_ANGLE && val <= SERVO_MAX_ANGLE) {
    sp_servo[i] = val;
    Serial.printf("setServo (%d,%.2f) - %.2f degrees\n", i, angle, sp_servo[i]);
  } else {
    Serial.printf("[setServo] WARNING: Invalid value %.2f specified for servo #%d. Valid range is %d to %d degrees.\n", val, i, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
  }

}

void setupTouchscreen() {
#ifdef ENABLE_TOUCHSCREEN
  Serial.println("Touchscreen ENABLED.");
#else
  Serial.println("Touchscreen DISABLED.");
#endif
}

void setupPlatform() {
  stu.home(sp_servo);    // set platform to "home" position (flat, centered).
  updateServos();
  delay(300);
}

//Initialize servo interface, sweep all six servos from MIN to MAX, to MID, to ensure they're all physically working.

//TODO: For different MIN, MAX, and MID values, we need to interpolate so that all servos physically arrive at their setpoints at the same time.
//Interpolation will depend on the error (distance to setpoint), and speed of the individual servo, that can be modeled as SERVO_SPEED or something...

void setupServos() {

  for (int i = 0; i < 6; i++) {
    servos[i].attach(i);
  }

  delay(500);

  for (int i = 0; i < 6; i++) {
    setServo(i, SERVO_MIN_ANGLE);
  }
  updateServos();
  delay(500);

  for (int i = 0; i < 6; i++) {
    setServo(i, SERVO_MAX_ANGLE);
  }
  updateServos();
  delay(500);

  for (int i = 0; i < 6; i++) {
    setServo(i, SERVO_MID_ANGLE);
  }
  updateServos();
  delay(500);


}

/**
   Setup serial port and add commands.
*/
void setupCommandLine(int bps = 9600)
{
  Serial.begin(bps);

  Serial.println("Welcome to Studuino, v1");
  Serial.printf("Built %s, %s\n",__DATE__, __TIME__);
  Serial.println("=======================");

#ifdef ENABLE_SERIAL_COMMANDS
  Serial.println("Command-line is ENABLED.");

  shell_init(shell_reader, shell_writer, 0);

  const int ccount = sizeof(commands);
  for (int i = 0; i < ccount; i++) {
    shell_register(commands[i].shell_program, commands[i].shell_command_string);
  }

#else

  Serial.println("Command-line is DISABLED.");

#endif
}

void setupNunchuck() {
#ifdef ENABLE_NUNCHUCK
  Serial.println("Nunchuck support is ENABLED.");

  nc.begin();
#else
  Serial.println("Nunchuck support is DISABLED.");
#endif
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);   //power indicator
  digitalWrite(LED_BUILTIN, HIGH);

  setupCommandLine();

  setupNunchuck();

  setupServos();

  setupPlatform();

  setupTouchscreen();
}

void loop() {

#ifdef ENABLE_SERIAL_COMMANDS
  processCommands();    //process any incoming serial commands.
#endif

#ifdef ENABLE_NUNCHUCK
  processNunchuck();
#endif

#ifdef ENABLE_TOUCHSCREEN
  processTouchscreen();
#endif

  updateServos();
}
