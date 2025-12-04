//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ryanne Geiger
//
//       LAB NAME:  Final Project
//
//      FILE NAME: booster_test_main.c
//
// NOTE: Autoformatted with CTRL+K+F
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a means to test the LP-MSPM0G3507 LaunchPad
//     and the BOOSTXL-EDUMKII buzzer, push buttons, joystick, and RGBs
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include <ti/devices/msp/peripherals/hw_iomux.h>
#include "LaunchPad.h"
#include "clock.h"
#include "uart.h"
#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "ti/devices/msp/peripherals/hw_gptimer.h"
#include "booster.h"
#include "adc.h"
#include "pitches.h"
#include "gfx.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_breakout();
void paddle_object();
void ball_object();

void pushbutton_test();
uint8_t filter_adc_val(uint8_t raw_val);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------

#define FAR_RIGHT_JOYSTICK_VALUE                              2800
#define RIGHT_JOYSTICK_VALUE                                  2050
#define LEFT_JOYSTICK_VALUE                                   1700
#define FAR_LEFT_JOYSTICK_VALUE                               1100
#define HIGH_COLOR_TONE                                       100
#define LOW_COLOR_TONE                                        50
#define BRIGHTNESS                                            50
#define Y_AXIS                                                3
#define X_AXIS                                                2
#define PB_JOYSTICK                                           2
#define PB_2                                                  1
#define PB_1                                                  0
#define NO_COLOR_TONE                                         0
#define DEADZONE                                              4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define PADDLE_WIDTH 40
#define PADDLE_HEIGHT 8

#define BALL_SPAWN_X (SCREEN_WIDTH / 2)
#define BALL_SPAWN_Y (110 - 6)

#define JOY_MIN 0
#define JOY_MAX 250

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------


int main(void) {
  // Initialize 
  clock_init_40mhz();
  launchpad_gpio_init();
  UART_init(115200); 

  // ADC0 and ADC1
  enable_joystick_and_tilt();

  // Push Buttons
  boost_pb_init();

  // RGBS
  enable_rgb_timers();

  ST7735S_init();
  ST7735S_clear_lcd(RGB_to_color(255, 255, 255));



  // Uncomment song name to test with RGBs
  //potter();
  // smallworld();
  // mario();

  // Endless loop to prevent program from ending

  while (1) {
    run_breakout();
  }
}
/* main */

void run_breakout()
{
  paddle_object();
  ball_object();
  msec_delay(8);
}

// Creates a paddle that can be manipulated using
// the joystic
void paddle_object()
{
    static int16_t x_pos = 40;               // initial paddle position
    static int16_t x_old = 40;                // old value that gets compared

    int16_t y_pos = 110;                     // y-value for the platform
    int16_t fast_paddle_speed = 2;           // pixels per frame
    int16_t slow_paddle_speed = 1;           // pixels per frame

    int16_t joy_x = ADC0_in(X_AXIS);    // read X-axis joystick

    // Move right quickly if joystick pushed far
    if (joy_x > FAR_RIGHT_JOYSTICK_VALUE) {
        x_pos += fast_paddle_speed;
    }
    // Move right slowly if pushed lightly
    else if (joy_x > RIGHT_JOYSTICK_VALUE) {
        x_pos += slow_paddle_speed;
    }
    // Move left slowly if joystick pushed lightly
    else if (joy_x < FAR_LEFT_JOYSTICK_VALUE){
      x_pos -= fast_paddle_speed;
    }
    // Move left quickly if joystick pushed far
    else if (joy_x < LEFT_JOYSTICK_VALUE){
      x_pos -= slow_paddle_speed;
    }
    // If in the middle, do nothing (neutral)
    else {
        // x_pos stays the same
    }

    // Clamps paddle to screen boundaries
    if (x_pos < 0){
      x_pos = 0;
    }

    // Ensures the right end of paddle hits right 
    // end of paddle screen
    if (x_pos > SCREEN_WIDTH - 45){
      x_pos = SCREEN_WIDTH - 45;
    }

    // Prints out first rectangle
    if (x_old == x_pos){
      static int first_draw_done = 0;
      if (!first_draw_done) {
        draw_rectangle(x_pos, y_pos, PADDLE_WIDTH, PADDLE_HEIGHT, 
                       RGB_to_color(255, 0, 0));
        first_draw_done = 1;               
      }
    }

    // Only redraw if position changed
    if (x_pos != x_old)
    {
        // Shift right
        if (x_pos > x_old){

          // erase left column
          draw_rectangle(x_old, y_pos, PADDLE_HEIGHT, PADDLE_WIDTH,
                        RGB_to_color(255, 255, 255));

          // draw right column  
          draw_rectangle(x_old + PADDLE_WIDTH - 1, y_pos, PADDLE_HEIGHT, PADDLE_WIDTH, 
              RGB_to_color(255, 0, 0));              
        }
        else // Shift left
        {

          // erase right column
          draw_rectangle(x_old + PADDLE_WIDTH - 1, y_pos, PADDLE_HEIGHT, PADDLE_WIDTH,
                        RGB_to_color(255, 255, 255));

          // draw left column  
          draw_rectangle(x_pos, y_pos, PADDLE_HEIGHT, PADDLE_WIDTH,
              RGB_to_color(255, 0, 0));  
        }

        x_old = x_pos;
    }
}


void ball_object()
{
    static float ball_x = BALL_SPAWN_X;
    static float ball_y = BALL_SPAWN_Y;
    static float old_x = BALL_SPAWN_X;
    static float old_y = BALL_SPAWN_Y;    
    static float vel_x = -0.5;
    static float vel_y = -0.5;
    static bool initialized = false;

    uint8_t radius = 5;

    if (!initialized)
    {
        ball_x = BALL_SPAWN_X;
        ball_y = BALL_SPAWN_Y;
        old_x = ball_x;
        old_y = ball_y;

        vel_x = -0.5;
        vel_y = -0.5;
        initialized = true;
    }

    // --------------------------------------------
    // ERASE ENTIRE OLD BALL — CLEAN METHOD
    // --------------------------------------------
    draw_circle(old_x, old_y, radius, RGB_to_color(255,255,255));

    // --------------------------------------------
    // UPDATE POSITION
    // --------------------------------------------
    ball_x += vel_x;
    ball_y += vel_y;

    // --------------------------------------------
    // SCREEN BOUNCING
    // --------------------------------------------
    if (ball_x - radius <= 0 || ball_x + radius >= SCREEN_WIDTH)
        vel_x = -vel_x;

    if (ball_y - radius <= 0 || ball_y + radius >= 128) // SCREEN_HEIGHT = 128?
        vel_y = -vel_y;

    // --------------------------------------------
    // DRAW NEW BALL
    // --------------------------------------------
    draw_circle(ball_x, ball_y, radius, RGB_to_color(255,0,0));

    // Save position
    old_x = ball_x;
    old_y = ball_y;
}








