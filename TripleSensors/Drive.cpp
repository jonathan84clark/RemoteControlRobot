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
Drive::Drive(int m1APin, int m1BPin, float m1Scale, float m1Cutoff, int m2APin, int m2BPin, float m2Scale, float m2Cutoff, int inSteering)
{
    steering = inSteering;
    motor1.Setup(m1APin, m1BPin, m1Scale, m1Cutoff);
    motor2.Setup(m2APin, m2BPin, m2Scale, m2Cutoff);
}

/****************************************
* DRIVE MANUAL CONTROL
* DESC: Takes a joystick y and x value and
* uses that to manually control the vehicle.
****************************************/
void Drive::ManualControl(float joystickY, float joystickX)
{
   if (steering == MODE_DIFF_STEER)
   {
       float leftThrottle = joystickY + joystickX;
       float rightThrottle = joystickY - joystickX;
       motor1.TorqueCommand(leftThrottle);
       motor2.TorqueCommand(rightThrottle);
   }
   else if (steering == MODE_PIVOT_STEER)
   {
       // TODO
   }
   else
   {
      // Invalid steering mode
   }
}
