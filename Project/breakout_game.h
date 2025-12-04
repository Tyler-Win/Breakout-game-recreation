// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Tyler Huynh
//
//         VERSION:  0.1
//
//       FILE NAME:  breakout_game.h
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    
//
//-----------------------------------------------------------------------------
// DISCLAIMER
//    
// *****************************************************************************
//******************************************************************************


#ifndef __BREAKOUT_GAME_H__
#define __BREAKOUT_GAME_H__

// Joystick values
#define FAR_RIGHT_JOYSTICK_VALUE                                            2800
#define RIGHT_JOYSTICK_VALUE                                                2050
#define LEFT_JOYSTICK_VALUE                                                 1700
#define FAR_LEFT_JOYSTICK_VALUE                                             1100
#define X_AXIS                                                                 2

// LCD values
#define SCREEN_WIDTH                                                         128
#define SCREEN_HEIGHT                                                        128

// Paddle object
#define PADDLE_WIDTH                                                          40
#define ONE_WIDTH                                                              1
#define PADDLE_HEIGHT                                                          8
#define PADDLE_SPAWN_X                                                        40
#define PADDLE_SPAWN_Y                                                       120
#define PADDLE_OLD_X                                                          40
#define ONE_PPF                                                                1
#define TWO_PPF                                                                2

// Ball object
#define BALL_SPAWN_X                                     (SCREEN_WIDTH / 2)
#define BALL_SPAWN_Y                                                   (110 - 6)

// Colors
#define WHITE                                        RGB_to_color(255, 255, 255)
#define RED                                              RGB_to_color(255, 0, 0)
#define GREEN                                            RGB_to_color(0, 255, 0)
#define BLUE                                             RGB_to_color(0, 0, 255)
#define PURPLE                                         RGB_to_color(255, 0, 255)
#define YELLOW                                         RGB_to_color(255, 255, 0)
#define CYAN                                           RGB_to_color(0, 255, 255)
#define ORANGE                                         RGB_to_color(255, 69, 0)

#endif /* __SPI_H__ */
