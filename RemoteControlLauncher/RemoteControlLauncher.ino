/***************************************************************
* REMOTE CONTROL LAUNCHER
* DESC: The remote control launcher to remotley launch rockets without
* Author: Jonathan L Clark
* Date: 6/29/2022
**************************************************************/
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

// System Variables
long msTicks = 0;
long debounceTime = 0;
unsigned long greenLedOffTime = 0;
unsigned long launchStopTime = 0;

// Power Pulse Variables
boolean armed = false;
boolean launchSeq = false;

const byte address[6] = "39428";
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define RADIO_CE 8
#define RADIO_CSN 7

#define GREEN_LED 6
#define BLUE_LED 5
#define RELAY_PIN 10


RF24 radio(RADIO_CE, RADIO_CSN); // CE, CSN

void setup() 
{
   Serial.begin(9600);
   pinMode(A3, INPUT);
   pinMode(GREEN_LED, OUTPUT);
   pinMode(BLUE_LED, OUTPUT);
   pinMode(RELAY_PIN, OUTPUT);
   digitalWrite(GREEN_LED, LOW);
   digitalWrite(BLUE_LED, LOW);
   digitalWrite(RELAY_PIN, LOW);
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
      digitalWrite(BLUE_LED, HIGH);
      radio.read(&data, sizeof(data));    //Reading the data
      if (data[0] != 0xC1 || data[1] != 0xE2)
      {
         //Serial.println("Error packet");
      }
      else
      {
         if (data[6] & 0x08)
         {
            armed = true;
         }
         else if (data[6] & 0x02)
         {
            armed = false;
         }

         if (data[6] & 0x10)
         {
            if (armed && !launchSeq)
            {
               launchSeq = true;
               digitalWrite(RELAY_PIN, HIGH);
               launchStopTime = msTicks + 2000;
            }
         }
         if ((data[6] & 0x10) && debounceTime < msTicks)
         {
             debounceTime = msTicks + 500;
         }
      }
      delay(5);
   }
   if (digitalRead(BLUE_LED) != LOW && greenLedOffTime < msTicks)
   {
       digitalWrite(BLUE_LED, LOW);
   }
   if (armed && digitalRead(GREEN_LED) == LOW)
   {
      digitalWrite(GREEN_LED, HIGH);
   }
   if (launchSeq && launchStopTime < msTicks)
   {
      launchSeq = false;
      armed = false;
      digitalWrite(RELAY_PIN, LOW);
   }
   else if (!armed && digitalRead(GREEN_LED) == HIGH)
   {
      digitalWrite(GREEN_LED, LOW);
   }
}
