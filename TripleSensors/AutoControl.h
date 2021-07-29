/*******************************************************
* AUTO CONTROL
* DESC: The auto-control class is designed to provide an automomous interface
* to the robot based on basic sensor input
* Author: Jonathan L Clark
* Date: 7/28/2021
*******************************************************/
#ifndef _AUTO_CONTROL_
#define _AUTO_CONTROL
#include "Motor.h"
#include "Drive.h"
#include "PID.h"

#define SENSOR_SAMPLES 5

class AutoControl
{
   public:
      AutoControl();
      void UpdateSensors();
      void UpdateSystem();
      bool GetEnabled() { return enabled;}
      void Setup(Drive* inDrive);
      void SetEnabled(int inEnabled) { enabled = inEnabled;}
      
   private:
      float ComputeAverage(unsigned int* data, unsigned int arraySize);
      unsigned int leftSamples[SENSOR_SAMPLES];
      unsigned int rightSamples[SENSOR_SAMPLES];
      unsigned int midSamples[SENSOR_SAMPLES];
      int sampleIndex = 0;
      float leftReading = 0;
      float rightReading = 0;
      float midReading = 0;
      int enabled;
      PID drivePid;
      PID steeringPid;
      Drive* drive;
      
};

#endif /* DRIVE */
