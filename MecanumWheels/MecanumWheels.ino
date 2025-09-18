#include <EEPROM.h>
#include "Motor.h"
#include <RF24.h>
#include <nRF24L01.h>

#define SYSTEM_ID 0x4D // Indicates that this is the Jonathan remote control system

#define RADIO_CE 14
#define RADIO_CSN 15
 
// Radio variables
const byte address[6] = "99885";
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
byte paired_remote_addr = 0x00;
bool controller_paired = false;
long msTicks = 0;
long timeoutTime = 0;
boolean gettingData = false;

RF24 radio(RADIO_CE, RADIO_CSN); // CE, CSN

Motor motor1(10, 9, 1.0, 0.0);
Motor motor2(8, 7, 1.0, 0.0);
Motor motor3(6, 5, 1.0, 0.0);
Motor motor4(3, 4, 1.0, 0.0);

void setup() 
{
   byte last_remote_addr = 0;
   EEPROM.get(10, last_remote_addr);
   Serial.print("Last eeprom address: ");
   Serial.println(last_remote_addr);
   paired_remote_addr = last_remote_addr;
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
      byte sync_set = 0;
      byte remote_address = data[1];
      byte read_system_id = (data[0] & ~0x80);
      Serial.println("Got data");
      // Once we pair we won't allow another pair until a reboot
      if ((data[0] & 0x80) == 0x80 && !controller_paired)
      {
         sync_set = 1;
         paired_remote_addr = remote_address;
         EEPROM.put(10, paired_remote_addr);
         //controller_paired = true;
         Serial.print("Paired to remote with addr: ");
         Serial.println(paired_remote_addr);
      }
      if (read_system_id != SYSTEM_ID || paired_remote_addr != remote_address)
      {

      }
      else
      {
         if (!controller_paired)
         {
             controller_paired = true;
         }
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
         //if (!autoControl.GetEnabled())
         //{
         //   drive.ManualControl(throttle, yaw);
         //}
         Serial.println("Data"); 
         gettingData = true;
         timeoutTime = msTicks + 500;
      }
      delay(5);
   }

}
