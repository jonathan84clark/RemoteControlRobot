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
#include "AutoControl.h"

// System Variables
long msTicks = 0;
long timeoutTime = 0;
long debounceTime = 0;
unsigned long nextReadTime = 0;
unsigned long pulseDownTime = 0;
unsigned long delta = 0;
unsigned long startMicros = 0;
unsigned long nextLightPulse = 0;
unsigned long nextAutoControl = 0;
unsigned long greenLedOffTime = 0;
unsigned long yellowOffTime = 0;
unsigned long buttonDebounce = 0;

// Power Pulse Variables
boolean phase = false;
boolean pulseDone = false;
float pulseTime = 0.0;
boolean powerIsPulse = false;
boolean lights_on = false;
boolean last_state = false;
boolean gettingData = false;
boolean dbgLedOn = false;

const byte address[6] = "39421";
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define RADIO_CE 8
#define RADIO_CSN 10
#define LEFT_A 9
#define LEFT_B 6

#define RIGHT_A 5
#define RIGHT_B 3
#define MODE_DIFF_STEER 1
#define THROTTLE_SCALE 1.0

#define GREEN_LED 7
#define YELLOW_LED 4
#define RED_LED 2

#define PULSE_PIN 6

#define UP_BTN    A5
#define DN_BTN    A7
#define LEFT_BTN  A4
#define RIGHT_BTN A6
#define GO_BTN    A3


RF24 radio(RADIO_CE, RADIO_CSN); // CE, CSN
// Setup the drive system
Drive drive(LEFT_A, LEFT_B, THROTTLE_SCALE, 0.0, RIGHT_A, RIGHT_B, THROTTLE_SCALE, 0.0, MODE_DIFF_STEER);

AutoControl autoControl;


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
   pinMode(PULSE_PIN, OUTPUT);
   pinMode(GREEN_LED, OUTPUT);
   pinMode(RED_LED, OUTPUT);
   pinMode(YELLOW_LED, OUTPUT);

   pinMode(UP_BTN, INPUT);
   pinMode(DN_BTN, INPUT);
   pinMode(LEFT_BTN, INPUT);
   pinMode(RIGHT_BTN, INPUT);
   pinMode(GO_BTN, INPUT);
   
   digitalWrite(PULSE_PIN, HIGH);

   radio.begin();
   radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
   radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
   radio.startListening();              //This sets the module as receiver

   //autoControl.SetEnabled(1);
   autoControl.Setup(&drive);
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
         if (!autoControl.GetEnabled())
         {
            drive.ManualControl(throttle, yaw);
         }
         if ((data[6] & 0x10) && debounceTime < msTicks)
         {

             debounceTime = msTicks + 500;
         }
         gettingData = true;
         timeoutTime = msTicks + 500;
      }
      delay(5);
   }
   if (digitalRead(GREEN_LED) == LOW && greenLedOffTime < msTicks)
   {
       digitalWrite(GREEN_LED, LOW);
   }
   if (nextReadTime < msTicks)
   {
      autoControl.UpdateSensors();
      nextReadTime = msTicks + 20;
   }
   if (nextAutoControl < msTicks)
   {
      autoControl.UpdateSystem();
      nextAutoControl = msTicks + 100;
   }
   if (timeoutTime < msTicks)
   {
       SystemsOff();
   }
   if (buttonDebounce < msTicks)
   {
      if (digitalRead(YELLOW_LED))
      {
         digitalWrite(YELLOW_LED, LOW);
      }
      if (digitalRead(UP_BTN))
      {
         //digitalWrite(YELLOW_LED, HIGH); // Stuck on
         buttonDebounce = msTicks + 200;
      }
      else if (digitalRead(DN_BTN))
      {
         //digitalWrite(YELLOW_LED, HIGH); // Not working
         buttonDebounce = msTicks + 200;
      }
      else if (digitalRead(LEFT_BTN))
      {
         digitalWrite(YELLOW_LED, HIGH);
         buttonDebounce = msTicks + 200;
      }
      else if (digitalRead(RIGHT_BTN))
      {
         //digitalWrite(YELLOW_LED, HIGH);
         buttonDebounce = msTicks + 200;
      }
      else if (digitalRead(GO_BTN))
      {
         //digitalWrite(YELLOW_LED, HIGH);
         buttonDebounce = msTicks + 200;
      }

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
