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
RF24 radio(9, 10); // CE, CSN
const byte address[6] = "39421";
boolean lights_on = false;
boolean last_state = false;
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void setup() 
{
   Serial.begin(9600);
   pinMode(A1, OUTPUT);
   pinMode(A2, OUTPUT);
   pinMode(A3, OUTPUT);
   pinMode(A4, OUTPUT);

   pinMode(8, OUTPUT);
   pinMode(6, OUTPUT);

   pinMode(3, OUTPUT);
   pinMode(5, OUTPUT);
   
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
      
      if (data[4] == 0 && data[5] > 150)
      {
          // Left Forward
          digitalWrite(8, HIGH);
          digitalWrite(6, LOW);

          // Right forward
          digitalWrite(3, HIGH);
          digitalWrite(5, LOW);
      }
      else if (data[4] == 1 && data[5] > 150)
      {
          // Left Forward
         digitalWrite(8, LOW);
         digitalWrite(6, HIGH);

         // Right forward
         digitalWrite(3, LOW);
         digitalWrite(5, HIGH);
      }
      else if (data[2] == 0 && data[3] > 150)
      {
          // Left Forward
         digitalWrite(8, HIGH);
         digitalWrite(6, LOW);

         // Right forward
         digitalWrite(3, LOW);
         digitalWrite(5, HIGH);
      }
      else if (data[2] == 1 && data[3] > 150)
      {
          // Left Forward
         digitalWrite(8, LOW);
         digitalWrite(6, HIGH);

         // Right forward
         digitalWrite(3, HIGH);
         digitalWrite(5, LOW);
      }
      else if (data[5] <= 150 && data[3] < 150)
      {
         // Left Forward
         digitalWrite(8, LOW);
         digitalWrite(6, LOW);

         // Right forward
         digitalWrite(3, LOW);
         digitalWrite(5, LOW);
      }
      
      delay(5);
   } 
}
