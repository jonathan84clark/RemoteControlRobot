/***************************************************************
* TRIPLE SENSORS
* DESC: The triple sensors is the basis for all new robot designs
* going forward.
* Author: Jonathan L Clark
* Date: 12/22/2019
**************************************************************/
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "Drive.h"
#include "Config.h"

// System Variables
long msTicks = 0;
long timeoutTime = 0;
long debounceTime = 0;
unsigned long nextReadTime = 0;
unsigned long pulseDownTime = 0;
unsigned long delta = 0;
unsigned long startMicros = 0;
long nextDebugTime = 0;

// Power Pulse Variables
boolean phase = false;
boolean pulseDone = false;
float pulseTime = 0.0;
boolean powerIsPulse = false;
boolean lights_on = false;
boolean last_state = false;

// Radio variables
const byte address[6] = "39421";
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#ifdef NUM_SENSORS
int trigger[] = {SENSOR_TRIGGER_L, SENSOR_TRIGGER_M, SENSOR_TRIGGER_R};
int enable[] = {SENSOR_ENABLE_L, SENSOR_ENABLE_M, SENSOR_ENABLE_R};
float sensorReadings[] = {0.0, 0.0, 0.0};
int sensorIndex = 0;
#endif

RF24 radio(RADIO_CE, RADIO_CSN); // CE, CSN
// Setup the drive system
Drive drive(LEFT_A, LEFT_B, THROTTLE_SCALE, 0.0, RIGHT_A, RIGHT_B, THROTTLE_SCALE, 0.0, MODE_DIFF_STEER);

/***********************************************************
* SET HEADLIGHTS
***********************************************************/
void SetHeadlights(bool state)
{
   digitalWrite(WHITE_HEADLIGHTS, state);
#ifdef AUX_HEADLIGHTS
   digitalWrite(AUX_HEADLIGHTS, state);
#endif
}

/***********************************************************
* SET HEADLIGHTS
***********************************************************/
void SystemsOff()
{
   drive.ManualControl(0.0, 0.0);
   SetHeadlights(false);
}

void setup() 
{
   Serial.begin(9600);
   pinMode(WHITE_HEADLIGHTS, OUTPUT);
#ifdef AUX_HEADLIGHTS
   pinMode(AUX_HEADLIGHTS, OUTPUT);
#endif
#ifdef BUZZER
   pinMode(BUZZER, OUTPUT);
#endif
   pinMode(PULSE_PIN, OUTPUT);
   digitalWrite(PULSE_PIN, HIGH);


#ifdef NUM_SENSORS
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
      if ((data[6] & 0x10) == 0x10 && debounceTime < msTicks)
      {
          if (lights_on)
          {
              SetHeadlights(false);
              lights_on = false;
          }
          else
          {
              SetHeadlights(true);
              lights_on = true;
          }
          debounceTime = msTicks + 500;
      }
#ifdef BUZZER
      if ((data[6] & 0x01) == 0x01)
      {
          digitalWrite(BUZZER, HIGH);
      }
      else
      {
          digitalWrite(BUZZER, LOW);
      }
#endif
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
       SystemsOff();
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
#ifdef NUM_SENSORS
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
#endif
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
