/***************************************************************
* TRANSMITTER
* DESC: The transmitter code takes data from a joystick and buttons
* and sends a heartbeat message to the reciever.
* Author: Jonathan L Clark
* Date: 12/22/2019
* Update: 12/25/2019, Added full support for all buttons and systems
* Update: 1/10/2020, Cleaned up the code, fixed an issue where the 
* joystick values were overflowing.
**************************************************************/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN         
const byte address[6] = "39421";     //Byte of array representing the address. This is the address where we will send the data. This should be same on the receiving side.
int button_pin = 2;
int center_y = 0;
int center_x = 0;
byte buttonRegister = 0x00;
byte data[8] = {0xC1, 0xE2, 0, 0, 0, 0, 0, 0};

#define JOYSTICK_BTN A2
#define JOYSTICK_X   A0
#define JOYSTICK_Y   A1
#define BTN_DOWN      2
#define BTN_RIGHT     3
#define BTN_UP        4
#define BTN_MID       5
#define BTN_LEFT      6

void setup() 
{
    Serial.begin(9600);
    pinMode(A7, OUTPUT);
    pinMode(A6, OUTPUT);
    pinMode(A5, OUTPUT);
    pinMode(JOYSTICK_X, INPUT);
    pinMode(JOYSTICK_Y, INPUT);
    pinMode(JOYSTICK_BTN, INPUT);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_RIGHT, INPUT_PULLUP);
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_MID, INPUT_PULLUP);
    pinMode(BTN_LEFT, INPUT_PULLUP);
    digitalWrite(A7, LOW);
    digitalWrite(A6, LOW);
    digitalWrite(A5, LOW);
    center_y = analogRead(JOYSTICK_Y);
    center_x = analogRead(JOYSTICK_X);
    Serial.println(radio.begin());                  //Starting the Wireless communication
    radio.openWritingPipe(address); //Setting the address where we will send the data
    radio.setPALevel(RF24_PA_MAX);  //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
    radio.stopListening();          //This sets the module as transmitter
}
void loop()
{
    // Calculate joystick data
    int joystickYMag = center_y - analogRead(JOYSTICK_Y);
    int joystickXMag = center_x - analogRead(JOYSTICK_X);
    float joystickYAdjusted = (float)joystickYMag * 255.0 / (float)center_y;
    float joystickXAdjusted = (float)joystickXMag * 255.0 / (float)center_x;
    byte xDir = 0;
    if (joystickXAdjusted < 0.0)
    {
       joystickXAdjusted *= -1.0;
       xDir = 1;
    }
    byte yDir = 0;
    if (joystickYAdjusted < 0.0)
    {
      joystickYAdjusted *= -1.0;
      yDir = 1;
    }
    buttonRegister = 0x00;
    if (digitalRead(JOYSTICK_BTN) == 1)
    {
       buttonRegister |= 0x01;
    }
    if (digitalRead(BTN_DOWN) == 0)
    {
       buttonRegister |= 0x02;
    }
    if (digitalRead(BTN_RIGHT) == 0)
    {
       buttonRegister |= 0x04;
    }
    if (digitalRead(BTN_UP) == 0)
    {
       buttonRegister |= 0x08;
    }
    if (digitalRead(BTN_MID) == 0)
    {
       buttonRegister |= 0x10;
    }
    if (digitalRead(BTN_LEFT) == 0)
    {
       buttonRegister |= 0x20;
    }
    if (joystickXAdjusted > 255.0)
    {
       joystickXAdjusted = 255.0;
    }
    if (joystickYAdjusted > 255)
    {
      joystickYAdjusted = 255.0;
    }
    

    // Now we packetize it
    data[2] = xDir;
    data[3] = (byte)joystickXAdjusted;
    data[4] = yDir;
    data[5] = (byte)joystickYAdjusted;
    data[6] = buttonRegister;
    radio.write(&data, sizeof(data));  //Sending the message to receiver 
    delay(100);
}
