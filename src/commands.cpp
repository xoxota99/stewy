#ifdef ENABLE_SERIAL_COMMANDS
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

#include "config.h"
#include "commands.h"

void processCommands()
{
  // This should always be called to process user input
  shell_task();
}

/**
Function to read data from serial port
Functions to read from physical media should use this prototype:
int my_reader_function(char * data)
*/
int shell_reader(char *data)
{
  // Wrapper for Serial.read() method
  if (Serial.available())
  {
    *data = Serial.read();
    return 1;
  }
  return 0;
}

/**
  Function to write data to serial port
  Functions to write to physical media should use this prototype
  void my_writer_function(char data)
*/
void shell_writer(char data)
{
  Serial.write(data);
}

int handleDemo(int argc, char **argv)
{
  const int d = 500;

  const int dval[][6] = {
      // sway
      {MAX_SWAY, 0, 0, 0, 0, 0},
      {MIN_SWAY, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0},

      // surge
      {0, MAX_SURGE, 0, 0, 0, 0},
      {0, MIN_SURGE, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0},

      // heave
      {0, 0, MAX_HEAVE, 0, 0, 0},
      {0, 0, MIN_HEAVE, 0, 0, 0},
      {0, 0, 0, 0, 0, 0},

      // pitch
      {0, 0, 0, MAX_PITCH, 0, 0},
      {0, 0, 0, MIN_PITCH, 0, 0},
      {0, 0, 0, 0, 0, 0},

      // roll
      {0, 0, 0, 0, MAX_ROLL, 0},
      {0, 0, 0, 0, MIN_ROLL, 0},
      {0, 0, 0, 0, 0, 0},

      // yaw
      {0, 0, 0, 0, 0, MAX_YAW},
      {0, 0, 0, 0, 0, MIN_YAW},
      {0, 0, 0, 0, 0, 0}};

  int ccount = (int)sizeof(dval) / sizeof(dval[0]);

  for (int i = 0; i < ccount; i++)
  {
    stu.moveTo(sp_servo, dval[i][0], dval[i][1], dval[i][2], dval[i][3], dval[i][4], dval[i][5]);
    updateServos();
    delay(d);
  }

  return SHELL_RET_SUCCESS;
}

/**
  set a single servo (#1-6) to the specified value in degrees.
*/
int handleSet(int argc, char **argv)
{
  if (argc == 1)
  {
    Log.info("Usage: set <servo> min|mid|max|<angle>");
    return SHELL_RET_FAILURE;
  }

  int srv;
  srv = atoi(argv[1]);
  if (srv > 0 && srv < 7)
  {
    int val;
    if (!strcmp(argv[2], "mid"))
    {
      val = SERVO_MID_ANGLE;
    }
    else if (!strcmp(argv[2], "min"))
    {
      val = SERVO_MIN_ANGLE;
    }
    else if (!strcmp(argv[2], "max"))
    {
      val = SERVO_MAX_ANGLE;
    }
    else
    {
      val = atoi(argv[2]);
    }

    if (val >= SERVO_MIN_ANGLE && val <= SERVO_MAX_ANGLE)
    {
      setServo(srv - 1, val);
    }
    else
    {
      Log.info("Invalid servo value for Servo #%d. Valid values are (min, mid, max), or a number between %d and %d.", srv, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
      Log.info("Usage: set <servo> min|mid|max|<angle>");
    }
  }
  else
  {
    Log.info("Invalid Servo number. Servo number must be between 1 and 6.");
    Log.info("Usage: set <servo> min|mid|max|<angle>");
  }
  // updateServos();

  return SHELL_RET_SUCCESS;
}

/**
  set a single servo (#1-6) to the specified value in microsecond pulse width.
*/
int handleMSet(int argc, char **argv)
{
  if (argc == 1)
  {
    Log.info("Usage: set <servo> min|mid|max|<microseconds>");
    return SHELL_RET_FAILURE;
  }

  int srv;
  srv = atoi(argv[1]);
  if (srv > 0 && srv < 7)
  {
    int val;
    if (!strcmp(argv[2], "mid"))
    {
      val = SERVO_MID_US;
    }
    else if (!strcmp(argv[2], "min"))
    {
      val = SERVO_MIN_US;
    }
    else if (!strcmp(argv[2], "max"))
    {
      val = SERVO_MAX_US;
    }
    else
    {
      val = atoi(argv[2]);
    }

    if (val >= SERVO_MIN_US && val <= SERVO_MAX_US)
    {
      setServoMicros(srv - 1, val);
#ifdef ENABLE_SERVOS
      if (SERVO_REVERSE[srv - 1])
      {
        val = SERVO_MIN_US + (SERVO_MAX_US - val);
      }
      servos[srv - 1].writeMicroseconds((int)constrain(val + SERVO_TRIM[srv - 1], SERVO_MIN_US, SERVO_MAX_US));
#endif
    }
    else
    {
      Log.info("Invalid servo value for Servo #%d. Valid values are (min, mid, max), or a number between %d and %d.", srv, SERVO_MIN_US, SERVO_MAX_US);
      Log.info("Usage: set <servo> min|mid|max|<angle>");
    }
  }
  else
  {
    Log.info("Invalid Servo number. Servo number must be between 1 and 6.");
    Log.info("Usage: set <servo> min|mid|max|<angle>");
  }

  return SHELL_RET_SUCCESS;
}

/**
Print some help.
*/
int handleHelp(int argc, char **argv)
{
  int size = *(&commands + 1) - commands;
  for (int i = 0; i < size; i++)
  {
    Log.info("%s\t\t%s", commands[i].shell_command_string, commands[i].shell_help_string);
  }
  return SHELL_RET_SUCCESS;
}

int handleSetAll(int argc, char **argv)
{
  if (argc == 1)
  {
    Log.info("Usage: setall <servo> min|mid|max|<angle>");
    return SHELL_RET_FAILURE;
  }

  char *token = argv[1];
  int val = 0;

  if (token != NULL)
  {
    bool special = false;

    if (strncmp(token, "mid", 3) != 0 &&
        strncmp(token, "min", 3) != 0 &&
        strncmp(token, "max", 3) != 0)
    {
      val = atoi(token);
    }
    else
    {
      special = true;
    }

    for (int i = 0; i < 6; i++)
    {

      if (special)
      {
        if (strncmp(token, "mid", 3) == 0)
        {
          val = SERVO_MID_ANGLE;
        }
        else if (strncmp(token, "min", 3) == 0)
        {
          val = SERVO_MIN_ANGLE;
        }
        else if (strncmp(token, "max", 3) == 0)
        {
          val = SERVO_MAX_ANGLE;
        }
      }

      if (val >= SERVO_MIN_ANGLE && val <= SERVO_MAX_ANGLE)
      {
        setServo(i, val);
      }
      else
      {
        Log.info("Invalid servo value for Servo #%d. Valid values are min|mid|max, or a number between %d and %d.", i + 1, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
        Log.info("Usage: setall min|mid|max|<angle>");
      }
    }
  }
  else
  {
    Log.info("Usage: setall min|mid|max|<angle>");
  }

  // updateServos();
  return SHELL_RET_SUCCESS;
}

int handleMSetAll(int argc, char **argv)
{
  if (argc == 1)
  {
    Log.info("Usage: msetall min|mid|max|<microseconds>");
    return SHELL_RET_FAILURE;
  }

  char *token = argv[1];
  int val = 0;

  if (token != NULL)
  {
    bool special = false;

    if (strncmp(token, "mid", 3) != 0 &&
        strncmp(token, "min", 3) != 0 &&
        strncmp(token, "max", 3) != 0)
    {
      val = atoi(token);
    }
    else
    {
      special = true;
    }

    for (int i = 0; i < 6; i++)
    {

      if (special)
      {
        if (strncmp(token, "mid", 3) == 0)
        {
          val = SERVO_MID_US;
        }
        else if (strncmp(token, "min", 3) == 0)
        {
          val = SERVO_MIN_US;
        }
        else if (strncmp(token, "max", 3) == 0)
        {
          val = SERVO_MAX_US;
        }
      }

      if (val >= SERVO_MIN_US && val <= SERVO_MAX_US)
      {
        setServoMicros(i, val);
#ifdef ENABLE_SERVOS
        if (SERVO_REVERSE[i])
        {
          val = SERVO_MIN_US + (SERVO_MAX_US - val);
        }
        servos[i].writeMicroseconds((int)constrain(val + SERVO_TRIM[i], SERVO_MIN_US, SERVO_MAX_US));
#endif
      }
      else
      {
        Log.info("Invalid servo value for Servo #%d. Valid values are min|mid|max, or a number between %d and %d.", i + 1, SERVO_MIN_US, SERVO_MAX_US);
        Log.info("Usage: msetall min|mid|max|<microseconds>");
      }
    }
  }
  else
  {
    Log.info("Usage: msetall min|mid|max|<microseconds>");
  }

  // updateServos();
  return SHELL_RET_SUCCESS;
}

int handleDump(int argc, char **argv)
{

  Log.info("===== Config =====");
  Log.info("px = %.3f", PX);
  Log.info("ix = %.3f", IX);
  Log.info("dx = %.3f", DX);
  Log.info("py = %.3f", PY);
  Log.info("iy = %.3f", IY);
  Log.info("dy = %.3f", DY);

  Log.info("===== Platform =====");
  Log.info("platform.sway = %d", stu.getSway());
  Log.info("platform.surge = %d", stu.getSurge());
  Log.info("platform.heave = %d", stu.getHeave());
  Log.info("platform.roll = %.2f", stu.getRoll());
  Log.info("platform.pitch = %.2f", stu.getPitch());
  Log.info("platform.yaw = %.2f", stu.getYaw());

  Log.info("\n===== Servos =====");
  for (int i = 0; i < 6; i++)
  {
#ifdef ENABLE_SERVOS
    Log.info("s%d (physical, setpoint, us) = (%d, %.2f, %.2f)", i, servos[i].read(), sp_servo[i], _toUs(servos[i].read()));
#else
    Log.info("s%d (physical, setpoint, us) = (N/A, %.2f, N/A)", i, sp_servo[i]);
#endif
  }

#ifdef ENABLE_NUNCHUCK
  Log.info("\n===== Nunchuck =====");
  Log.info("nunchuck.ok = %d", nc.isOk());
  if (nc.isOk())
  {
    Log.info("nunchuck.buttons.c = %s", nc.getButtonC() ? "true" : "false");
    Log.info("nunchuck.buttons.z = %s", nc.getButtonC() ? "true" : "false");
    Log.info("nunchuck.joystick.x = %d", nc.getJoyX());
    Log.info("nunchuck.joystick.y = %d", nc.getJoyY());
    Log.info("nunchuck.tilt.x = %.2f", nc.getTiltX());
    Log.info("nunchuck.tilt.y = %.2f", nc.getTiltY());
    Log.info("nunchuck.tilt.z = %.2f", nc.getTiltZ());
    Log.info("nunchuck.accel.x = %d", nc.getAccelX());
    Log.info("nunchuck.accel.y = %d", nc.getAccelY());
    Log.info("nunchuck.accel.z = %d", nc.getAccelZ());
  }
#endif

#ifdef ENABLE_TOUCHSCREEN
  Log.info("\n===== Touch screen =====");

  TSPoint p = ts.getPoint();

  Log.info("touchscreen.x = %d", p.x);
  Log.info("touchscreen.y = %d", p.y);
  Log.info("touchscreen.z = %d", p.z);
#endif

  return SHELL_RET_SUCCESS;
}

int handleReset(int argc, char **argv)
{
  CPU_RESTART;
  return SHELL_RET_SUCCESS; // unreachable?
}

int handleLog(int argc, char **argv)
{
  if (argc != 2)
  {
    Log.info("Usage: log [SILENT | VERBOSE | TRACE | INFO | WARNING | ERROR | FATAL]");
    return SHELL_RET_FAILURE;
  }

  char *token = argv[1];

  if (strncmp(token, "SILENT", 6) == 0)
  {
    Log.setLevel(LOG_LEVEL_SILENT);
  }
  if (strncmp(token, "VERBOSE", 7) == 0)
  {
    Log.setLevel(LOG_LEVEL_VERBOSE);
  }
  else if (strncmp(token, "TRACE", 5) == 0)
  {
    Log.setLevel(LOG_LEVEL_TRACE);
  }
  else if (strncmp(token, "INFO", 4) == 0)
  {
    Log.setLevel(LOG_LEVEL_INFO);
  }
  else if (strncmp(token, "WARNING", 7) == 0)
  {
    Log.setLevel(LOG_LEVEL_WARNING);
  }
  else if (strncmp(token, "ERROR", 5) == 0)
  {
    Log.setLevel(LOG_LEVEL_ERROR);
  }
  else if (strncmp(token, "FATAL", 5) == 0)
  {
    Log.setLevel(LOG_LEVEL_FATAL);
  }
  else
  {
    Log.info("Usage: log [SILENT | VERBOSE | TRACE | INFO | WARNING | ERROR | FATAL]");
    return SHELL_RET_FAILURE;
  }

  return SHELL_RET_SUCCESS;
}

int handleMoveTo(int argc, char **argv)
{
  if (argc == 1)
  {
    Log.info("Usage: moveto home | <pitch angle> <roll angle>");
    return SHELL_RET_FAILURE;
  }

  double pitch, roll;
  char *token = argv[1]; // pitch

  if (strncmp(token, "home", 4) == 0 ||
      strncmp(token, "zero", 4) == 0)
  {
    stu.home(sp_servo);
  }
  else
  {
    pitch = atof(token);
    if (argc < 3)
    {
      Log.info("Usage: moveto home | <pitch angle> <roll angle>");
      return SHELL_RET_FAILURE;
    }
    token = argv[2]; // roll
    roll = atof(token);
    stu.moveTo(sp_servo, pitch, roll);
  }

  // updateServos();
  return SHELL_RET_SUCCESS;
}

#ifdef ENABLE_TOUCHSCREEN
int handlePID(int argc, char **argv)
{
  char *cmd = argv[0];
  if (argc != 2)
  {
    Log.info("Usage: %s <value>", cmd);
    return SHELL_RET_FAILURE;
  }

  // char* token = argv[1];
  double val = atof(argv[1]);
  if (strncmp(cmd, "px", 2) == 0)
  {
    PX = val;
    rollPID.SetTunings(PX, IX, DX);
  }
  else if (strncmp(cmd, "ix", 2) == 0)
  {
    IX = val;
    rollPID.SetTunings(PX, IX, DX);
  }
  else if (strncmp(cmd, "dx", 2) == 0)
  {
    DX = val;
    rollPID.SetTunings(PX, IX, DX);
  }
  else if (strncmp(cmd, "py", 2) == 0)
  {
    PY = val;
    pitchPID.SetTunings(PY, IY, DY);
  }
  else if (strncmp(cmd, "iy", 2) == 0)
  {
    IY = val;
    pitchPID.SetTunings(PY, IY, DY);
  }
  else if (strncmp(cmd, "dy", 2) == 0)
  {
    DY = val;
    pitchPID.SetTunings(PY, IY, DY);
  }
  return SHELL_RET_SUCCESS;
}
#endif

#endif // ENABLE_SERIAL_COMMANDS
