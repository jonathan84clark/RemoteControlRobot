#include <EEPROM.h>
#include "Motor.h"
#include "Drive.h"
#include <RF24.h>
#include <nRF24L01.h>

#define SYSTEM_ID 0x4D // Indicates that this is the Jonathan remote control system

#define RADIO_CE 14
#define RADIO_CSN 15

#define LEFT_FRONT_A 9
#define LEFT_FRONT_B 10
#define LEFT_REAR_A 7
#define LEFT_REAR_B 8
#define RIGHT_FRONT_A 5
#define RIGHT_FRONT_B 6
#define RIGHT_REAR_A 4
#define RIGHT_REAR_B 3

// Radio variables
const byte address[6] = "99885";
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
byte paired_remote_addr = 0x00;
bool controller_paired = false;
bool lock_angle = false;
long msTicks = 0;
long timeoutTime = 0;
boolean gettingData = false;

RF24 radio(RADIO_CE, RADIO_CSN); // CE, CSN

Motor leftFrontMotor(9, 10, 1.0, 0.0);
Motor leftRearMotor(7, 8, 1.0, 0.0);
Motor rightFrontMotor(5, 6, 1.0, 0.0);
Motor rightRearMotor(4, 3, 1.0, 0.0);

Drive drive(LEFT_FRONT_A, LEFT_FRONT_B, LEFT_REAR_A, LEFT_REAR_B, RIGHT_FRONT_A, RIGHT_FRONT_B, RIGHT_REAR_A, RIGHT_REAR_B);

void setup() 
{
   Serial.begin(115200);
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
            //throttle = 1.0;
         }
         else if (data[6] & 0x02)
         {
            //throttle = -1.0;
         }
         if (data[6] & 0x10)
         {
            lock_angle = true;
         }
         else
         {
            lock_angle = false;
         }
         //Serial.println(data[6]);
         drive.ManualControl(throttle, yaw, lock_angle);
         Serial.println("Data"); 
         gettingData = true;
         timeoutTime = msTicks + 500;
      }
      delay(5);
   }
   if (timeoutTime < msTicks)
   {
       drive.ManualControl(0.0, 0.0, false);
       timeoutTime = msTicks + 500;
   }

}
