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
      Drive(int leftFrontA, int leftFrontB, int leftBackA, int leftBackB, int rightFrontA, int rightFrontB, int rightBackA, int rightBackB);
      void ManualControl(float joystickY, float joystickX);
      
   private:
      int steering;
      Motor leftFront;
      Motor leftBack;
      Motor rightFront;
      Motor rightBack;
      
};

#endif /* DRIVE */
