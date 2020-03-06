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

const byte address[6] = "39423";
boolean lights_on = false;
boolean last_state = false;
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define RIGHT_MOTOR 5
#define LEFT_MOTOR 6

#define WHITE_HEADLIGHTS A3

#define SENSOR_TRIGGER A0
#define SENSOR_ECHO 2

void setup() 
{
   Serial.begin(9600);
   pinMode(WHITE_HEADLIGHTS, OUTPUT);

   pinMode(RIGHT_MOTOR, OUTPUT);
   pinMode(LEFT_MOTOR, OUTPUT);

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
      if (data[4] == 0 && data[5] > 150)
      {
          analogWrite(LEFT_MOTOR, 240);
          analogWrite(RIGHT_MOTOR, 10);
      }
      else if (data[4] == 1 && data[5] > 150)
      {
          analogWrite(LEFT_MOTOR, 10);
          analogWrite(RIGHT_MOTOR, 240);
      }
      else if (data[2] == 0 && data[3] > 150)
      {
          analogWrite(LEFT_MOTOR, 240);
          analogWrite(RIGHT_MOTOR, 240);
      }
      else if (data[2] == 1 && data[3] > 150)
      {
          analogWrite(LEFT_MOTOR, 10);
          analogWrite(RIGHT_MOTOR, 10);
      }
      else if (data[5] <= 150 && data[3] < 150)
      {
          analogWrite(LEFT_MOTOR, 0);
          analogWrite(RIGHT_MOTOR, 0);
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
   if (nextReadTime < msTicks || pulseDone)
   {
     float distInch = delta / 148;
     Serial.println(distInch);
     pulseDone = false;
     phase = false;
     digitalWrite(SENSOR_TRIGGER, LOW);
     delayMicroseconds(5);
     digitalWrite(SENSOR_TRIGGER, HIGH);
     delayMicroseconds(10);
     digitalWrite(SENSOR_TRIGGER, LOW);
     nextReadTime = msTicks + 50;
   }
   if (timeoutTime < msTicks)
   {
      digitalWrite(WHITE_HEADLIGHTS, LOW);
      analogWrite(LEFT_MOTOR, 0);
      analogWrite(RIGHT_MOTOR, 0);
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
