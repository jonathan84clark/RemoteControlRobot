/*******************************************************
* PID
* DESC: PID loop library originally by Ivmech Mechatronics
* ported to Arduino by Jonathan L Clark
* Author: Jonathan L Clark
* Date: 7/28/2021
*******************************************************/
#ifndef _PID_LOOP_
#define _PID_LOOP_

class PID
{
   public:
      PID(float inP, float inI, float inD);
      PID();
      void Clear();
      float Update(float feedback_value, unsigned long in_current_time);
      void Setup(float inP, float inI, float inD);
      void SetSetPoint(float inSetPoint) { set_point = inSetPoint;}

      
   private:
      float set_point;
      
      float Kp;
      float Ki;
      float Kd;

      float pTerm;
      float iTerm;
      float dTerm;
      float lastError;
      
      unsigned long sample_time;
      unsigned long current_time;
      unsigned long last_time;

      float int_error;
      float windup_gaurd;
      float output;
      
};

#endif /* DRIVE */
