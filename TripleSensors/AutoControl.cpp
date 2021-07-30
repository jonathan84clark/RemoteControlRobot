/*******************************************************
* AUTO CONTROL
* DESC: The auto-control class is designed to provide an automomous interface
* to the robot based on basic sensor input
* Author: Jonathan L Clark
* Date: 7/28/2021
*******************************************************/
#include "Drive.h"
#include "AutoControl.h"
#include "Config.h"
#include "PID.h"
#include <Arduino.h>

#define SYSTEM_VOLTAGE 5.0
#define MAX_ADC_VALUE 1023.0

/****************************************
* AUTO CONTROL DEFAULT CONSTRUCTOR
****************************************/
AutoControl::AutoControl()
{
   sampleIndex = 0;
   enabled = 0;
   drivePid.Setup(0.001, 0.0, 0.0);
   drivePid.SetSetPoint(400.0);
}

/****************************************
* SETUP
* DESC: Sets up the auto-control system
****************************************/
void AutoControl::Setup(Drive* inDrive)
{
   drive = inDrive;
}

/****************************************
* COMPUTE AVERAGE
****************************************/
float AutoControl::ComputeAverage(unsigned int* data, unsigned int arraySize)
{
   float output = 0;
   for (int i = 0; i < arraySize; i++)
   {
       output += (float)data[i];
   }
   output = output / (float)arraySize;

   return output;
}

/****************************************
* UPDATE SENSORS
* DESC: Updates the sensor readings
****************************************/
void AutoControl::UpdateSensors()
{
   unsigned int ir_left = 0;
   unsigned int ir_mid = 0;
   unsigned int ir_right = 0;
   
#ifdef IR_LEFT
   ir_left = analogRead(IR_LEFT);
#endif
#ifdef IR_MID
   ir_mid = analogRead(IR_MID);
#endif
#ifdef IR_RIGHT
   ir_right = analogRead(IR_RIGHT);
#endif
   leftSamples[sampleIndex] = ir_left;
   rightSamples[sampleIndex] = ir_right;
   midSamples[sampleIndex] = ir_mid;

   leftReading = ComputeAverage(leftSamples, SENSOR_SAMPLES);
   rightReading = ComputeAverage(rightSamples, SENSOR_SAMPLES);
   midReading = ComputeAverage(midSamples, SENSOR_SAMPLES);
   sampleIndex++;

   if (sampleIndex == SENSOR_SAMPLES)
   {
      sampleIndex = 0;
   }
   //Serial.println(rightReading);
}

/****************************************
* UPDATE SYSTEM
* DESC: Updates the auto-control system
****************************************/
void AutoControl::UpdateSystem()
{
    if (!enabled)
    {
       return; // Do, nothing we aren't functional right now
    }
    unsigned long currentTime = millis();
    float output = drivePid.Update((float)midReading, currentTime);
    if (output > 0.8)
    {
       output = 0.8;
    }
    if (output < -0.8)
    {
      output = -0.8;
    }
    drive->ManualControl(output, 0.0);

    //Serial.println(output);
    
    
}
