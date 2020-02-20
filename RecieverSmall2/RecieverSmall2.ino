/***************************************************************
* TRANSMITTER
* DESC: The transmitter code takes data from a joystick and buttons
* and sends a heartbeat message to the reciever.
* Author: Jonathan L Clark
* Date: 12/22/2019
**************************************************************/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(10, 9); // CE, CSN
long msTicks = 0;
long timeoutTime = 0;
long debounceTime = 0;
unsigned long nextReadTime = 0;
unsigned long pulseDownTime = 0;
unsigned long delta = 0;
unsigned long startMicros = 0;
boolean phase = false;
boolean pulseDone = false;

const byte address[6] = "39421";
boolean lights_on = false;
boolean last_state = false;
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define TURN_A 3
#define TURN_B 5

#define MAIN_A 6
#define MAIN_B 7

#define WHITE_HEADLIGHTS A3

#define SENSOR_TRIGGER A0
#define SENSOR_ECHO 2

void setup() 
{
   Serial.begin(9600);
   
   pinMode(WHITE_HEADLIGHTS, OUTPUT);

   pinMode(TURN_A, OUTPUT);
   pinMode(TURN_B, OUTPUT);

   pinMode(MAIN_A, OUTPUT);
   pinMode(MAIN_B, OUTPUT);

   //digitalWrite(TURN_A, LOW);
   //digitalWrite(TURN_B, HIGH);

   pinMode(SENSOR_TRIGGER, OUTPUT);
   attachInterrupt(digitalPinToInterrupt(SENSOR_ECHO), echoIsr, CHANGE);
   
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
      Serial.println("test");
      if (data[4] == 0 && data[5] > 150)
      {
         // main motor drive
         digitalWrite(MAIN_A, HIGH);
         digitalWrite(MAIN_B, LOW);
      }
      else if (data[4] == 1 && data[5] > 150)
      {
         // main motor drive
         digitalWrite(MAIN_A, LOW);
         digitalWrite(MAIN_B, HIGH);
      }
      else if (data[2] == 0 && data[3] > 150)
      {
          // Right Turrn
          digitalWrite(TURN_A, LOW);
          digitalWrite(TURN_B, HIGH);
      }
      else if (data[2] == 1 && data[3] > 150)
      {
         // Left Turn
         digitalWrite(TURN_A, HIGH);
         digitalWrite(TURN_B, LOW);
      }
      else if (data[5] <= 150 && data[3] < 150)
      {
         digitalWrite(TURN_A, LOW);
         digitalWrite(TURN_B, LOW);

         digitalWrite(MAIN_A, LOW);
         digitalWrite(MAIN_B, LOW);
      }
      if ((data[6] & 0x10) == 0x10 && debounceTime < msTicks)
      {
          if (lights_on)
          {
              digitalWrite(WHITE_HEADLIGHTS, LOW);
              lights_on = false;
          }
          else
          {
              digitalWrite(WHITE_HEADLIGHTS, HIGH);
              lights_on = true;
          }
          debounceTime = msTicks + 500;
      }
      timeoutTime = msTicks + 500;
      delay(5);
   }
   if (timeoutTime < msTicks)
   {
      digitalWrite(TURN_A, LOW);
      digitalWrite(TURN_B, LOW);
      digitalWrite(MAIN_A, LOW);
      digitalWrite(MAIN_B, LOW);
      digitalWrite(WHITE_HEADLIGHTS, LOW);
   }
}

void echoIsr() 
{
   if (phase)
   {
      delta = micros() - startMicros;
      phase = false;
      pulseDone = true;
   }
   else
   {
      startMicros = micros();
      phase = true;
   }
}
