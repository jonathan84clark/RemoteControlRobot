/*******************************************************
* DRIVE CLASS
* DESC: The drive class provides a high-level interface to 
* a motor drive system. This allows the user to turn, go forward,
* reverse and perform other manuevers. 
* Author: Jonathan L Clark
* Date: 10/17/2020
*******************************************************/
#ifndef _DRIVE_
#define _DRIVE_

#include "Motor.h"

#define MODE_DIFF_STEER  1
#define MODE_PIVOT_STEER 2

class Drive
{
   public:
      Drive();
      Drive(int leftFrontA, int leftFrontB, int leftRearA, int leftRearB, int rightFrontA, int rightFrontB, int rightRearA, int rightRearB);
      void ManualControl(float joystickY, float joystickX, bool controller_lock);
      
   private:
      Motor leftFront;
      Motor leftRear;
      Motor rightFront;
      Motor rightRear;
      
};

#endif /* DRIVE */
