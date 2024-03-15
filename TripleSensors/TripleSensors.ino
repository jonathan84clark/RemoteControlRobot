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

// Power Pulse Variables
boolean phase = false;
boolean pulseDone = false;
float pulseTime = 0.0;
boolean powerIsPulse = false;
boolean lights_on = false;
boolean last_state = false;
boolean gettingData = false;
boolean dbgLedOn = false;

// Radio variables
#if ROBOT_CONFIG == SAND_RUNNDER
const byte address[6] = "39422";
#elif ROBOT_CONFIG == MEDIUM_BOT
const byte address[6] = "39427";
#elif ROBOT_CONFIG == BOT_V2
const byte address[6] = "39428";
#elif ROBOT_CONFIG == SUPER_POWERS_A4
const byte address[6] = "39424";
#else
const byte address[6] = "39421";
#endif
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#ifdef NUM_SENSORS
int trigger[] = {SENSOR_TRIGGER_L, SENSOR_TRIGGER_M, SENSOR_TRIGGER_R};
int enable[] = {SENSOR_ENABLE_L, SENSOR_ENABLE_M, SENSOR_ENABLE_R};
float sensorReadings[] = {0.0, 0.0, 0.0};
int sensorIndex = 0;
#endif

RF24 radio(RADIO_CE, RADIO_CSN); // CE, CSN
// Setup the drive system
#ifdef STEER_A
Drive drive(STEER_A, STEER_B, THROTTLE_SCALE, 0.0, MAIN_A, MAIN_B, THROTTLE_SCALE, 0.0, MODE_PIVOT_STEER);
#else
Drive drive(LEFT_A, LEFT_B, THROTTLE_SCALE, 0.0, RIGHT_A, RIGHT_B, THROTTLE_SCALE, 0.0, MODE_DIFF_STEER);
#endif

AutoControl autoControl;

/***********************************************************
* SET HEADLIGHTS
***********************************************************/
void SetHeadlights(bool state)
{
#ifdef WHITE_HEADLIGHTS
   digitalWrite(WHITE_HEADLIGHTS, state);
#endif
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
#ifdef IR_LEFT
   pinMode(IR_LEFT, INPUT);
#endif
#ifdef IR_MID
   pinMode(IR_MID, INPUT);
#endif
#ifdef IR_RIGHT
   pinMode(IR_RIGHT, INPUT);
#endif
   pinMode(A3, INPUT);
#ifdef WHITE_HEADLIGHTS
   pinMode(WHITE_HEADLIGHTS, OUTPUT);
#endif
#ifdef AUX_HEADLIGHTS
   pinMode(AUX_HEADLIGHTS, OUTPUT);
#endif
#ifdef BUZZER
   pinMode(BUZZER, OUTPUT);
#endif
   //pinMode(PULSE_PIN, OUTPUT);
   //digitalWrite(PULSE_PIN, HIGH);

#ifdef DEBUG_LED
   pinMode(DEBUG_LED, OUTPUT);
   digitalWrite(DEBUG_LED, LOW);
#endif


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

   //autoControl.SetEnabled(1);
   autoControl.Setup(&drive);
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
         if (data[6] & 0x01)
         {
            digitalWrite(BUZZER, HIGH);
         }
         else
         {
            digitalWrite(BUZZER, LOW);
         }
#endif
         gettingData = true;
         timeoutTime = msTicks + 500;
      }
      delay(5);
   }
   if (nextLightPulse < msTicks && gettingData)
   {
#ifdef DEBUG_LED
       dbgLedOn = !dbgLedOn;
       digitalWrite(DEBUG_LED, dbgLedOn);
       nextLightPulse = msTicks + 100;
#endif
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
#ifdef DEBUG_LED
       gettingData = false;
       dbgLedOn = false;
       digitalWrite(DEBUG_LED, dbgLedOn);
#endif
       SystemsOff();
   }
   if (pulseTime < msTicks)
   {
       if (powerIsPulse)
       {
          powerIsPulse = false;
          //digitalWrite(PULSE_PIN, HIGH);
          pulseTime = msTicks + 5000;
       }
       else
       {
          powerIsPulse = true;
          //digitalWrite(PULSE_PIN, LOW);
          pulseTime = msTicks + 200;
       }
   }
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
