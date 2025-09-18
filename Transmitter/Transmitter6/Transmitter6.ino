/***************************************************************
* TRANSMITTER 6
* DESC: The transmitter code takes data from a joystick and buttons
* and sends a heartbeat message to the reciever.
* Author: Jonathan L Clark
* Date: 12/22/2019
* Update: 12/25/2019, Added full support for all buttons and systems
* Update: 1/10/2020, Cleaned up the code, fixed an issue where the 
* joystick values were overflowing.
* Update: 3/5/2020, Cleaned up the code, added additional buttons and
* added the ability to easily add more buttons.
* Update: 5/1/2021, Added support for jumper-based configuration
**************************************************************/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(10, 9); // CE, CSN
            
int center_y = 0;
int center_x = 0;
byte buttonRegister1 = 0x00;
byte buttonRegister2 = 0x00;
byte device_address = 130;
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
const byte address[6] = "99885";

#define JOYSTICK_X   A0
#define JOYSTICK_Y   A1
#define JOYSTICK_BTN  A5
#define BTN_DOWN      4
#define BTN_RIGHT     5
#define BTN_UP        6
#define BTN_MID       7
#define BTN_LEFT      8

#define SYNC_BTN 2

#define CFG_BIT0 A2
#define CFG_BIT1 A3
#define CFG_BIT2 A4

#define LED_PIN 3

#define SYSTEM_ID 0x4D // Indicates that this is the Jonathan remote control system

// Map of all the button locations.
static int button_map[] = {JOYSTICK_BTN, BTN_DOWN, BTN_RIGHT, BTN_UP,
                           BTN_MID, BTN_LEFT};

void setup() 
{
    Serial.begin(9600);
    pinMode(CFG_BIT0, INPUT);
    pinMode(CFG_BIT1, INPUT);
    pinMode(CFG_BIT2, INPUT);
    pinMode(JOYSTICK_X, INPUT);
    pinMode(JOYSTICK_Y, INPUT);
    pinMode(JOYSTICK_BTN, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_RIGHT, INPUT_PULLUP);
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_MID, INPUT_PULLUP);
    pinMode(BTN_LEFT, INPUT_PULLUP);
    pinMode(SYNC_BTN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);


    byte cfg_address = 0;

    // Handle negative high values
    cfg_address = (unsigned int)(analogRead(CFG_BIT0) < 100) | (unsigned int)(analogRead(CFG_BIT1) < 100) << 1;
    unsigned int bitVal2 = ~digitalRead(CFG_BIT2) & 0x0001;
    cfg_address |= bitVal2 << 2;

    // Handle positive high values
    cfg_address = (unsigned int)(analogRead(CFG_BIT0) > 1000) | (unsigned int)(analogRead(CFG_BIT1) > 1000) << 1;
    cfg_address |= (unsigned int)digitalRead(CFG_BIT2) << 2;
    cfg_address = (~cfg_address & 0x1F);

    device_address += cfg_address;
    Serial.print("Device address: ");
    Serial.println(device_address);
    center_y = analogRead(JOYSTICK_Y);
    center_x = analogRead(JOYSTICK_X);
      // Setup and configure rf radio
    if (!radio.begin()) {
     Serial.println(F("radio hardware not responding!"));
    while (true) {
      // hold in an infinite loop
    }
  }
    radio.openWritingPipe(address); //Setting the address where we will send the data
    radio.setPALevel(RF24_PA_MAX);  //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
    radio.stopListening();          //This sets the module as transmitter
}
void loop()
{
    // Calculate joystick data
    analogWrite(LED_PIN, 10);
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
    for (int i = 0; i < 7; i++)
    {
        if (digitalRead(button_map[i]) == 0)
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
    buttonRegister1 ^= 0x01;
    
    // Now we packetize it
    data[0] = SYSTEM_ID;
    if (digitalRead(SYNC_BTN) == LOW)
    {
        data[0] |= 0x80; // Indicates to the system we are okay syncing
        Serial.println("Sync button down");
    }
    //buttonRegister1 &= ~0x02;
    data[1] = device_address; // Tell the other device what our remote control address is
    data[2] = xDir;
    data[3] = (byte)joystickXAdjusted;
    data[4] = yDir;
    data[5] = (byte)joystickYAdjusted;
    data[6] = buttonRegister1;
    data[7] = buttonRegister2;
    //Serial.println(buttonRegister1);
    radio.write(&data, sizeof(data));  //Sending the message to receiver 
    delay(50);
    analogWrite(LED_PIN, 0);
    delay(50);
}
