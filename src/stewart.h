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

/*
* Shamelessly stolen from the work of Daniel Waters, https://www.youtube.com/watch?v=1jrP3_1ML9M
*/

#include "config.h"
#include <math.h>

#ifndef degrees
#define degrees(a) (a * 180.0 / M_PI)
#endif

#ifndef radians
#define radians(a) (a * M_PI / 180.0)
#endif

#ifndef STEWART_H
#define STEWART_H

/*
  TODO: We make an assumption of mirror symmetry for AXIS3 along the Y axis.
  That is, AXIS1 is at (e.g.) 30 degrees, and AXIS3 will be at 120 degrees
  We account for this by negating the value of x-coordinates generated based
  on this axis later on. This is potentially messy, and should maybe be refactored.
*/
#define AXIS3             AXIS1

const double THETA_S[6] = {     //Servo arm angle (radians)
  radians(THETA_S_DEG[0]),
  radians(THETA_S_DEG[1]),
  radians(THETA_S_DEG[2]),
  radians(THETA_S_DEG[3]),
  radians(THETA_S_DEG[4]),
  radians(THETA_S_DEG[5])
};

const double THETA_P = radians(THETA_P_DEG);  //Theta P, in radians
const double THETA_B = radians(THETA_B_DEG);  //Theta B, in radians

/*
  XY cartesian coordinates of the platform joints, based on the polar
  coordinates (platform radius P_RAD, radial axis AXIS[1|2\3], and offset THETA_P.
  These coordinates are in the plane of the platform itself.
*/
const double P_COORDS[6][2] = {
  {P_RAD * cos(AXIS1 + THETA_P), P_RAD * sin(AXIS1 + THETA_P)},
  {P_RAD * cos(AXIS1 - THETA_P), P_RAD * sin(AXIS1 - THETA_P)},
  {P_RAD * cos(AXIS2 + THETA_P), P_RAD * sin(AXIS2 + THETA_P)},
  { -P_RAD * cos(AXIS2 + THETA_P), P_RAD * sin(AXIS2 + THETA_P)},
  { -P_RAD * cos(AXIS3 - THETA_P), P_RAD * sin(AXIS3 - THETA_P)},
  { -P_RAD * cos(AXIS3 + THETA_P), P_RAD * sin(AXIS3 + THETA_P)}
};

/*
  XY cartesian coordinates of the servo centers, based on the polar
  coordinates (base radius B_RAD, radial axis AXIS[1|2\3], and offset THETA_B.
  These coordinates are in the plane of the base itself.
*/
const double B_COORDS[6][2] = {
  {B_RAD * cos(AXIS1 + THETA_B), B_RAD * sin(AXIS1 + THETA_B)},
  {B_RAD * cos(AXIS1 - THETA_B), B_RAD * sin(AXIS1 - THETA_B)},
  {B_RAD * cos(AXIS2 + THETA_B), B_RAD * sin(AXIS2 + THETA_B)},
  { -B_RAD * cos(AXIS2 + THETA_B), B_RAD * sin(AXIS2 + THETA_B)},
  { -B_RAD * cos(AXIS3 - THETA_B), B_RAD * sin(AXIS3 - THETA_B)},
  { -B_RAD * cos(AXIS3 + THETA_B), B_RAD * sin(AXIS3 + THETA_B)}
};

//==============================================================================

class Stewart {

private:
  //Setpoints (internal state)
  int _sp_sway = 0,     //sway (x) in mm
      _sp_surge = 0,    //surge (y) in mm
      _sp_heave = 0;    //heave (z) in mm

  float _sp_pitch = 0,  //pitch (x) in radians
      _sp_roll = 0,     //roll (y) in radians
      _sp_yaw = 0;      //yaw (z) in radians


public:
  bool home(float *servoValues) {
    return moveTo (servoValues, 0, 0, 0, 0, 0, 0);  //HOME position. No rotation, no translation.
  }

  bool moveTo(float *servoValues, int sway, int surge, int heave, float pitch, float roll, float yaw) {

    /*
    * TODO:
    * 1) Understand the "geometry shenanigans" below (k, l, m values).
    * 2) optimize so we don't run through this loop if we know we're already at the desired setpoint(s).
    */

    double pivot_x, pivot_y, pivot_z,       //Global XYZ coordinates of platform pivot points
    d2,                             //Distance^2 between servo pivot and platform link.
    k, l, m,                        //intermediate values
    servo_rad;                      //Angle (radians) to turn each servo.

    float oldValues[6];
    // intermediate values, to avoid recalculating SIN / COS
    double cr=cos(roll),
    cp=cos(pitch),
    cy=cos(yaw),
    sr=sin(roll),
    sp=sin(pitch),
    sy=sin(yaw);

    for (int i = 0; i < 6; i++) {
      oldValues[i]=servoValues[i];
    }

    bool bOk=true;

    for (int i = 0; i < 6; i++) {
      pivot_x = P_COORDS[i][0] * cr * cy + P_COORDS[i][1] * (sp * sr * cr - cp * sy) + sway;
      pivot_y = P_COORDS[i][0] * cr * sy + P_COORDS[i][1] * (cp * cy + sp * sr * sy) + surge;
      pivot_z = -P_COORDS[i][0] * sr + P_COORDS[i][1] * sp * cr + Z_HOME + heave;

      d2 = pow(pivot_x - B_COORDS[i][0], 2) + pow(pivot_y - B_COORDS[i][1], 2) + pow(pivot_z, 2);

      //Geometry shenanigans
      k = d2 - (pow(ROD_LENGTH, 2) - pow(ARM_LENGTH, 2));
      l = 2 * ARM_LENGTH * pivot_z;
      m = 2 * ARM_LENGTH * (cos(THETA_S[i]) * (pivot_x - B_COORDS[i][0]) + sin(THETA_S[i]) * (pivot_y - B_COORDS[i][1]));

      if (sqrt(d2)>(ARM_LENGTH+ROD_LENGTH)  //the required virtual arm length is longer than physically possible
          || abs(k/(sqrt(l*l+m*m)))>=1) {   //some other bad stuff happened.
        //bad juju.
        bOk=false;
        break;
      }

      servo_rad = asin(k / sqrt(l * l + m * m)) - atan(m / l);
      servoValues[i] = degrees(servo_rad) * SERVO_REVERSE[i];
    }

    if(bOk){
      _sp_sway = sway;
      _sp_surge = surge;
      _sp_heave = heave;
      _sp_pitch = pitch;
      _sp_roll = roll;
      _sp_yaw = yaw;
    } else {
      //reset back to old values.
      for (int i = 0; i < 6; i++) {
        servoValues[i]=oldValues[i];
      }
    }

    return bOk;
  }

  bool moveTo(float *servoValues, float pitch, float roll) {
    return moveTo(servoValues, _sp_sway, _sp_surge, _sp_heave, pitch, roll, _sp_yaw);
  }

};
#endif
