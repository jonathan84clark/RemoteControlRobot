/*******************************************************
* MOTOR CLASS
* DESC: The motor class manages the interface with an H-bridge
* allowing a motor to spin forward or backward
* Author: Jonathan L Clark
* Date: 10/17/2020
*******************************************************/
#ifndef _MOTOR_
#define _MOTOR_

class Motor
{
   public:
      // Public Functions
      Motor();
      Motor(int inAControl, int inBControl, float inScale, float inCutoff);
      void Setup(int inAControl, int inBControl, float inScale, float inCutoff);
      void TorqueCommand(float input);
      
   private:
      int a_control;
      int b_control;
      float scale;
      float cutOff;
      bool motorReady;
      int NormalizeThrottle(int input);
      
      
};

#endif /* MOTOR */
