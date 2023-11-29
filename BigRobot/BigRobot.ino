/***************************************************************
* BIG ROBOT
* DESC: This code is designed to control the large robot chasis with
* * macanum wheels
* GPIO16 -> MISO (SD)
* GPIO17 -> CSN (SD)
* GPIO18 -> SCK (SD)
* GPIO19 -> MOSI (SD)
* Author: Jonathan L Clark
* Date: 7/12/2023
**************************************************************/
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "Drive.h"

#define CE 20
#define CSN 17

#define BLUE_LED 21

#define BACK_RIGHT_A 1
#define BACK_RIGHT_B 0

#define BACK_LEFT_A 5
#define BACK_LEFT_B 4

#define FRONT_LEFT_A 6
#define FRONT_LEFT_B 7

#define FRONT_RIGHT_A 2
#define FRONT_RIGHT_B 3

// System Variables
long msTicks = 0;
long timeoutTime = 0;
long debounceTime = 0;
unsigned long nextReadTime = 0;
unsigned long pulseDownTime = 0;
unsigned long delta = 0;
unsigned long startMicros = 0;
unsigned long ledOffTime = 0;

const byte address[6] = "39421";
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

RF24 radio(CE, CSN); // CE, CSN
Drive drive(FRONT_LEFT_A, FRONT_LEFT_B, BACK_LEFT_A, BACK_LEFT_B, FRONT_RIGHT_A, FRONT_RIGHT_B, BACK_RIGHT_A, BACK_RIGHT_B);

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
   pinMode(BLUE_LED, OUTPUT);
   
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
      Serial.println(throttle);
      //digitalWrite(BLUE_LED, HIGH);
      ledOffTime = msTicks + 50;
      drive.ManualControl(throttle, yaw);
      timeoutTime = msTicks + 5000;
      delay(5);
   }
   //if (digitalRead(BLUE_LED) == HIGH && ledOffTime < msTicks)
   //{
   //    //digitalWrite(BLUE_LED, LOW);
  // }
   if (timeoutTime < msTicks)
   {
       SystemsOff();
   }
}
