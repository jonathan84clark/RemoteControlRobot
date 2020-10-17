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
#include "Motor.h"
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

#define LEFT_A 5
#define LEFT_B 3

#define RIGHT_A 6
#define RIGHT_B 7

#define WHITE_HEADLIGHTS 4

#define SENSOR_TRIGGER_L A4
#define SENSOR_TRIGGER_M A3
#define SENSOR_TRIGGER_R A2

#define SENSOR_ENABLE_L A7
#define SENSOR_ENABLE_M A6
#define SENSOR_ENABLE_R A5
#define SENSOR_ECHO 2

Motor left(LEFT_A, LEFT_B);
Motor right(RIGHT_A, RIGHT_B);

void setup() 
{
   Serial.begin(9600);
   pinMode(WHITE_HEADLIGHTS, OUTPUT);

   //pinMode(LEFT_A, OUTPUT);
   //pinMode(LEFT_B, OUTPUT);

   //pinMode(RIGHT_A, OUTPUT);
   //pinMode(RIGHT_B, OUTPUT);


   pinMode(SENSOR_TRIGGER_L, OUTPUT);
   pinMode(SENSOR_TRIGGER_M, OUTPUT);
   pinMode(SENSOR_TRIGGER_R, OUTPUT);
   pinMode(SENSOR_ENABLE_L, OUTPUT);
   pinMode(SENSOR_ENABLE_M, OUTPUT);
   pinMode(SENSOR_ENABLE_R, OUTPUT);

   digitalWrite(SENSOR_TRIGGER_L, LOW);
   digitalWrite(SENSOR_TRIGGER_M, LOW);
   digitalWrite(SENSOR_TRIGGER_R, LOW);
   digitalWrite(SENSOR_ENABLE_L, LOW);
   digitalWrite(SENSOR_ENABLE_M, LOW);
   digitalWrite(SENSOR_ENABLE_R, LOW);

   attachInterrupt(digitalPinToInterrupt(SENSOR_ECHO), echoIsr, CHANGE);
   
   radio.begin();
   radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
   radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
   radio.startListening();              //This sets the module as receiver
}

float normalize_throttle(float input)
{
   if (input > 1.0)
   {
     input = 1.0;
   }
   else if (input < -1.0)
   {
     input = -1.0;
   }
   return input;
}

void loop()
{
   msTicks = millis();
   if (radio.available())              //Looking for the data.
   {
      radio.read(&data, sizeof(data));    //Reading the data
      float throttle = ((float)data[5] / 255.0);
      float yaw = ((float)data[3] / 255.0);
      bool isTurn = false;
      Serial.print(yaw);
      if (yaw > throttle)
      {
        isTurn = true;
      }
      if (data[2] == 1)
      {
         yaw = yaw * -1.0;
      }
      if (data[4] == 1)
      {
         throttle = throttle * -1.0;
      }
      float left_throttle = 0.0;
      float right_throttle = 0.0;
      if (isTurn)
      {
        left_throttle = yaw;
        right_throttle = yaw * -1.0;
      }
      else
      {
        left_throttle = throttle;
        right_throttle = throttle;
      }
      
      left.TorqueCommand(left_throttle);
      right.TorqueCommand(right_throttle);
      Serial.print("Throttle");
      Serial.println(throttle);
      /*
      // Left
      else if (data[2] == 0 && data[3] > 150)
      {
          digitalWrite(LEFT_A, LOW);
          digitalWrite(LEFT_B, HIGH);

          digitalWrite(RIGHT_A, LOW);
          digitalWrite(RIGHT_B, HIGH);
      }
      // Right
      else if (data[2] == 1 && data[3] > 150)
      {
         digitalWrite(LEFT_A, HIGH);
         digitalWrite(LEFT_B, LOW);

         digitalWrite(RIGHT_A, HIGH);
         digitalWrite(RIGHT_B, LOW);
      }
      else if (data[5] <= 150 && data[3] < 150)
      {
         digitalWrite(LEFT_A, LOW);
         digitalWrite(LEFT_B, LOW);

         digitalWrite(RIGHT_A, LOW);
         digitalWrite(RIGHT_B, LOW);
      }
      */
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
   if (nextReadTime < msTicks || pulseDone)
   {
     float distInch = delta / 148;
     //Serial.println(distInch);
     pulseDone = false;
     phase = false;
     digitalWrite(SENSOR_ENABLE_R, HIGH);
     delayMicroseconds(5);
     digitalWrite(SENSOR_TRIGGER_R, LOW);
     delayMicroseconds(5);
     digitalWrite(SENSOR_TRIGGER_R, HIGH);
     delayMicroseconds(10);
     digitalWrite(SENSOR_TRIGGER_R, LOW);
     nextReadTime = msTicks + 50;
   }
   if (timeoutTime < msTicks)
   {
      digitalWrite(RIGHT_A, LOW);
      digitalWrite(RIGHT_B, LOW);
      digitalWrite(LEFT_A, LOW);
      digitalWrite(LEFT_B, LOW);
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
