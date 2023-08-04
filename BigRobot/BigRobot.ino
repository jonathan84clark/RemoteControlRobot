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

#define CE 20
#define CSN 17


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

const byte address[6] = "39421";
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

RF24 radio(CE, CSN); // CE, CSN


/***********************************************************
* SET HEADLIGHTS
***********************************************************/
void SystemsOff()
{
   
}

void setup() 
{
   Serial.begin(9600);
   
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
      Serial.println("Got data!");
      Serial.println(throttle);
      //drive.ManualControl(throttle, yaw);
      timeoutTime = msTicks + 500;
      delay(5);
   }
   if (nextReadTime < msTicks || pulseDone)
   {
      //ReadSensors();
   }
   if (timeoutTime < msTicks)
   {
       SystemsOff();
   }
}
