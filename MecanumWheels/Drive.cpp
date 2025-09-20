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
Drive::Drive(int leftFrontA, int leftFrontB, int leftRearA, int leftRearB, int rightFrontA, int rightFrontB, int rightRearA, int rightRearB)
{
    leftFront.Setup(leftFrontA, leftFrontB, 1.0, 0.0);
    leftRear.Setup(leftRearA, leftRearB, 1.0, 0.0);
    rightFront.Setup(rightFrontA, rightFrontB, 1.0, 0.0);
    rightRear.Setup(rightRearA, rightRearB, 1.0, 0.0);
}

/****************************************
* DRIVE MANUAL CONTROL
* DESC: Takes a joystick y and x value and
* uses that to manually control the vehicle.
****************************************/
void Drive::ManualControl(float joystickY, float joystickX, bool controller_lock)
{
   if (controller_lock)
   {
       // TODO: Now we only can slide in lock mode, but later we want to apply forward vectors
       float forward_throttle = joystickX;
       float rear_throttle = -joystickX;
       leftFront.TorqueCommand(forward_throttle);
       rightFront.TorqueCommand(-forward_throttle);
       leftRear.TorqueCommand(rear_throttle);
       rightRear.TorqueCommand(-rear_throttle);
   }
   else
   {
      float leftThrottle = joystickY + joystickX;
      float rightThrottle = joystickY - joystickX;
      leftFront.TorqueCommand(leftThrottle);
      leftRear.TorqueCommand(leftThrottle);
      rightFront.TorqueCommand(rightThrottle);
      rightRear.TorqueCommand(rightThrottle);
   }
}
