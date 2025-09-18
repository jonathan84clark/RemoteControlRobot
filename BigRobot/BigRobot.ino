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

#define CE 7
#define CSN 10

//#define BLUE_LED 21

#define BACK_RIGHT_A 3
#define BACK_RIGHT_B A7

#define BACK_LEFT_A 5
#define BACK_LEFT_B A6

#define FRONT_LEFT_A 6
#define FRONT_LEFT_B A3

#define FRONT_RIGHT_A 9
#define FRONT_RIGHT_B A2

// System Variables
long msTicks = 0;
long timeoutTime = 0;
long debounceTime = 0;
unsigned long nextReadTime = 0;
unsigned long pulseDownTime = 0;
unsigned long delta = 0;
unsigned long startMicros = 0;
unsigned long ledOffTime = 0;

const byte address[6] = "99885";
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned long packet_index = 0;

//RF24 radio(CE, CSN); // CE, CSN
//Drive drive(FRONT_LEFT_A, FRONT_LEFT_B, BACK_LEFT_A, BACK_LEFT_B, FRONT_RIGHT_A, FRONT_RIGHT_B, BACK_RIGHT_A, BACK_RIGHT_B);

/***********************************************************
* SET HEADLIGHTS
***********************************************************/
void SystemsOff()
{
   //drive.ManualControl(0.0, 0.0);
}

void setup() 
{
   Serial.begin(9600);
   //pinMode(BLUE_LED, OUTPUT);
   pinMode(BACK_RIGHT_A, OUTPUT);
   pinMode(BACK_RIGHT_B, OUTPUT);
   pinMode(BACK_LEFT_A, OUTPUT);
   pinMode(BACK_LEFT_B, OUTPUT);
   pinMode(FRONT_LEFT_A, OUTPUT);
   pinMode(FRONT_LEFT_B, OUTPUT);
   pinMode(FRONT_RIGHT_A, OUTPUT);
   pinMode(FRONT_RIGHT_B, OUTPUT);

   digitalWrite(BACK_RIGHT_A, HIGH);
   digitalWrite(BACK_RIGHT_B, HIGH);
   digitalWrite(BACK_LEFT_A, HIGH);
   digitalWrite(BACK_LEFT_B, HIGH);
   digitalWrite(FRONT_LEFT_A, HIGH);
   digitalWrite(FRONT_LEFT_B, HIGH);
   digitalWrite(FRONT_RIGHT_A, HIGH);
   digitalWrite(FRONT_RIGHT_B, HIGH);
   //radio.begin();
   //radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
   //radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
   //radio.startListening();              //This sets the module as receiver

   //digitalWrite(A7, LOW);
   //digitalWrite(3, HIGH);
   //analogWrite(A6, LOW);
   //drive.ManualControl(1.0, 0.0);
}

void loop()
{
  /*
   msTicks = millis();
   if (radio.available())              //Looking for the data.
   {
      Serial.println("Got Data");
      Serial.println(packet_index++);
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
      //drive.ManualControl(throttle, yaw);
      timeoutTime = msTicks + 5000;
      //delay(5);
   }
   //if (digitalRead(BLUE_LED) == HIGH && ledOffTime < msTicks)
   //{
   //    //digitalWrite(BLUE_LED, LOW);
  // }
   if (timeoutTime < msTicks)
   {
       SystemsOff();
   }
   */
}
