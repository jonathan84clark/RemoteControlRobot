/***************************************************************
* TRIPLE SENSORS
* DESC: The triple sensors is the basis for all new robot designs
* going forward.
* Author: Jonathan L Clark
* Date: 12/22/2019
**************************************************************/
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "Drive.h"
#include "Config.h"

// System Variables
long msTicks = 0;
long timeoutTime = 0;
long debounceTime = 0;
unsigned long nextReadTime = 0;
unsigned long pulseDownTime = 0;
unsigned long delta = 0;
unsigned long startMicros = 0;
long nextDebugTime = 0;

// Power Pulse Variables
boolean phase = false;
boolean pulseDone = false;
float pulseTime = 0.0;
boolean powerIsPulse = false;
boolean lights_on = false;
boolean last_state = false;

//#define SOFT_SPI_MISO_PIN 14
//#define SOFT_SPI_MOSI_PIN 16
//#define SOFT_SPI_SCK_PIN 15

// Radio variables
#if ROBOT_CONFIG == ROBOT_DIFF_CAR
//const byte address[6] = "39428";
const byte address[6] = "39421";
#endif
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

RF24 radio(8, 10); // CE, CSN
// Setup the drive system
#ifdef STEER_A
Drive drive(STEER_A, STEER_B, THROTTLE_SCALE, 0.0, MAIN_A, MAIN_B, THROTTLE_SCALE, 0.0, MODE_PIVOT_STEER);
#else
Drive drive(LEFT_A, LEFT_B, THROTTLE_SCALE, 0.0, RIGHT_A, RIGHT_B, THROTTLE_SCALE, 0.0, MODE_DIFF_STEER);
#endif


/***********************************************************
* SET HEADLIGHTS
***********************************************************/
void SystemsOff()
{
   drive.ManualControl(0.0, 0.0);
}

void setup() 
{
   Serial.begin(115200);
   pinMode(PULSE_PIN, OUTPUT);
   digitalWrite(PULSE_PIN, HIGH);
   
   radio.begin();
   radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
   radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
   radio.startListening();              //This sets the module as receiver
}

void loop()
{
   msTicks = millis();
   if (radio.available())              //Looking for the data.
   {
      radio.read(&data, sizeof(data));    //Reading the data
      float throttle = ((float)data[5] / 255.0);
      float yaw = ((float)data[3] / 255.0);
      yaw = (data[2] == 1) ? yaw * -1.0 : yaw;
      throttle = (data[4] == 1) ? throttle * -1.0 : throttle;
      // Handles turn buttons
      if (data[6] & 0x20)
      {
         yaw = -1.0;
      }
      else if (data[6] & 0x04)
      {
         yaw = 1.0;
      }
      if (data[6] & 0x08)
      {
         throttle = 1.0;
      }
      else if (data[6] & 0x02)
      {
         throttle = -1.0;
      }
      Serial.println("Got data!");
      //drive.ManualControl(throttle, yaw);
      timeoutTime = msTicks + 500;
      delay(5);
   }
   if (nextDebugTime < msTicks)
   {
      DebugPrint();
      nextDebugTime = msTicks + 1000;
      Serial.println("Main loop");
   }
   if (nextReadTime < msTicks || pulseDone)
   {
      //ReadSensors();
   }
   if (timeoutTime < msTicks)
   {
       SystemsOff();
   }
   if (pulseTime < msTicks)
   {
       if (powerIsPulse)
       {
          powerIsPulse = false;
          digitalWrite(PULSE_PIN, HIGH);
          pulseTime = msTicks + 5000;
       }
       else
       {
          powerIsPulse = true;
          digitalWrite(PULSE_PIN, LOW);
          pulseTime = msTicks + 200;
       }
   }
}

/****************************************
* DEBUG PRINT
* DESC: Prints debug statements about the state
* of the system.
****************************************/
void DebugPrint()
{
  
}
