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
#include "Drive.h"
#if ROBOT_TRIPLE
RF24 radio(10, 9); // CE, CSN
#else
RF24 radio(8, 10); // CE, CSN
#endif
long msTicks = 0;
long timeoutTime = 0;
long debounceTime = 0;
unsigned long nextReadTime = 0;
unsigned long pulseDownTime = 0;
unsigned long delta = 0;
unsigned long startMicros = 0;
long nextDebugTime = 0;
boolean phase = false;
boolean pulseDone = false;
float pulseTime = 0.0;
boolean powerIsPulse = false;
bool isTimeout = false;

const byte address[6] = "39421";
boolean lights_on = false;
boolean last_state = false;
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define ROBOT_TRIPLE 0
#define ROBOT_FASTROV 1

#if ROBOT_TRIPLE
#define LEFT_A 5
#define LEFT_B 3

#define RIGHT_A 6
#define RIGHT_B 7
#define PULSE_PIN 8
#define WHITE_HEADLIGHTS 4
#define THROTTLE_SCALE 1.0
#else
#define LEFT_A 5
#define LEFT_B 6

#define RIGHT_A 3
#define RIGHT_B 9

#define PULSE_PIN 7
#define WHITE_HEADLIGHTS A0
#define AUX_HEADLIGHTS A1
#define THROTTLE_SCALE 0.80
#endif

#define NUM_SENSORS 3
#define SENSOR_TRIGGER_L A4
#define SENSOR_TRIGGER_M A3
#define SENSOR_TRIGGER_R A2

#define SENSOR_ENABLE_L A7
#define SENSOR_ENABLE_M A6
#define SENSOR_ENABLE_R A5
#define SENSOR_ECHO 2

int trigger[] = {SENSOR_TRIGGER_L, SENSOR_TRIGGER_M, SENSOR_TRIGGER_R};
int enable[] = {SENSOR_ENABLE_L, SENSOR_ENABLE_M, SENSOR_ENABLE_R};
float sensorReadings[] = {0.0, 0.0, 0.0};
int sensorIndex = 0;
// Setup the drive system
Drive drive(LEFT_A, LEFT_B, THROTTLE_SCALE, 0.0, RIGHT_A, RIGHT_B, THROTTLE_SCALE, 0.0, MODE_DIFF_STEER);

void setup() 
{
   Serial.begin(9600);
   pinMode(WHITE_HEADLIGHTS, OUTPUT);
#if ROBOT_FASTROV
   pinMode(AUX_HEADLIGHTS, OUTPUT);
#endif
   pinMode(PULSE_PIN, OUTPUT);
   digitalWrite(PULSE_PIN, HIGH);


#if ROBOT_TRIPLE
   for (int i = 0; i < NUM_SENSORS; i++)
   {
      pinMode(trigger[i], OUTPUT);
      pinMode(enable[i], OUTPUT);
      digitalWrite(trigger[i], LOW);
      digitalWrite(enable[i], LOW);
   }
   attachInterrupt(digitalPinToInterrupt(SENSOR_ECHO), echoIsr, CHANGE);
#endif
   
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
      drive.ManualControl(throttle, yaw);
      isTimeout = false;
      if ((data[6] & 0x10) == 0x10 && debounceTime < msTicks)
      {
          if (lights_on)
          {
              digitalWrite(WHITE_HEADLIGHTS, LOW);
#if ROBOT_FASTROV
              digitalWrite(AUX_HEADLIGHTS, LOW);
#endif
              lights_on = false;
          }
          else
          {
              digitalWrite(WHITE_HEADLIGHTS, HIGH);
#if ROBOT_FASTROV
              digitalWrite(AUX_HEADLIGHTS, HIGH);
#endif
              lights_on = true;
          }
          debounceTime = msTicks + 500;
      }
      timeoutTime = msTicks + 500;
      delay(5);
   }
   if (nextDebugTime < msTicks)
   {
      DebugPrint();
   }
   if (nextReadTime < msTicks || pulseDone)
   {
      //ReadSensors();
   }
   if (timeoutTime < msTicks)
   {
      digitalWrite(RIGHT_A, LOW);
      digitalWrite(RIGHT_B, LOW);
      digitalWrite(LEFT_A, LOW);
      digitalWrite(LEFT_B, LOW);
      digitalWrite(WHITE_HEADLIGHTS, LOW);
      powerIsPulse = false;
      digitalWrite(PULSE_PIN, HIGH);
      pulseTime = msTicks + 5000;
      isTimeout = true;
   }
   if (pulseTime < msTicks && !isTimeout)
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
  /*
   Serial.print("Dist1: ");
   Serial.print(sensorReadings[0]);
   Serial.print("Dist2: ");
   Serial.print(sensorReadings[1]);
   Serial.print("Dist3: ");
   Serial.println(sensorReadings[2]);
   nextDebugTime = msTicks + 500;
   */
}

/****************************************
* READ SENSORS
* DESC: Reads the ultrasonic distance sensors
****************************************/
void ReadSensors()
{
   float distInch = (float)delta / 148.0;
   if (pulseDone) // We finished a read, cache the distance
   {
      sensorReadings[sensorIndex] = distInch;
   }
   pulseDone = false;
   phase = false;
   digitalWrite(enable[sensorIndex], LOW);
   sensorIndex++;
   if (sensorIndex == NUM_SENSORS)
   {
      sensorIndex = 0;
   }
   digitalWrite(enable[sensorIndex], HIGH);
   delayMicroseconds(5);
   digitalWrite(trigger[sensorIndex], LOW);
   delayMicroseconds(5);
   digitalWrite(trigger[sensorIndex], HIGH);
   delayMicroseconds(10);
   digitalWrite(trigger[sensorIndex], LOW);
   nextReadTime = msTicks + 50;
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
