/*******************************************************
* DRIVE CLASS
* DESC: The drive class provides a high-level interface to 
* a motor drive system. This allows the user to turn, go forward,
* reverse and perform other manuevers. 
* Author: Jonathan L Clark
* Date: 10/17/2020
*******************************************************/

#include "Drive.h"
#include "Motor.h"
#include <Arduino.h>

/****************************************
* DRIVE CONSTRUCTOR
****************************************/
Drive::Drive(int leftFrontA, int leftFrontB, int leftBackA, int leftBackB, int rightFrontA, int rightFrontB, int rightBackA, int rightBackB)
{
    //steering = inSteering;
    leftFront.Setup(leftFrontA, leftFrontB, 1.0, 0.0);
    leftBack.Setup(leftBackA, leftBackB, 1.0, 0.0);
    rightFront.Setup(rightFrontA, rightFrontB, 1.0, 0.0);
    rightBack.Setup(rightBackA, rightBackB, 1.0, 0.0);
}

/****************************************
* DRIVE MANUAL CONTROL
* DESC: Takes a joystick y and x value and
* uses that to manually control the vehicle.
****************************************/
void Drive::ManualControl(float joystickY, float joystickX)
{
   float leftThrottle = joystickY + joystickX;
   float rightThrottle = joystickY - joystickX;
   leftFront.TorqueCommand(leftThrottle);
   leftBack.TorqueCommand(leftThrottle);
   rightFront.TorqueCommand(rightThrottle);
   rightBack.TorqueCommand(rightThrottle);
}
