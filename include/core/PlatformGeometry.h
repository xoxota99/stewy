#pragma once
/**
 * @file PlatformGeometry.h
 * @brief Geometry definitions for the Stewart platform
 *
 * This file contains the geometric constants and calculations
 * for the Stewart platform.
 *
 * @author Philippe Desrosiers
 * @copyright Copyright (C) 2018 Philippe Desrosiers
 * @license GPL-3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <Arduino.h>

namespace stewy
{
  namespace core
  {

// Movement limits for the platform
#define SWAY_MIN -50  // Minimum sway (x) in mm
#define SWAY_MAX 50   // Maximum sway (x) in mm
#define SURGE_MIN -50 // Minimum surge (y) in mm
#define SURGE_MAX 50  // Maximum surge (y) in mm
#define HEAVE_MIN -30 // Minimum heave (z) in mm
#define HEAVE_MAX 30  // Maximum heave (z) in mm
#define PITCH_MIN -30 // Minimum pitch in degrees
#define PITCH_MAX 30  // Maximum pitch in degrees
#define ROLL_MIN -30  // Minimum roll in degrees
#define ROLL_MAX 30   // Maximum roll in degrees
#define YAW_MIN -30   // Minimum yaw in degrees
#define YAW_MAX 30    // Maximum yaw in degrees

// Enhanced IK algorithm options
#define USE_ENHANCED_IK         // Use the enhanced IK algorithm for better range of movement
#define ROTATION_POINT_OFFSET 0 // Height offset for rotation point in mm (0 = rotate around home position)
#define TRANSLATION_FIRST true  // Apply translation before rotation for better range

/*
  If defined, the IK algorithm will "slam" values to min or max when it encounters
  an asymptotic condition. That is, if the solution requires that the servo (e.g.) extend
  beyond its physical limits, it will set the servo angle to MAX.

  If NOT defined, the IK algorithm will simply refuse to modify ANY servo endpoints
  when it encounters an asymptotic condition.
*/
// #define SLAM

/*
  Prescalar to the output of the platform IK solution for each servo.
  NOTE: Even with aggro, the solution will never fall outside the range of
  [SERVO_ANGLE_MIN .. SERVO_ANGLE_MAX]
*/
#define AGGRO 1.5F

    //==============================================================================
    // Geometry of the platform.

#define THETA_P_DEG 45.25                // Platform joint angle (degrees) offset from AXIS[1|2|3]. A value of zero puts these joints directly on the axes
#define THETA_B_DEG 24.5                 // Base Servo pinion angle (degrees) offset from AXIS[1|2|3]. A value of zero puts the servo pinion directly on the axes
#define THETA_P (THETA_P_DEG * PI / 180) // Theta P, in radians
#define THETA_B (THETA_B_DEG * PI / 180) // Theta B, in radians
#define P_RAD 50                         // Platform radius (mm). The distance from the center of the platform to the center of one platform / pushrod "joint". This should be the same for all six pushrods.
#define B_RAD 80.2                       // Base radius (mm). Distance from the center of the base plate to the center of one servo pinion gear. Again, this should be the same for all six servos.
#define ARM_LENGTH 25                    // Servo arm length (mm). Distance from the center of the servo pivot to the center of the pushrod pivot on the servo arm.
#define ROD_LENGTH 155                   // Push rod length (mm). Distance between pushrod ball joints (servo to platform).
#define Z_HOME 148                       // Default Z height of the platform (above the base), with servo arms horizontal. Formally, the distance from the plane described by the collection of servo pinion gear centers, to the plane described by the collection of platform / pushrod joints.

    /*
       There are three axes of symmetry (AXIS1, AXIS2, AXIS3). Looking down on the
       platform from above (along the Y axis), with 0 degrees being the X-positive line, and traveling
       in a CC direction, these are at 30 degrees, 120 degrees, and 240 degrees. All
       the polar coordinates of pivot points, servo centers, etc. are calculated based on
       an axis, and an offset angle (positive or negative theta) from the axis.
     */

#define AXIS1 (PI / 6)  // 30 degrees.
#define AXIS2 (-PI / 2) //-90 degrees.
/*
   NOTE: We make an assumption of mirror symmetry for AXIS3 along the Y axis.
   That is, AXIS1 is at (e.g.) 30 degrees, and AXIS3 will be at 120 degrees
   We account for this by negating the value of x-coordinates generated based
   on this axis later on. This is potentially messy, and should maybe be refactored.
 */
#define AXIS3 AXIS1

    /*
       Absolute angle that the servo arm plane of rotation is at (degrees), from the world-X axis.
    */
    const double THETA_S_DEG[6] = {
        -60,
        120,
        180,
        0,
        60,
        -120};

    const double THETA_S[6] = { // Servo arm angle (radians)
        radians(THETA_S_DEG[0]),
        radians(THETA_S_DEG[1]),
        radians(THETA_S_DEG[2]),
        radians(THETA_S_DEG[3]),
        radians(THETA_S_DEG[4]),
        radians(THETA_S_DEG[5])};

    /*
       XY cartesian coordinates of the platform joints, based on the polar
       coordinates (platform radius P_RAD, radial axis AXIS[1|2\3], and offset THETA_P.
       These coordinates are in the plane of the platform itself.
     */
    const double P_COORDS[6][2] = {
        {P_RAD * cos(AXIS1 + THETA_P), P_RAD *sin(AXIS1 + THETA_P)},
        {P_RAD * cos(AXIS1 - THETA_P), P_RAD *sin(AXIS1 - THETA_P)},
        {P_RAD * cos(AXIS2 + THETA_P), P_RAD *sin(AXIS2 + THETA_P)},
        {-P_RAD * cos(AXIS2 + THETA_P), P_RAD *sin(AXIS2 + THETA_P)},
        {-P_RAD * cos(AXIS3 - THETA_P), P_RAD *sin(AXIS3 - THETA_P)},
        {-P_RAD * cos(AXIS3 + THETA_P), P_RAD *sin(AXIS3 + THETA_P)}};

    /*
       XY cartesian coordinates of the servo centers, based on the polar
       coordinates (base radius B_RAD, radial axis AXIS[1|2\3], and offset THETA_B.
       These coordinates are in the plane of the base itself.
     */
    const double B_COORDS[6][2] = {
        {B_RAD * cos(AXIS1 + THETA_B), B_RAD *sin(AXIS1 + THETA_B)},
        {B_RAD * cos(AXIS1 - THETA_B), B_RAD *sin(AXIS1 - THETA_B)},
        {B_RAD * cos(AXIS2 + THETA_B), B_RAD *sin(AXIS2 + THETA_B)},
        {-B_RAD * cos(AXIS2 + THETA_B), B_RAD *sin(AXIS2 + THETA_B)},
        {-B_RAD * cos(AXIS3 - THETA_B), B_RAD *sin(AXIS3 - THETA_B)},
        {-B_RAD * cos(AXIS3 + THETA_B), B_RAD *sin(AXIS3 + THETA_B)}};

  } // namespace core
} // namespace stewy
