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
#include <ArduinoLog.h>
#include <Servo.h>
#include "config.h"
#include "Platform.h"

#ifdef ENABLE_NUNCHUCK
#include "chuck.h"

#endif

#ifdef ENABLE_SERIAL_COMMANDS
#include "commands.h"
#endif

#ifdef ENABLE_TOUCHSCREEN
#include "touch.h"
#endif

//=== Macros
#ifdef CORE_TEENSY
// Software reset macros / MMap FOR TEENSY ONLY
#define CPU_RESTART_VAL 0x5FA0004                         // write this magic number...
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C           // to this memory location...
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL) // presto!

// #else
//
// #define CPU_RESTART asm volatile ("  jmp 0")                // close enough for arduino

#endif

// this is the magic trick for printf to support float
asm(".global _printf_float");
// this is the magic trick for scanf to support float
asm(".global _scanf_float");

//=== Actual code

xy_coordf setpoint = DEFAULT_SETPOINT;

Platform stu; // Stewart platform object.

#ifdef ENABLE_SERVOS
Servo servos[6]; // servo objects.
#endif

float sp_servo[6]; // servo setpoints in degrees, between SERVO_MIN_ANGLE and SERVO_MAX_ANGLE.
// Logger* logger = Log.instance();

float _toUs(int value)
{
  return map(value, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, SERVO_MIN_US, SERVO_MAX_US);
}

float _toAngle(float value)
{
  return map(value, SERVO_MIN_US, SERVO_MAX_US, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
}

// Set servo values to the angles represented by the setpoints in sp_servo[].
// DOES: Apply trim values.
// DOES: Automatically reverse signal for reversed servos.
// DOES: Write signals to the physical servos.
void updateServos()
{
  static float sValues[6];

  for (int i = 0; i < 6; i++)
  {
    // sp_servo holds a value between SERVO_MIN_ANGLE and SERVO_MAX_ANGLE.
    // apply reverse.
    float val = sp_servo[i];
    if (SERVO_REVERSE[i])
    {
      val = SERVO_MIN_ANGLE + (SERVO_MAX_ANGLE - val);
    }

    // translate angle to pulse width
    val = _toUs(val);

    if (val != sValues[i])
    {
      // don't write to the servo if you don't have to.
      sValues[i] = val;
      Log.trace("SRV: s%d = %.2f + %d (value + trim)", i, val, SERVO_TRIM[i]);

#ifdef ENABLE_SERVOS
      servos[i].writeMicroseconds((int)constrain(val + SERVO_TRIM[i], SERVO_MIN_US, SERVO_MAX_US));
#endif
    }
  }
}

// Calculates and assigns values to sp_servo.
// DOES: Ignore out-of-range values. These will generate a warning on the serial monitor.
// DOES NOT: Apply servo trim values.
// DOES NOT: Automatically reverse signal for reversed servos.
// DOES NOT: digitally write a signal to any servo. Writing is done in updateServos();
void setServo(int i, int angle)
{
  int val = angle;
  if (val >= SERVO_MIN_ANGLE && val <= SERVO_MAX_ANGLE)
  {
    sp_servo[i] = val;
    Log.trace("setServo %d - %.2f degrees", i, sp_servo[i]);
  }
  else
  {
    Log.warning("setServo: Invalid value '%.2f' specified for servo #%d. Valid range is %d to %d degrees.", val, i, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
  }
}

void setServoMicros(int i, int micros)
{
  int val = micros;
  if (val >= SERVO_MIN_US && val <= SERVO_MAX_US)
  {
    sp_servo[i] = _toAngle(val);
    Log.trace("setServoMicros %d - %.2f us", i, val);
  }
  else
  {
    Log.warning("setServoMicros: Invalid value '%.2f' specified for servo #%d. Valid range is %d to %d us.", val, i, SERVO_MIN_US, SERVO_MAX_US);
  }
}

void setupTouchscreen()
{
#ifdef ENABLE_TOUCHSCREEN
  Log.trace("Touchscreen ENABLED.");
  rollPID.SetSampleTime(ROLL_PID_SAMPLE_TIME);
  pitchPID.SetSampleTime(PITCH_PID_SAMPLE_TIME);
  rollPID.SetOutputLimits(ROLL_PID_LIMIT_MIN, ROLL_PID_LIMIT_MAX);
  pitchPID.SetOutputLimits(PITCH_PID_LIMIT_MIN, PITCH_PID_LIMIT_MAX);
  rollPID.SetMode(AUTOMATIC);
  pitchPID.SetMode(AUTOMATIC);
#else
  Log.trace("Touchscreen DISABLED.");
#endif
}

void setupPlatform()
{
  stu.home(sp_servo); // set platform to "home" position (flat, centered).
  updateServos();
  delay(300);
}

// Initialize servo interface, sweep all six servos from MIN to MAX, to MID, to ensure they're all physically working.
void setupServos()
{
#ifdef ENABLE_SERVOS
  int d = 500;
#else
  int d = 0;
#endif

  for (int i = 0; i < 6; i++)
  {
#ifdef ENABLE_SERVOS
    servos[i].attach(SERVO_PINS[i]);
#endif
    setServo(i, SERVO_MIN_ANGLE);
  }
  updateServos();
  delay(d);

  for (int pos = SERVO_MIN_ANGLE; pos < SERVO_MID_ANGLE; pos += 4)
  {
    for (int i = 0; i < 6; i++)
    {
      setServo(i, pos);
    }
    updateServos();
    delay(10);
  }

  // for (int i = 0; i < 6; i++) {
  //   setServo(i, SERVO_MAX_ANGLE);
  // }
  // updateServos();
  // delay(d);
  //
  // for (int i = 0; i < 6; i++) {
  //   setServo(i, SERVO_MID_ANGLE);
  // }
  // updateServos();
  // delay(d);
}

/**
Setup serial port and add commands.
*/
void setupCommandLine(int bps = 9600)
{
  Serial.begin(bps);
  delay(50);

  Log.begin(LOG_LEVEL, &Serial);

  Log.info("Studuino, v1");
  Log.info("Built %s, %s", __DATE__, __TIME__);
  Log.info("=======================");

#ifdef ENABLE_SERIAL_COMMANDS
  Log.trace("Command-line is ENABLED.");

  shell_init(shell_reader, shell_writer, 0);

  const int c1 = sizeof(commands);
  if (c1 > 0)
  {
    const int c2 = sizeof(commands[0]);
    const int ccount = c1 / c2;

    for (int i = 0; i < ccount; i++)
    {
      Log.trace("Registering command: %s", commands[i].shell_command_string);
      shell_register(commands[i].shell_program, commands[i].shell_command_string);
    }
  }
#else

  Log.trace("Command-line is DISABLED.");

#endif
  delay(100);
}

void setupNunchuck()
{
#ifdef ENABLE_NUNCHUCK
  Log.trace("Nunchuck support is ENABLED.");

  nc.begin();
  if (nc.type == Unknown)
  {
    /** If the device isn't auto-detected, set the type explicatly
     * 	NUNCHUCK,
     WIICLASSIC,
     GuitarHeroController,
     GuitarHeroWorldTourDrums,
     DrumController,
     DrawsomeTablet,
     Turntable
     */

    Log.warning("Controller type NOT AUTO-DETECTED. Set to Nunchuck by default.");
    nc.type = NUNCHUCK;
  }
#else
  Log.trace("Nunchuck support is DISABLED.");
#endif
}

void setup()
{
  Log.setLevel(LOG_LEVEL); // config.h

  pinMode(LED_BUILTIN, OUTPUT); // power indicator
  digitalWrite(LED_BUILTIN, HIGH);

  setupCommandLine(115200);

  setupNunchuck();

  setupPlatform();

  setupTouchscreen();

  setupServos(); // Servos come last, because this setup takes the most time...
}

void loop()
{

#ifdef ENABLE_SERIAL_COMMANDS
  processCommands(); // process any incoming serial commands.
#endif

#ifdef ENABLE_NUNCHUCK
  processNunchuck();
  blinker.loop();
#endif

#ifdef ENABLE_TOUCHSCREEN
  processTouchscreen();
#endif

  updateServos(); // Servos come last, because they take the most time.
}
