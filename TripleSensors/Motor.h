/*******************************************************
* BLASTER SUB CLASS
* DESC: The blaster sub class is designed to behave like a blaster
* shot.
* Author: Jonathan L Clark
* Date: 10/21/2019
*******************************************************/
#ifndef _MOTOR_
#define _MOTOR_

class Motor
{
   public:
      // Public Functions
      Motor();
      Motor(int inAControl, int inBControl);
      void TorqueCommand(float input);
      
   private:
      int a_control;
      int b_control;
      
};

#endif /* BLASTER */
