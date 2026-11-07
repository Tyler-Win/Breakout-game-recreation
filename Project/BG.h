// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Tyler Huynh
//
//         VERSION:  0.1
//
//       FILE NAME:  BG.h
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This module serves to provide definitions that initialize and run a 
//    breakout game. This file also contains various #defines that can be 
//    located in breakout_game_main.c that serve to allow the program to 
//    function as originally intended.
//    
//
//-----------------------------------------------------------------------------
// DISCLAIMER
//    This code was developed for educational purposes as part of the CSC202
//    course and is provided "as is" without warranties of any kind, whether
//    express, implied, or statutory.
//
//    The author and organization do not warrant the accuracy, completeness, or
//    reliability of the code. The author and organization shall not be liable
//    for any direct, indirect, incidental, special, exemplary, or consequential
//    damages arising out of the use of or inability to use the code, even if
//    advised of the possibility of such damages.
//
//    Use of this code is at your own risk, and it is recommended to validate
//    and adapt the code for your specific application and hardware requirements.
//
// Copyright (c) 2024 by TBD
//    You may use, edit, run or distribute this file as long as the above
//    copyright notice remains
// *****************************************************************************
//******************************************************************************

#ifndef __BG_H__
#define __BG_H__

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------

// Game loop
#define BUZZER_DELAY                                                          75
#define PADDLE_BUZZ_DELAY                                                     25

// Joystick values
#define FAR_RIGHT_JOYSTICK_VALUE                                            2800
#define RIGHT_JOYSTICK_VALUE                                                2050
#define LEFT_JOYSTICK_VALUE                                                 1700
#define FAR_LEFT_JOYSTICK_VALUE                                             1100
#define X_AXIS                                                                 2

// LCD screen values
#define SCREEN_WIDTH                                                         128
#define SCREEN_HEIGHT                                                        128
#define OFF_BOUNDS_X                                                        -100
#define OFF_BOUNDS_Y                                                        -100

// Game functionality values
#define LIVES                                                                  3

// # Define to set up paddle object
#define PADDLE_WIDTH                                                          40
#define PADDLE_HEIGHT                                                          8
#define PADDLE_SPAWN_X                                                        40
#define PADDLE_SPAWN_Y                                                       120
#define PADDLE_OLD_X                                                          40
#define BUFFER                                                                47
#define ONE_WIDTH                                                              1
#define ONE_PPF                                                                1
#define TWO_PPF                                                                2
#define SCALING                                                             0.02

// # Define to set up Ball object
#define BALL_SPAWN_X                                          (SCREEN_WIDTH / 2)
#define BALL_SPAWN_Y                                                   (110 - 6)
#define BALL_OLD_X                                                             0
#define BALL_OLD_Y                                                             0
#define BALL_VEL_X                                                          0.5f
#define BALL_VEL_Y                                                         -0.5f
#define BALL_RADIUS                                                            5

// # Define to properly set up Brick object
#define BRICK_WIDTH                                                         29.5
#define BRICK_HEIGHT                                                          10
#define BRICK_SPACING_X                                                        2
#define BRICK_SPACING_Y                                                        2 
#define BRICK_START_X                                                          2
#define BRICK_START_Y                                                          2
#define NUM_ROWS                                                               4
#define NUM_COLS                                                               4

// # Define for colors
#define RED                                              RGB_to_color(255, 0, 0)
#define GREEN                                            RGB_to_color(0, 255, 0)
#define BLUE                                             RGB_to_color(0, 0, 255)
#define WHITE                                        RGB_to_color(255, 255, 255)
#define PURPLE                                         RGB_to_color(255, 0, 255)
#define ORANGE                                         RGB_to_color(255, 165, 0)
#define OFF                                               RGB_to_color(0, 0, 0,)

// # Define for pause position, text
#define PAUSE_TEXT                                                      "PAUSED"
#define FIRST_HALF_GAME_START_TEXT                                   "PRESS PB2"
#define SECOND_HALF_GAME_START_TEXT                                   "TO START"
#define WIN_TEXT                                                      "YOU WIN!"
#define PAUSE_X_POS                                                           40
#define PAUSE_Y_POS                                                           80
#define WIN_X_POS                                                             30
#define WIN_Y_POS                                                             70
#define FIRST_HALF_START_TEXT_X_POS                                           25
#define FIRST_HALF_START_TEXT_Y_POS                                           60
#define SECOND_HALF_START_TEXT_X_POS                                          30
#define SECOND_HALF_START_TEXT_Y_POS                                          70
#define SPACING                                                                8

#endif /* __SPI_H__ */
