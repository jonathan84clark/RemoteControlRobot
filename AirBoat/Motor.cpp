/*******************************************************
* MOTOR CLASS
* DESC: The motor class manages the interface with an H-bridge
* allowing a motor to spin forward or backward
* Author: Jonathan L Clark
* Date: 10/17/2020
*******************************************************/

#include "Motor.h"
#include <Arduino.h>

/****************************************
* MOTOR DEFAULT CONSTRUCTOR
****************************************/
Motor::Motor()
{
  motorReady = false;
}

/****************************************
* MOTOR CONSTRUCTOR
****************************************/
Motor::Motor(int inAControl, int inBControl, float inScale, float inCutoff)
{
   Setup(inAControl, inBControl, inScale, inCutoff);
}

/****************************************
* MOTOR SETUP
* DESC: Sets up the motor, setting the correct
* output ports.
****************************************/
void Motor::Setup(int inAControl, int inBControl, float inScale, float inCutoff)
{
   a_control = inAControl;
   b_control = inBControl;
   scale = inScale;
   cutOff = inCutoff;
   pinMode(a_control, OUTPUT);
   pinMode(b_control, OUTPUT);
   motorReady = true;
}

/****************************************
* NORMALIZE THROTTLE
* DESC: Keeps the throttle within the defined
* range defined by a byte
****************************************/
int Motor::NormalizeThrottle(int input)
{
   if (input > 255)
   {
      input = 255;
   }
   else if (input < 0)
   {
      input = 0;
   }
   return input;
}

/****************************************
* TORQUE COMMAND
* DESC: Handles applying power to the motor
* creating torque.
****************************************/
void Motor::TorqueCommand(float throttle)
{
    if (!motorReady)
    {
       return;
    }
    throttle = throttle * scale;
    if (throttle < 0.0)
    {
       Serial.println("Back up not working");
       throttle = throttle * -1.0;
       throttle = (throttle < cutOff) ? 0.0 : throttle;
       int analogValue = NormalizeThrottle((int)(throttle * 255.0));
       Serial.println(analogValue);
       analogWrite(b_control, analogValue);
       digitalWrite(a_control, LOW);
    }
    else if (throttle == 0.0)
    {
       digitalWrite(b_control, LOW);
       digitalWrite(a_control, LOW);
    }
    else
    {
       throttle = (throttle < cutOff) ? 0.0 : throttle;
       int analogValue = NormalizeThrottle((int)(throttle * 255.0));
       analogWrite(a_control, analogValue);
       digitalWrite(b_control, LOW);
    }
}
