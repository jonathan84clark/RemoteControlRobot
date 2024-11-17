/*******************************************************
* CONFIG
* DESC: The robot config file defines the pinouts and hardware
* configuration for each robot type.
* Author: Jonathan L Clark
* Date: 10/27/2020
*******************************************************/
#ifndef _CONFIG_
#define _CONFIG_

// Robot Configuration Types
#define ROBOT_TRIPLE  0
#define ROBOT_TANK    1
#define ROBOT_FASTROV 2
#define SAND_RUNNDER  3
#define MINI_BOT      4
#define MEDIUM_BOT    5
#define BOT_V2        6
#define SUPER_POWERS_A4 7

#define ROBOT_CONFIG MINI_BOT 

/***********TRIPLE SENSOR CONFIG************/
#if ROBOT_CONFIG == ROBOT_TRIPLE
   // Motor Pinouts
   #define LEFT_A 5
   #define LEFT_B 3

   #define RIGHT_A 6
   #define RIGHT_B 7

   // Accessory Pinouts
   #define PULSE_PIN 8
   #define WHITE_HEADLIGHTS 4
   #define THROTTLE_SCALE 1.0

   // Radio Pinouts
   #define RADIO_CE 10
   #define RADIO_CSN 9

   // Sensor Configuration
   #define NUM_SENSORS 3
   #define SENSOR_TRIGGER_L A4
   #define SENSOR_TRIGGER_M A3
   #define SENSOR_TRIGGER_R A2

   #define SENSOR_ENABLE_L A7
   #define SENSOR_ENABLE_M A6
   #define SENSOR_ENABLE_R A5
   #define SENSOR_ECHO 2

/***********ROBOT TANK (TRACKED) CONFIG***************/
#elif ROBOT_CONFIG == ROBOT_TANK
   // Motor Pinouts
   #define LEFT_A 8
   #define LEFT_B 6

   #define RIGHT_A 3
   #define RIGHT_B 5

   // Accessory Pinouts
   #define PULSE_PIN 7
   #define WHITE_HEADLIGHTS A1
   #define AUX_HEADLIGHTS A2
   #define THROTTLE_SCALE 1.0
   #define BUZZER 4

   // Radio Pinouts
   #define RADIO_CE 9
   #define RADIO_CSN 10

/***********ROBOT TANK (TRACKED) CONFIG***************/
#elif ROBOT_CONFIG == SAND_RUNNDER
   // Motor Pinouts
   #define STEER_A 5
   #define STEER_B 3

   #define MAIN_A 6
   #define MAIN_B 7

   // Accessory Pinouts
   #define PULSE_PIN 8
   #define WHITE_HEADLIGHTS A3
   #define THROTTLE_SCALE 1.0

   // Radio Pinouts
   #define RADIO_CE 10
   #define RADIO_CSN 9

/***********************************/
#elif ROBOT_CONFIG == MINI_BOT
   // Motor Pinouts
   #define LEFT_A 6
   #define LEFT_B 9

   #define RIGHT_A 3
   #define RIGHT_B 5

   // Accessory Pinouts
   #define PULSE_PIN A3
   #define WHITE_HEADLIGHTS A0
   #define THROTTLE_SCALE 0.80

   // Radio Pinouts
   #define RADIO_CE 8
   #define RADIO_CSN 10
/***********************************/
#elif ROBOT_CONFIG == MEDIUM_BOT
   // Motor Pinouts
   #define LEFT_A 3
   #define LEFT_B 5

   #define RIGHT_A 6
   #define RIGHT_B 9

   // Accessory Pinouts
   #define PULSE_PIN A3
   #define WHITE_HEADLIGHTS A0
   #define THROTTLE_SCALE 0.80

   // Radio Pinouts
   #define RADIO_CE 8
   #define RADIO_CSN 10
/***********************************/
#elif ROBOT_CONFIG == BOT_V2
   // Motor Pinouts
   #define STEER_A 3
   #define STEER_B 5

   #define MAIN_A 9
   #define MAIN_B 6

   // Accessory Pinouts
   #define PULSE_PIN 7
   #define THROTTLE_SCALE 1.0

   // Radio Pinouts
   #define RADIO_CE 8
   #define RADIO_CSN 10

   #define DEBUG_LED A0
   #define IR_LEFT A1
   #define IR_MID A2
   #define IR_RIGHT A3

#elif ROBOT_CONFIG == SUPER_POWERS_A4
   // Motor Pinouts
   #define STEER_A 9
   #define STEER_B 6

   #define MAIN_A 5 
   #define MAIN_B 3

   // Accessory Pinouts
   #define THROTTLE_SCALE 1.0

   // Radio Pinouts
   #define RADIO_CE 8
   #define RADIO_CSN 7

   #define DEBUG_LED 2
   
/***********************************/
#else
   // Motor Pinouts
   #define LEFT_A 6
   #define LEFT_B 5

   #define RIGHT_A 3
   #define RIGHT_B 9

   // Accessory Pinouts
   #define PULSE_PIN 7
   #define WHITE_HEADLIGHTS A0
   #define AUX_HEADLIGHTS A1
   #define THROTTLE_SCALE 0.80

   // Radio Pinouts
   #define RADIO_CE 8
   #define RADIO_CSN 10

#endif

#endif /* CONFIG */
