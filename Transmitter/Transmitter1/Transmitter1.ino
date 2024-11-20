/***************************************************************
* TRANSMITTER
* DESC: The transmitter code takes data from a joystick and buttons
* and sends a heartbeat message to the reciever.
* Author: Jonathan L Clark
* Date: 12/22/2019
* Update: 12/25/2019, Added full support for all buttons and systems
* Update: 1/10/2020, Cleaned up the code, fixed an issue where the 
* joystick values were overflowing.
* Update: 3/5/2020, Cleaned up the code, added additional buttons and
* added the ability to easily add more buttons.
**************************************************************/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN
int center_y = 0;
int center_x = 0;
byte buttonRegister1 = 0x00;
byte buttonRegister2 = 0x00;
byte device_address = 95;
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int pulse_counter = 0;
boolean powerIsPulse = false;
const byte address[6] = "99885";

#define JOYSTICK_X   A0
#define JOYSTICK_Y   A1
#define JOYSTICK_BTN A2
#define BTN_DOWN      2
#define BTN_RIGHT     3
#define BTN_UP        4
#define BTN_MID       5
#define BTN_LEFT      6

#define POWER_PULSE   7

#define SYSTEM_ID 0x4D // Indicates that this is the Jonathan remote control system

// Map of all the button locations.
static int button_map[] = {JOYSTICK_BTN, BTN_DOWN, BTN_RIGHT, BTN_UP,
                           BTN_MID, BTN_LEFT};

void setup() 
{
    Serial.begin(9600);
    pinMode(JOYSTICK_X, INPUT);
    pinMode(JOYSTICK_Y, INPUT);
    pinMode(JOYSTICK_BTN, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_RIGHT, INPUT_PULLUP);
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_MID, INPUT_PULLUP);
    pinMode(BTN_LEFT, INPUT_PULLUP);
    pinMode(POWER_PULSE, OUTPUT);
    digitalWrite(POWER_PULSE, HIGH);
    center_y = analogRead(JOYSTICK_Y);
    center_x = analogRead(JOYSTICK_X);
    radio.begin();                  //Starting the Wireless communication
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
    buttonRegister1 = 0x00;
    buttonRegister2 = 0x00;
    byte mask = 0x01;
    // Check all of our buttons and pack the button data
    for (int i = 0; i < 10; i++)
    {
        if (i > 0 && digitalRead(button_map[i]) == 0)
        {
            if (i > 7)
            {
               buttonRegister2 |= mask;
            }
            else
            {
               buttonRegister1 |= mask;
            }
        }
        else if (i == 0)
        {
            if (digitalRead(button_map[i]) == 1)
            {
               buttonRegister1 |= mask;
            }
        }
        mask = mask << 1;
        if (i == 7)
        {
           mask = 0x01; // Reset the mask to move to the next register
        }
    }
    if (joystickXAdjusted > 255.0)
    {
       joystickXAdjusted = 255.0;
    }
    if (joystickYAdjusted > 255)
    {
      joystickYAdjusted = 255.0;
    }
    // Hanlde pulsing the power supply
    if (powerIsPulse && pulse_counter > 3)
    {
       powerIsPulse = false;
       pulse_counter = 0;
       digitalWrite(POWER_PULSE, HIGH);
    }
    else if (!powerIsPulse && pulse_counter > 40)
    {
       powerIsPulse = true;
       pulse_counter = 0;
       digitalWrite(POWER_PULSE, LOW);
    }
    pulse_counter++;
    
    // Now we packetize it
    data[0] = SYSTEM_ID;
    if ((buttonRegister1 & 0x01) == 0x01)
    {
        data[0] |= 0x80; // Indicates to the system we are okay syncing
        Serial.println("Sync button down");
    }
    data[1] = device_address;
    data[2] = xDir;
    data[3] = (byte)joystickXAdjusted;
    data[4] = yDir;
    data[5] = (byte)joystickYAdjusted;
    data[6] = buttonRegister1;
    data[7] = buttonRegister2;
    //Serial.println(buttonRegister1);
    radio.write(&data, sizeof(data));  //Sending the message to receiver 
    delay(100);
}
