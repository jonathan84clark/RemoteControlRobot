/*******************************************************
* AUTO CONTROL
* DESC: The auto-control class is designed to provide an automomous interface
* to the robot based on basic sensor input
* Author: Jonathan L Clark
* Date: 7/28/2021
*******************************************************/
#include "Drive.h"
#include "AutoControl.h"
#include "PID.h"
#include <Arduino.h>

#define SYSTEM_VOLTAGE 5.0
#define MAX_ADC_VALUE 1023.0

#define AUTO_THROTTLE 0.20

/****************************************
* AUTO CONTROL DEFAULT CONSTRUCTOR
****************************************/
AutoControl::AutoControl()
{
   sampleIndex = 0;
   enabled = 0;
   throttle = AUTO_THROTTLE;
   yaw = 0.0;
   drivePid.Setup(0.001, 0.0, 0.0);
   drivePid.SetSetPoint(400.0);
   steeringPid.Setup(0.005, 0.0, 0.0);
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
   //Serial.println(leftReading);
}

/****************************************
* MANAGE AVOIDANCE STEERING
* DESC: Handles the see-and-avoid steering part of the algorithm
****************************************/
float AutoControl::ManageAvoidanceSteering()
{
    float yaw = 0.0;
    bool leftYawActive = false;
    bool rightYawActive = false;
    unsigned long currentTime = millis();
    
    // Determine if the left reading is active
    if (leftReading > 150)
    {
       leftYawActive = true;
    }
    else if (leftReading < 100)
    {
       leftYawActive = false;
    }

    // Determine if the right is active
    if (rightReading > 150)
    {
       rightYawActive = true;
    }
    else if (rightReading < 100)
    {
       rightYawActive = false;
    }

    // Handle the auto controls
    if (rightYawActive && leftYawActive)
    {
        steeringPid.SetSetPoint(0);
        float delta = leftReading - rightReading;
        yaw = steeringPid.Update(delta, currentTime);
        yaw *= -1.0;
        drive->ManualControl(0.0, yaw);
        
    }
    else if (rightYawActive)
    {
        steeringPid.SetSetPoint(200);
        yaw = steeringPid.Update(rightReading, currentTime);
    }
    else if (leftYawActive)
    {
        steeringPid.SetSetPoint(200);
        yaw = steeringPid.Update(leftReading, currentTime);
        yaw = yaw * -1.0;
    }
    else
    {
        yaw = 0.0;
        steeringPid.Clear();
    }
    return yaw;
  
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
    
    //Serial.println(midReading);
    // First we backup with the wheels rotated
    if (midReading > 150 && uturnStage == 0 && nextUturnTime < currentTime)
    {
        Serial.println("Start uturn");
        uturnStage = 1;
        throttle = -0.4;
        if (leftReading < rightReading)
        {
           yaw = 1.0;
        }
        else
        {
           yaw = -1.0;
        }
        nextUturnTime = currentTime + 1500; // Start with 4 seconds
    }
    else if (uturnStage == 1 && nextUturnTime < currentTime)
    {
        throttle = AUTO_THROTTLE;
        yaw = 0.0;
        uturnStage = 0;
        nextUturnTime = currentTime + 1000;
        //Serial.println("Uturn finished");
    }

    if (uturnStage == 0)
    {
        yaw = ManageAvoidanceSteering();
    }
    drive->ManualControl(throttle, yaw);
    
    //float yaw = ManageAvoidanceSteering();
    //drive->ManualControl(output, yaw);

    //Serial.println(output);
    
    
}
