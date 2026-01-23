********************************************************************************
//                           BREAKOUT GAME RECREATION
********************************************************************************
//
// CSC 202 Final Project by Tyler Huynh and Adnan Heganovic
//
// This program serves as a recreation of the popular Atari game breakout
// designed to run on the LP-MSPM0G3507 LaunchPad with the use of the
// BOOSTXL-EDUMKII. Utilizes the BOOSTXL-EDUMKII SDK developed by Ryanne 
// Geiger (More Information located below).
// 
// The objective of Breakout is to aim and hit the ball at the bricks
// located at the top of the screen using your paddle located at the bottom
// and remove all of them before losing all 3 lives. 
//
// This game serves as an extension to Ryanne's project and opens the doors
// for future game development on the LP-MSPM0G3507 for future projects.
//
// Many thanks to Prof Bruce Link, TA's, and fellow students during this project.
//
//
********************************************************************************

// How to play:
//      - Press PB1 to start the game. PB2 is to pause the game and PB1 is
//        to resume progress
//      - Move the paddle at the bottom of the screen left or right using
//        the joystick located on the BOOSTXL-EDUMKII SDK
//      - Aim the ball towards the bricks while also managing to prevent
//        the ball from hitting the bottom of the screen
//      - You have 3 lives, represented by the current LED color: Green is for
//        3, yellow is for 2, and red is for 1
//      - If you lose all three lives, press the reset on the MSPM0G3507 to
//        restart the game

********************************************************************************
//                           SDK BOOSTXL-EDUMKII
********************************************************************************
//
// CSC 202 Final Project by Ryanne Geiger
//
// The goal of this project was to create a SDK for the BOOSTXL-EDUMKII 
// BoosterPack for use with the LP-MSPM0G3507 LaunchPad. The SDK will allow
// future students to save time by providing code samples and helper functions.
//  
// Please note that this is a work in progress. 
//
// Many thanks to Andrew Deford for his assistance in creating the LCD code 
// and Prof Bruce Link for the guidance during this project.
//
********************************************************************************

// Accelerometer
  3-Axis 
    x-axis Uses ADC1, channel 6
    y-axis Uses ADC0, channel 7 
    z-axis Uses ADC1, channel 5


// Microphone
  Uses ADC0, channel 5


// JoyStick
  x-axis: Uses ADC0, channel 2
  y-axis: Uses ADC1, channel 1
  Select: See push buttons 


// Push Buttons
 Notes: Jumper 18 needs to be removed
        Jumper 22 needs to be moved down 1
      This allows use of the push buttons on the boosterpack but user is 
       no longer able to use UART functions. 

  Button 1                   22 GPIO PA11 
  Button 2                   34 GPIO PA12 
  JoyStick Select            59 GPIO PA26 


// LCD Screen
  Andrew Deford created the raw code for use of color LCD screen 
   - ST7735S sets up screen 
   - gfx allows user to create a rectangle


// BUZZER
  Andrew Deford helped edit the code that enables and inits 
   - Pitches and Songs are OpenSource on GitHub 
        - Potter, Mario, SmallWorld
     

// Timers - PWM - BUZZER - RGBs
  BUZZER       PB4      TIMA0_C2
  RGB RED      PB1      TIMA1_C1
  RGB GREEN    PA28     TIMG7_C0
  RGB BLUE     PA31     TIMG7_C1


// Light Sensor
   Note: To Be Developed 
  14 15 PB2 UART3_TX UART2_CTS I2C1_SCL TIMA0_C3 UART1_CTS TIMG6_C0 TIMA1_C0


// Temp Sensor
   Note: To Be Developed
  32 33 PB16 UART2_RX SPI1_SCK UART3_RTS TIMG8_C1 TIMG7_C1
  

// Servo Motor Connector
   Note: To Be Developed
  28 29 PB12 UART3_TX TIMA0_C2 TIMA_FAL1 TIMA0_C1


// UART Serial Console
   Note: To Be Developed


