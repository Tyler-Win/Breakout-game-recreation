// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//    DESIGNER NAME:  Tyler Huynh
//
//          VERSION:  0.1
//
//        FILE NAME:  BG.c
//
// NOTE: Autoformatted with CTRL+K+F
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//     This file contains a collection of functions for initializing and
//     configuring various hardware peripherals on the LP-MSPM0G3507 LaunchPad
//     and the BOOSTXL-EDUMKII including:
//       - RGB Management
//       - PWM Motor Management
//       - Buzzer Management
//       - Joystick Reading
//       - Push Button Reading
//       - Accelerometer Reading
//       - Microphone Reading
//
//     This code is adapted from various Texas Instruments' LaunchPad
//     project template for the LP-MSPM0G3507, using C language and no RTOS.
//
//-----------------------------------------------------------------------------
// DISCLAIMER
//     This code was developed for educational purposes as part of the CSC202
//     course and is provided "as is" without warranties of any kind, whether
//     express, implied, or statutory.
//
//     The author and organization do not warrant the accuracy, completeness, or
//     reliability of the code. The author and organization shall not be liable
//     for any direct, indirect, incidental, special, exemplary, or consequential
//     damages arising out of the use of or inability to use the code, even if
//     advised of the possibility of such damages.
//
//     Use of this code is at your own risk, and it is recommended to validate
//     and adapt the code for your specific application and hardware requirements.
//
// Copyright (c) 2024 by TBD
//     You may use, edit, run or distribute this file as long as the above
//     copyright notice remains
// *****************************************************************************
//******************************************************************************
//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>
//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include <ti/devices/msp/peripherals/hw_iomux.h>
#include "LaunchPad.h"
#include "clock.h"
#include "adc.h"
#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "ti/devices/msp/peripherals/hw_gptimer.h"
#include "booster.h"
#include "BG.h"


//-----------------------------------------------------------------------------
// Define global variable and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

// Define a structure to hold Paddle data
typedef struct {
    int16_t x_pos;
    int16_t y_pos;
    int16_t width;
    int16_t height;
    int16_t x_old;
    bool first_draw_done;
} Paddle;

// Define a structure to hold Ball data
typedef struct {
    float x;
    float y;
    float x_old;
    float y_old;
    float vel_x;
    float vel_y;
    int8_t radius;
} Ball;

// Define a structure to hold Brick data
typedef struct {
    int16_t x;
    int16_t y;
    bool alive;
    bool drawn;
} Brick;

// Define a structure to hold the state of Breakout
typedef struct{
  Ball ball;
  Paddle paddle;
  Brick bricks[16];
  int8_t lives;
} GameState;

