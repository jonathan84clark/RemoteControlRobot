# Remote Control Robot
The remote control robot stack contains software designed for remote control robots. The code contains several projects for different robot types and contains code for the transmitter.  

## Hardware Requirements
The robot control modules require an Arduino microcontroller such as the Arduino Nano, Uno, Raspberry PI Pico and others. The code is designed to work with the NRF24L01 RF transceiver. These two components are extremely inexpensive and can be purchased on Amazon.
The robot chasis can be anything but for best results I've found using an RC car with a differential drive system (left, right motor control) works the best.  

## Code Configuration
The pinouts must be setup for each robot type. This does require a little digging into the code to understand how things are setup.  

## Compilation
To use the code you much have Arduino Sketch installed with the NRF24l01 library.  
