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

// System Variables
long msTicks = 0;
float lastThrottle = 0.0;

byte lastRegister1Val = 0x00;
byte lastRegister2Val = 0x00;

#define LED0 A0
#define LED1 A1
#define LED2 A2
#define LED3 A3

#define TOP_BLUE A4
#define TOP_GREEN A6
#define LED6 5
#define LED7 4

const byte address[6] = "39421";
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

byte leds[8] = {LED0, LED1, LED2, LED3, TOP_BLUE, TOP_GREEN, LED6, LED7};
bool ledStates[8] = {false, false, false, false, false, false, false, false};

RF24 radio(8, 10); // CE, CSN

void setup() 
{
   Serial.begin(115200);

   // Set initial state
   for (int i = 0; i < 8; i++)
   {
      pinMode(leds[i], OUTPUT);
      digitalWrite(leds[i], LOW);
   }
   
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

      byte buttonRegister1 = data[6];
      byte buttonRegister2 = data[7];

      if (!(buttonRegister1 & 0x01) && (buttonRegister1 & 0x01) != (lastRegister1Val & 0x01))
      {
         ledStates[0] = !ledStates[0];
         digitalWrite(LED1, ledStates[0]);
         
      }
      if ((buttonRegister1 & 0x02) && (buttonRegister1 & 0x02) != (lastRegister1Val & 0x02))
      {
         ledStates[1] = !ledStates[1];
         digitalWrite(LED2, ledStates[1]);
      }
      if ((buttonRegister1 & 0x04) && (buttonRegister1 & 0x04) != (lastRegister1Val & 0x04))
      {
         ledStates[2] = !ledStates[2];
         digitalWrite(LED3, ledStates[2]);
      }
      if ((buttonRegister1 & 0x08) && (buttonRegister1 & 0x08) != (lastRegister1Val & 0x08))
      {
         ledStates[3] = !ledStates[3];
          digitalWrite(LED6, ledStates[3]); //ledStates[3]);
      }
      if ((buttonRegister1 & 0x10) && (buttonRegister1 & 0x10) != (lastRegister1Val & 0x10))
      {
         ledStates[4] = !ledStates[4];
         digitalWrite(LED7, ledStates[4]); //ledStates[4]);
      }
      if ((buttonRegister1 & 0x20) && (buttonRegister1 & 0x20) != (lastRegister1Val & 0x20))
      {
         ledStates[5] = !ledStates[5];
         digitalWrite(LED0, ledStates[5]);
      }
      lastRegister1Val = buttonRegister1;
      lastRegister2Val = buttonRegister2;
      lastThrottle = throttle;
      delay(5);
   }
}

/****************************************
* DEBUG PRINT
* DESC: Prints debug statements about the state
* of the system.
****************************************/
void DebugPrint()
{
  
}
