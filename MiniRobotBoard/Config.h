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
#define ROBOT_DIFF_CAR  0

#define ROBOT_CONFIG ROBOT_DIFF_CAR 

/***********TRIPLE SENSOR CONFIG************/
#if ROBOT_CONFIG == ROBOT_TRIPLE
   // Motor Pinouts
   #define LEFT_A 5
   #define LEFT_B 3

   #define RIGHT_A 6
   #define RIGHT_B 7

   // Accessory Pinouts
   #define PULSE_PIN A0
   #define THROTTLE_SCALE 1.0

   // Radio Pinouts
   #define RADIO_CE 7
   #define RADIO_CSN 8

#endif

#endif /* CONFIG */
