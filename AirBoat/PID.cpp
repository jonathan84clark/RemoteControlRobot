/*******************************************************
* PID
* DESC: PID loop library originally by Ivmech Mechatronics
* ported to Arduino by Jonathan L Clark
* Author: Jonathan L Clark
* Date: 7/28/2021
*******************************************************/
#include <Arduino.h>
#include "PID.h"

/****************************************
* PID DEFAULT CONSTRUCTOR
****************************************/
PID::PID()
{
   Kp = 0;
   Ki = 0;
   Kd = 0;

   sample_time = 0;
   current_time = 0;
   last_time = 0;

   Clear();
}

/****************************************
* PID CONSTRUCTOR
****************************************/
PID::PID(float inP, float inI, float inD)
{
   Kp = inP;
   Ki = inI;
   Kd = inD;

   sample_time = 0;
   current_time = 0;
   last_time = current_time;

   Clear();
}

/****************************************
* SETUP
* DESC: Setup the PID object
****************************************/
void PID::Setup(float inP, float inI, float inD)
{
   Kp = inP;
   Ki = inI;
   Kd = inD;

   sample_time = 0;
   current_time = 0;
   last_time = current_time;

   Clear();    
}

/****************************************
* CLEAR
****************************************/
void PID::Clear()
{
   set_point = 0.0;
   pTerm = 0;
   iTerm = 0;
   dTerm = 0;
   lastError = 0;

   int_error = 0;
   windup_gaurd = 20.0;
   output = 0.0;
}

/****************************************
* PID UPDATE
****************************************/
float PID::Update(float feedback_value, unsigned long in_current_time)
{
    float error = set_point - feedback_value;
    current_time = in_current_time;
    unsigned long deltaTimeMs = current_time - last_time;
    float delta_time = (float)deltaTimeMs;
    float delta_error = error - lastError;

    if (delta_time >= sample_time)
    {
       pTerm = Kp * error;
       iTerm += error * delta_time;
       if (iTerm < (windup_gaurd * -1.0))
       {
          iTerm = windup_gaurd * -1.0;
       }
       else if (iTerm > windup_gaurd)
       {
          iTerm = windup_gaurd;
       }
       dTerm = 0.0;
       if (delta_time > 0.0)
       {
           dTerm = delta_error / delta_time;
       }

       last_time = current_time;
       lastError = error;

       output = pTerm + (Ki * iTerm) + (Kd * dTerm);
    }
    return output;
}
