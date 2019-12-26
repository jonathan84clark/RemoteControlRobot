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
RF24 radio(8, 10); // CE, CSN
const byte address[6] = "39421";
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void setup() 
{
   Serial.begin(9600);

   pinMode(9, OUTPUT);
   pinMode(6, OUTPUT);

   pinMode(3, OUTPUT);
   pinMode(5, OUTPUT);

   // Left Forward
   //digitalWrite(9, HIGH);
   //digitalWrite(6, LOW);

   //
   //digitalWrite(3, HIGH);
   //digitalWrite(5, LOW);
   
   radio.begin();
   radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
   radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
   radio.startListening();              //This sets the module as receiver
}

void loop()
{
   if (radio.available())              //Looking for the data.
   {
      radio.read(&data, sizeof(data));    //Reading the data
      if (data[4] == 0 && data[5] > 50)
      {
         digitalWrite(3, HIGH);
         digitalWrite(5, LOW);

         digitalWrite(9, HIGH);
         digitalWrite(6, LOW);
      }
      else if (data[4] == 1 && data[5] > 50)
      {
         digitalWrite(3, LOW);
         digitalWrite(5, HIGH);

         digitalWrite(9, LOW);
         digitalWrite(6, HIGH);
      }
      else
      {
         digitalWrite(3, LOW);
         digitalWrite(5, LOW);

         digitalWrite(9, LOW);
         digitalWrite(6, LOW);
      }
      
      delay(5);
   } 
}
