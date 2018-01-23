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
#include "servos.h"

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
  const int home[6] = {0, 0, 0, 0, 0, 0};

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

  for(int i=0, j=0;i<sizeof(dval) / sizeof(dval[0]);i++){
    stu.moveTo(sp_servo, dval[i][j++], dval[i][j++], dval[i][j++], dval[i][j++], dval[i][j++], dval[i][j++]);
    updateServos();
    delay(d);
  }

  return SHELL_RET_SUCCESS;
}


/**
set a single servo (#1-6) to the specified uS value.

@param tokens The rest of the input command.
*/
int handleSet(int argc, char** argv)
{
  if (argc == 1) {
    shell_println("Usage: set <servo> min|mid|max|<angle>");
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
      shell_printf("Invalid servo value for Servo #%d. Valid values are (min, mid, max), or a number between %d and %d.\n", srv, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
      shell_println("Usage: set <servo> min|mid|max|<angle>");
    }
  } else {
    shell_println("Invalid Servo number. Servo number must be between 1 and 6.");
    shell_println("Usage: set <servo> min|mid|max|<angle>");
  }
  updateServos();

  return SHELL_RET_SUCCESS;
}

/**
Print some help.

@param tokens The rest of the input command.
*/
int handleHelp(int argc, char** argv)
{
  shell_println("demo      Do a little dance.");
  shell_println("dump      Display information about the system.");
  shell_println("moveTo    Move the platform to the specified pitch / roll (in degrees).");
  shell_println("reset     Restart the system.");
  shell_println("set       Set a specific servo to a specific angle (in degrees).");
  shell_println("setall    Set all servos to a specific angle (in degrees).");
  shell_println("help | ?  This message.");

  return SHELL_RET_SUCCESS;
}

int handleSetAll(int argc, char** argv)
{
  if (argc == 1) {
    shell_println("Usage: set <servo> min|mid|max|<microseconds>");
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
        shell_printf("Invalid servo value for Servo #%d. Valid values are min|mid|max, or a number between %d and %d.\n", i + 1, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
        shell_println("Usage: setall min|mid|max|<microseconds>");
      }
    }
  } else {
    shell_println("Usage: setall min|mid|max|<microseconds>");
  }

  updateServos();
  return SHELL_RET_SUCCESS;
}

int handleDump(int argc, char** argv) {

  shell_println("===== Platform =====");
  shell_printf("platform.sway = %d\n", stu.getSway());
  shell_printf("platform.surge = %d\n", stu.getSurge());
  shell_printf("platform.heave = %d\n", stu.getHeave());
  shell_printf("platform.roll = %d\n", stu.getRoll());
  shell_printf("platform.pitch = %d\n", stu.getPitch());
  shell_printf("platform.yaw = %d\n", stu.getYaw());

  shell_println("\n===== Servos =====");
  for (int i = 0; i < 6; i++) {
    shell_printf("s%d (physical, setpoint) = (%.2f, %.2f)\n", i, servos[i].read(), sp_servo[i]);
  }

  #ifdef ENABLE_NUNCHUCK
  shell_println("\n===== Nunchuck =====");
  shell_printf("nunchuck.ok = %d\n",nc.isOk());
  if(nc.isOk()){
    shell_printf("nunchuck.buttons.c = %d\n",nc.getButtonC());
    shell_printf("nunchuck.buttons.z = %d\n",nc.getButtonC());
    shell_printf("nunchuck.joystick.x = %.2f\n",nc.getJoyX());
    shell_printf("nunchuck.joystick.y = %.2f\n",nc.getJoyY());
    shell_printf("nunchuck.tilt.x = %.2f\n",nc.getTiltX());
    shell_printf("nunchuck.tilt.y = %.2f\n",nc.getTiltY());
    shell_printf("nunchuck.tilt.z = %.2f\n",nc.getTiltZ());
    shell_printf("nunchuck.accel.x = %.2f\n",nc.getAccelX());
    shell_printf("nunchuck.accel.y = %.2f\n",nc.getAccelY());
    shell_printf("nunchuck.accel.z = %.2f\n",nc.getAccelZ());
  }
  #endif

  #ifdef ENABLE_TOUCHSCREEN
  shell_println("\n===== Touch screen =====");

  TSPoint p = ts.getPoint();
  if (p.z > ts.pressureThreshhold) {
    shell_printf("touchscreen.x = %d\n",p.x);
    shell_printf("touchscreen.y = %d\n",p.y);
    shell_printf("touchscreen.z = %d\n",p.z);
  } else {
    shell_println("touchscreen.x = 0\n");
    shell_println("touchscreen.y = 0\n");
    shell_println("touchscreen.z = 0\n");
  }
  #endif

  return SHELL_RET_SUCCESS;
}

int handleReset(int argc, char** argv)
{
  CPU_RESTART;
  return SHELL_RET_SUCCESS; //unreachable?
}

int handleMoveTo(int argc, char** argv) {
  if (argc == 1) {
    shell_println("Usage: moveto home | <pitch angle> <roll angle>");
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
      shell_println("Usage: moveto home | <pitch angle> <roll angle>");
      return SHELL_RET_FAILURE;
    }
    token = argv[2];  //roll
    roll = atof(token);
    stu.moveTo(sp_servo, pitch, roll);
  }

  updateServos();
  return SHELL_RET_SUCCESS;
}

#endif  //ENABLE_SERIAL_COMMANDS
