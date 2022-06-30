/***************************************************************
* PROGRAMMABLE ROBOT
* DESC: The programmable robot allows a series of commands to be programmed into memory
* going forward.
* Author: Jonathan L Clark
* Date: 12/22/2019
**************************************************************/
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "Drive.h"

// System Variables
long msTicks = 0;
long timeoutTime = 0;
long debounceTime = 0;
unsigned long greenLedOffTime = 0;

// Power Pulse Variables
boolean gettingData = false;

const byte address[6] = "39428";
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define RADIO_CE 7
#define RADIO_CSN 10
#define LEFT_A 3
#define LEFT_B 5

#define RIGHT_A 9
#define RIGHT_B 6
#define MODE_DIFF_STEER 1
#define THROTTLE_SCALE 1.0

#define GREEN_LED 4
#define BLUE_LED A6

#define MAX_ACTIONS    256


RF24 radio(RADIO_CE, RADIO_CSN); // CE, CSN
// Setup the drive system
Drive drive(LEFT_A, LEFT_B, THROTTLE_SCALE, 0.0, RIGHT_A, RIGHT_B, THROTTLE_SCALE, 0.0, MODE_DIFF_STEER);


/***********************************************************
* SET HEADLIGHTS
***********************************************************/
void SystemsOff()
{
   drive.ManualControl(0.0, 0.0);
}

void setup() 
{
   Serial.begin(9600);
   pinMode(A3, INPUT);
   pinMode(GREEN_LED, OUTPUT);
   pinMode(BLUE_LED, OUTPUT);
   digitalWrite(GREEN_LED, LOW);
   digitalWrite(BLUE_LED, LOW);
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
      greenLedOffTime = msTicks + 500;
      digitalWrite(GREEN_LED, HIGH);
      radio.read(&data, sizeof(data));    //Reading the data
      float throttle = ((float)data[5] / 255.0);
      float yaw = ((float)data[3] / 255.0);
      yaw = (data[2] == 1) ? yaw * -1.0 : yaw;
      throttle = (data[4] == 1) ? throttle * -1.0 : throttle;
      if (data[0] != 0xC1 || data[1] != 0xE2)
      {
         //Serial.println("Error packet");
      }
      else
      {
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
         drive.ManualControl(throttle, yaw);
         if ((data[6] & 0x10) && debounceTime < msTicks)
         {
             debounceTime = msTicks + 500;
         }
         //Serial.println(yaw);
         //Serial.println("Got data! yeah!");
         gettingData = true;
         timeoutTime = msTicks + 500;
      }
      delay(5);
   }
   if (digitalRead(GREEN_LED) != LOW && greenLedOffTime < msTicks)
   {
       digitalWrite(GREEN_LED, LOW);
   }
   if (timeoutTime < msTicks)
   {
       SystemsOff();
   }
}
