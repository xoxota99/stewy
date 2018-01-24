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
int shell_reader(char * data)
{
  // Wrapper for Serial.read() method
  if (Serial.available()) {
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

int handleDemo(int argc, char** argv) {
  const int d = 300;

  const int dval[][6]= {
    //sway
    {50, 0, 0, 0, 0, 0},
    {-50, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},

    //surge
    {0, 55, 0, 0, 0, 0},
    {0, -55, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},

    //heave
    {0, 0, 25, 0, 0, 0},
    {0, 0, -22, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},

    //pitch
    {0, 0, 0, 32, 0, 0},
    {0, 0, 0, -29, 0, 0},
    {0, 0, 0, 0, 0, 0},

    //roll
    {0, 0, 0, 0, 30, 0},
    {0, 0, 0, 0, -30, 0},
    {0, 0, 0, 0, 0, 0},

    //yaw
    {0, 0, 0, 0, 0, 69},
    {0, 0, 0, 0, 0, -69},
    {0, 0, 0, 0, 0, 0}
  };

  int ccount=(int)sizeof(dval) / sizeof(dval[0]);

  for(int i=0;i<ccount;i++){
    stu.moveTo(sp_servo, dval[i][0], dval[i][1], dval[i][2], dval[i][3], dval[i][4], dval[i][5]);
    updateServos();
    delay(d);
  }

  return SHELL_RET_SUCCESS;
}


/**
  set a single servo (#1-6) to the specified value in degrees.
*/
int handleSet(int argc, char** argv)
{
  if (argc == 1) {
    Logger::info("Usage: set <servo> min|mid|max|<angle>");
    return SHELL_RET_FAILURE;
  }

  int srv;
  srv = atoi(argv[1]);
  if (srv > 0 && srv < 7) {
    int val;
    if (!strcmp(argv[2], "mid")) {
      val = SERVO_MID_ANGLE;
    } else if (!strcmp(argv[2], "min")) {
      val = SERVO_MIN_ANGLE;
    } else if (!strcmp(argv[2], "max")) {
      val = SERVO_MAX_ANGLE;
    } else {
      val = atoi(argv[2]);
    }


    if (val >= SERVO_MIN_ANGLE && val <= SERVO_MAX_ANGLE) {
      setServo(srv - 1, val);
    } else {
      Logger::info("Invalid servo value for Servo #%d. Valid values are (min, mid, max), or a number between %d and %d.", srv, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
      Logger::info("Usage: set <servo> min|mid|max|<angle>");
    }
  } else {
    Logger::info("Invalid Servo number. Servo number must be between 1 and 6.");
    Logger::info("Usage: set <servo> min|mid|max|<angle>");
  }
  // updateServos();

  return SHELL_RET_SUCCESS;
}

/**
  set a single servo (#1-6) to the specified value in microsecond pulse width.
*/
int handleMSet(int argc, char** argv)
{
  if (argc == 1) {
    Logger::info("Usage: set <servo> min|mid|max|<microseconds>");
    return SHELL_RET_FAILURE;
  }

  int srv;
  srv = atoi(argv[1]);
  if (srv > 0 && srv < 7) {
    int val;
    if (!strcmp(argv[2], "mid")) {
      val = SERVO_MID_US;
    } else if (!strcmp(argv[2], "min")) {
      val = SERVO_MIN_US;
    } else if (!strcmp(argv[2], "max")) {
      val = SERVO_MAX_US;
    } else {
      val = atoi(argv[2]);
    }


    if (val >= SERVO_MIN_US && val <= SERVO_MAX_US) {
      setServoMicros(srv-1,val);
#ifdef ENABLE_SERVOS
      if (SERVO_REVERSE[srv-1]) {
        val = SERVO_MIN_US + (SERVO_MAX_US - val);
      }
      servos[srv-1].writeMicroseconds(min(SERVO_MAX_US, max(SERVO_MIN_US, (int)val + SERVO_TRIM[srv-1])));
#endif
    } else {
      Logger::info("Invalid servo value for Servo #%d. Valid values are (min, mid, max), or a number between %d and %d.", srv, SERVO_MIN_US, SERVO_MAX_US);
      Logger::info("Usage: set <servo> min|mid|max|<angle>");
    }
  } else {
    Logger::info("Invalid Servo number. Servo number must be between 1 and 6.");
    Logger::info("Usage: set <servo> min|mid|max|<angle>");
  }

  return SHELL_RET_SUCCESS;
}

/**
Print some help.
*/
int handleHelp(int argc, char** argv)
{
  Logger::info("demo      Do a little dance.");
  Logger::info("dump      Display information about the system.");
  Logger::info("moveTo    Move the platform to the specified pitch / roll (in degrees).");
  Logger::info("reset     Restart the system.");
  Logger::info("set       Set a specific servo to a specific angle (in degrees).");
  Logger::info("mset      Set a specific servo to a specific angle (in microseconds).");
  Logger::info("setall    Set all servos to a specific angle (in degrees).");
  Logger::info("msetall   Set all servos to a specific angle (in microseconds).");
  Logger::info("log       Set the log level.");
  Logger::info("help | ?  This message.");

  return SHELL_RET_SUCCESS;
}

int handleSetAll(int argc, char** argv)
{
  if (argc == 1) {
    Logger::info("Usage: setall <servo> min|mid|max|<angle>");
    return SHELL_RET_FAILURE;
  }

  char* token = argv[1];
  int val = 0;

  if (token != NULL) {
    bool special = false;

    if (strncmp(token, "mid", 3) != 0 &&
    strncmp(token, "min", 3) != 0 &&
    strncmp(token, "max", 3) != 0) {
      val = atoi(token);
    } else {
      special = true;
    }

    for (int i = 0; i < 6; i++) {

      if (special) {
        if (strncmp(token, "mid", 3) == 0) {
          val = SERVO_MID_ANGLE;
        } else if (strncmp(token, "min", 3) == 0) {
          val = SERVO_MIN_ANGLE;
        } else if (strncmp(token, "max", 3) == 0) {
          val = SERVO_MAX_ANGLE;
        }
      }

      if (val >= SERVO_MIN_ANGLE && val <= SERVO_MAX_ANGLE) {
        setServo(i, val);
      } else {
        Logger::info("Invalid servo value for Servo #%d. Valid values are min|mid|max, or a number between %d and %d.", i + 1, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
        Logger::info("Usage: setall min|mid|max|<angle>");
      }
    }
  } else {
    Logger::info("Usage: setall min|mid|max|<angle>");
  }

  // updateServos();
  return SHELL_RET_SUCCESS;
}


int handleMSetAll(int argc, char** argv)
{
  if (argc == 1) {
    Logger::info("Usage: msetall min|mid|max|<microseconds>");
    return SHELL_RET_FAILURE;
  }

  char* token = argv[1];
  int val = 0;

  if (token != NULL) {
    bool special = false;

    if (strncmp(token, "mid", 3) != 0 &&
      strncmp(token, "min", 3) != 0 &&
      strncmp(token, "max", 3) != 0) {
      val = atoi(token);
    } else {
      special = true;
    }

    for (int i = 0; i < 6; i++) {

      if (special) {
        if (strncmp(token, "mid", 3) == 0) {
          val = SERVO_MID_US;
        } else if (strncmp(token, "min", 3) == 0) {
          val = SERVO_MIN_US;
        } else if (strncmp(token, "max", 3) == 0) {
          val = SERVO_MAX_US;
        }
      }

      if (val >= SERVO_MIN_US && val <= SERVO_MAX_US) {
        setServoMicros(i,val);
#ifdef ENABLE_SERVOS
        if (SERVO_REVERSE[i]) {
          val = SERVO_MIN_US + (SERVO_MAX_US - val);
        }
        servos[i].writeMicroseconds(min(SERVO_MAX_US, max(SERVO_MIN_US, (int)val + SERVO_TRIM[i])));
#endif
      } else {
        Logger::info("Invalid servo value for Servo #%d. Valid values are min|mid|max, or a number between %d and %d.", i + 1, SERVO_MIN_US, SERVO_MAX_US);
        Logger::info("Usage: msetall min|mid|max|<microseconds>");
      }
    }
  } else {
    Logger::info("Usage: msetall min|mid|max|<microseconds>");
  }

  // updateServos();
  return SHELL_RET_SUCCESS;
}

int handleDump(int argc, char** argv) {

  Logger::info("===== Platform =====");
  Logger::info("platform.sway = %d", stu.getSway());
  Logger::info("platform.surge = %d", stu.getSurge());
  Logger::info("platform.heave = %d", stu.getHeave());
  Logger::info("platform.roll = %.2f", stu.getRoll());
  Logger::info("platform.pitch = %.2f", stu.getPitch());
  Logger::info("platform.yaw = %.2f", stu.getYaw());

  Logger::info("\n===== Servos =====");
  for (int i = 0; i < 6; i++) {
#ifdef ENABLE_SERVOS
    Logger::info("s%d (physical, setpoint, us) = (%d, %.2f, %.2f)", i, servos[i].read(), sp_servo[i], _toUs(servos[i].read()));
#else
    Logger::info("s%d (physical, setpoint, us) = (N/A, %.2f, %.2f)", i, sp_servo[i], _toUs(servos[i].read()));
#endif
  }

#ifdef ENABLE_NUNCHUCK
  Logger::info("\n===== Nunchuck =====");
  Logger::info("nunchuck.ok = %d",nc.isOk());
  if(nc.isOk()){
    Logger::info("nunchuck.buttons.c = %s",nc.getButtonC()?"true":"false");
    Logger::info("nunchuck.buttons.z = %s",nc.getButtonC()?"true":"false");
    Logger::info("nunchuck.joystick.x = %d",nc.getJoyX());
    Logger::info("nunchuck.joystick.y = %d",nc.getJoyY());
    Logger::info("nunchuck.tilt.x = %.2f",nc.getTiltX());
    Logger::info("nunchuck.tilt.y = %.2f",nc.getTiltY());
    Logger::info("nunchuck.tilt.z = %.2f",nc.getTiltZ());
    Logger::info("nunchuck.accel.x = %d",nc.getAccelX());
    Logger::info("nunchuck.accel.y = %d",nc.getAccelY());
    Logger::info("nunchuck.accel.z = %d",nc.getAccelZ());
  }
  #endif

#ifdef ENABLE_TOUCHSCREEN
  Logger::info("\n===== Touch screen =====");

  TSPoint p = ts.getPoint();

  Logger::info("touchscreen.x = %d",p.x);
  Logger::info("touchscreen.y = %d",p.y);
  Logger::info("touchscreen.z = %d",p.z);
  #endif

  return SHELL_RET_SUCCESS;
}

int handleReset(int argc, char** argv)
{
  CPU_RESTART;
  return SHELL_RET_SUCCESS; //unreachable?
}

int handleLog(int argc, char** argv)
{
  if (argc != 2) {
    Logger::info("Usage: log [TRACE | DEBUG | INFO | WARN | ERROR | FATAL]");
    return SHELL_RET_FAILURE;
  }

  char* token = argv[1];

  if (strncmp(token, "TRACE", 5) == 0) {
    Logger::level = Logger::TRACE;
  } else if (strncmp(token, "DEBUG", 5) == 0) {
    Logger::level = Logger::DEBUG;
  } else if (strncmp(token, "INFO", 4) == 0) {
    Logger::level = Logger::INFO;
  } else if (strncmp(token, "WARN", 4) == 0) {
    Logger::level = Logger::WARN;
  } else if (strncmp(token, "ERROR", 5) == 0) {
    Logger::level = Logger::ERROR;
  } else if (strncmp(token, "FATAL", 5) == 0) {
    Logger::level = Logger::FATAL;
  } else {
    Logger::info("Usage: log [TRACE | DEBUG | INFO | WARN | ERROR | FATAL]");
    return SHELL_RET_FAILURE;
  }

  return SHELL_RET_SUCCESS;
}

int handleMoveTo(int argc, char** argv) {
  if (argc == 1) {
    Logger::info("Usage: moveto home | <pitch angle> <roll angle>");
    return SHELL_RET_FAILURE;
  }

  double pitch, roll;
  char* token = argv[1];  //pitch

  if (strncmp(token, "home", 4) == 0 ||
  strncmp(token, "zero", 4) == 0) {
    stu.home(sp_servo);
  } else {
    pitch = atof(token);
    if (argc < 3) {
      Logger::info("Usage: moveto home | <pitch angle> <roll angle>");
      return SHELL_RET_FAILURE;
    }
    token = argv[2];  //roll
    roll = atof(token);
    stu.moveTo(sp_servo, pitch, roll);
  }

  // updateServos();
  return SHELL_RET_SUCCESS;
}

#endif  //ENABLE_SERIAL_COMMANDS
