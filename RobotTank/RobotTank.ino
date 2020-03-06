/***************************************************************
* ROBOT TANK
* DESC: The robot tank is a small robot designed to move using tracks.
* Author: Jonathan L Clark
* Date: 12/22/2019
**************************************************************/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN
long msTicks = 0;
long timeoutTime = 0;
long debounceTime = 0;

const byte address[6] = "39421";
boolean lights_on = false;
boolean last_state = false;
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define LEFT_A 8
#define LEFT_B 6

#define RIGHT_A 3
#define RIGHT_B 5

#define WHITE_HEADLIGHTS A1
#define SECOND_HEADLIGHTS A2
#define BUZZER 4

void setup() 
{
   Serial.begin(9600);
   pinMode(WHITE_HEADLIGHTS, OUTPUT);
   pinMode(SECOND_HEADLIGHTS, OUTPUT);

   pinMode(LEFT_A, OUTPUT);
   pinMode(LEFT_B, OUTPUT);

   pinMode(RIGHT_A, OUTPUT);
   pinMode(RIGHT_B, OUTPUT);

   pinMode(BUZZER, OUTPUT);
   
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
      Serial.println(data[5]);
      if (data[4] == 0 && data[5] > 150)
      {
          // Left Forward
          digitalWrite(LEFT_A, HIGH);
          digitalWrite(LEFT_B, LOW);

          // Right forward
          digitalWrite(RIGHT_A, HIGH);
          digitalWrite(RIGHT_B, LOW);
      }
      else if (data[4] == 1 && data[5] > 150)
      {
          // Left Forward
         digitalWrite(LEFT_A, LOW);
         digitalWrite(LEFT_B, HIGH);

         // Right forward
         digitalWrite(RIGHT_A, LOW);
         digitalWrite(RIGHT_B, HIGH);
      }
      else if (data[2] == 0 && data[3] > 150)
      {
          // Left Forward
         digitalWrite(LEFT_A, HIGH);
         digitalWrite(LEFT_B, LOW);

         // Right forward
         digitalWrite(RIGHT_A, LOW);
         digitalWrite(RIGHT_B, HIGH);
      }
      else if (data[2] == 1 && data[3] > 150)
      {
          // Left Forward
         digitalWrite(LEFT_A, LOW);
         digitalWrite(LEFT_B, HIGH);

         // Right forward
         digitalWrite(RIGHT_A, HIGH);
         digitalWrite(RIGHT_B, LOW);
      }
      else if (data[5] <= 150 && data[3] < 150)
      {
         // Left Forward
         digitalWrite(LEFT_A, LOW);
         digitalWrite(LEFT_B, LOW);

         // Right forward
         digitalWrite(RIGHT_A, LOW);
         digitalWrite(RIGHT_B, LOW);
      }
      if ((data[6] & 0x10) == 0x10 && debounceTime < msTicks)
      {
          if (lights_on)
          {
              digitalWrite(WHITE_HEADLIGHTS, LOW);
              digitalWrite(SECOND_HEADLIGHTS, LOW);
              lights_on = false;
          }
          else
          {
              digitalWrite(WHITE_HEADLIGHTS, HIGH);
              digitalWrite(SECOND_HEADLIGHTS, HIGH);
              lights_on = true;
          }
          debounceTime = msTicks + 500;
      }
      if ((data[6] & 0x01) == 0x01)
      {
          digitalWrite(BUZZER, HIGH);
      }
      else
      {
          digitalWrite(BUZZER, LOW);
      }
      timeoutTime = msTicks + 500;
      delay(5);
   }
   if (timeoutTime < msTicks)
   {
      digitalWrite(RIGHT_A, LOW);
      digitalWrite(RIGHT_B, LOW);
      digitalWrite(LEFT_A, LOW);
      digitalWrite(LEFT_B, LOW);
      digitalWrite(WHITE_HEADLIGHTS, LOW);
      digitalWrite(SECOND_HEADLIGHTS, LOW);
      digitalWrite(BUZZER, LOW);
   }

    
}
