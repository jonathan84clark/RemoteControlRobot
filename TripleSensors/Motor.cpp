/*******************************************************
* BLASTER SUB CLASS
* DESC: The blaster sub class is designed to behave like a blaster
* shot.
* Author: Jonathan L Clark
* Date: 10/21/2019
*******************************************************/

#include "Motor.h"
#include <Arduino.h>

Motor::Motor(int inAControl, int inBControl)
{
   a_control = inAControl;
   b_control = inBControl;
   pinMode(a_control, OUTPUT);
   pinMode(b_control, OUTPUT);
}

void Motor::TorqueCommand(float throttle)
{
    if (throttle < 0.0)
    {
       int analogValue = (int)(throttle * -1.0f) * 255.0;
       analogWrite(b_control, analogValue);
       digitalWrite(a_control, LOW);
    }
    else
    {
       int analogValue = (int)(throttle * 255.0);
       analogWrite(a_control, analogValue);
       digitalWrite(b_control, LOW);
    }
}
