//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Tyler Huynh
//
//       LAB NAME:  Final Project
//
//      FILE NAME: breakout_game.c
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
#include "breakout_game.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_breakout();
void paddle_object();
void ball_object();
void square_object(uint8_t x, uint8_t y, color565_t color);

void pushbutton_test();
uint8_t filter_adc_val(uint8_t raw_val);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define PADDLE_WIDTH 40
#define PADDLE_HEIGHT 8

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
int16_t paddle_x_global = 40;
float ball_x_global = 0;
float ball_y_global = 0;
float old_ball_x_global = 0;
float old_ball_y_global = 0;


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
  ST7735S_clear_lcd(WHITE);



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

  square_object(80, 40, RED);

  msec_delay(8);
}

// Creates a paddle that can be manipulated using
// the joystic
void paddle_object()
{
    static int16_t x_pos = PADDLE_SPAWN_X;   // initial paddle position
    static int16_t x_old = PADDLE_OLD_X;     // old value that gets compared

    int16_t y_pos = PADDLE_SPAWN_Y;                     // y-value for the platform
    int16_t fast_paddle_speed = TWO_PPF;           // pixels per frame
    int16_t slow_paddle_speed = ONE_PPF;           // pixels per frame

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
    if (x_pos > SCREEN_WIDTH - PADDLE_WIDTH){
      x_pos = SCREEN_WIDTH - PADDLE_WIDTH;
    }

    // Prints out first rectangle
    if (x_old == x_pos){
      static bool first_draw_done = false;
      if (!first_draw_done) {
        draw_rectangle(x_pos, y_pos, PADDLE_WIDTH, PADDLE_HEIGHT, 
                       RED);
        first_draw_done = true;               
      }
    }

    // Only redraw if position changed
    if (x_pos != x_old)
    {
        // Shift right
        if (x_pos > x_old){

          // erase left column
          draw_rectangle(x_old, y_pos, PADDLE_HEIGHT, PADDLE_WIDTH,
                        WHITE);

          // draw right column  
          draw_rectangle(x_old + PADDLE_WIDTH - ONE_WIDTH, y_pos, 
                        PADDLE_HEIGHT, PADDLE_WIDTH, RED);              
        }
        else // Shift left
        {
          
          // erase right column
          draw_rectangle(x_old + PADDLE_WIDTH - ONE_WIDTH, y_pos, 
                        PADDLE_HEIGHT, PADDLE_WIDTH, WHITE);

          // draw left column  
          draw_rectangle(x_pos, y_pos, PADDLE_HEIGHT, PADDLE_WIDTH, RED);  
        }

        x_old = x_pos;
        paddle_x_global = x_pos;
    }
}


void ball_object()
{
    static float ball_x = 0;          // balls' x coord
    static float ball_y = 0;          // ball's y coord
    static float vel_x = -0.5;        // speed in the x direction
    static float vel_y = -0.5;        // speed in the y direction
    static float old_x = 0;
    static float old_y = 0;        
    static bool initialized = false;       
    uint8_t radius = 5;
    int16_t paddle_y = 120;

    int16_t paddle_x = paddle_x_global;    

    // ensures balls spawns in same area each time
    if (!initialized)
    {
        ball_x = BALL_SPAWN_X;
        ball_y = BALL_SPAWN_Y;
        old_x = ball_x;
        old_y = ball_y;

        vel_x = -0.5;  // initial launch direction
        vel_y = -0.5;
        initialized = true;
    }

    // Erase previous ball
    draw_circle(ball_x, ball_y, radius, RGB_to_color(255, 255, 255));

    old_x = ball_x;
    old_y = ball_y;

    // Update ball position
    ball_x += vel_x;
    ball_y += vel_y;

    // Update ball globally for squares
    ball_x_global = ball_x;
    ball_y_global = ball_y;

    old_ball_x_global = old_x;
    old_ball_y_global = old_y;

    // Ball paddle collision check
    bool x_overlap = (ball_x + radius >= paddle_x) && 
                     (ball_x - radius <= paddle_x + PADDLE_WIDTH);

    bool y_overlap = (ball_y + radius >= paddle_y) && (old_y < paddle_y);


    if (x_overlap && y_overlap)
    {
      vel_y = -vel_y;

      // change vel_x based on where ball hits paddle
      float hit_position = (ball_x - (paddle_x + PADDLE_WIDTH/2));
      vel_x += hit_position * 0.02;

      // Ball doesn't fall into
      ball_y = paddle_y - radius - 1;
    }      

    // if the ball's x/y goes outside the left or right walls
    // then invert vel_x, add to ball_x, and randomize the balls 
    // y speed
    if (ball_x - radius <= 0 || ball_x + radius >= SCREEN_WIDTH)
    {
        vel_x = -vel_x;
        ball_x += vel_x;
        vel_y += ((rand() % 4) - 2) * 0.1;  // slight variation
    }

    // if the ball's x/y goes outside the top or bottom walls
    // then invert vel_y, add to ball_y, and randomize the balls 
    // x speed
    if (ball_y - radius <= 0)
    {
        vel_y = -vel_y;
        ball_y += vel_y;
        vel_x += ((rand() % 4) - 2) * 0.1;  // slight variation
    }

    // Hits floor. Game over
    if (ball_y + radius >= SCREEN_HEIGHT)
    {
      vel_y = 0;
      vel_x = 0;
      msec_delay(5);
      initialized = false;
      draw_circle(ball_x, ball_y, radius, WHITE);
    } else {
      // Draw ball at new position
      draw_circle(ball_x, ball_y, radius, PURPLE);     
    }          
    
}

void square_object(uint8_t square_x, uint8_t square_y, color565_t color)
{
  uint8_t square_width = 20;
  uint8_t square_height = 20;
  static bool terminated = false;

  // Ball's X, Y, and radius
  int8_t ball_x = ball_x_global;
  int8_t ball_y = ball_y_global;
  int8_t old_x = old_ball_x_global;
  int8_t old_y = old_ball_y_global;
  int8_t radius = 5;

  // draw square
  if (!terminated){
      draw_rectangle(square_x, square_y, square_width, square_height, color);
  }

  // Ball square collision check

  // If ball hits from the top of the rectangle
  bool top_x_overlap = (ball_x + radius >= square_x) && 
                       (ball_x - radius <= square_x + square_width);

  bool top_y_overlap = (ball_y + radius >= square_y) && (old_y < square_height);


  // If ball hits from the left of the rectangle
  bool left_x_overlap = (ball_x + radius >= square_x) && (old_x < square_x);

  bool left_y_overlap = (ball_y + radius >= square_y) &&
                        (ball_y - radius <= square_y + square_height);


  // If ball hits from the right of the rectangle
  bool right_x_overlap = (ball_x - radius <= square_x + square_width) &&
                         (old_x > square_width);

  bool right_y_overlap = (ball_y + radius >= square_y) &&
                         (ball_y - radius <= square_y + square_height);


  // If ball hits from the bottom of the rectangle                                           
  bool bottom_x_overlap = (ball_x + radius >= square_x) &&
                          (ball_x - radius <= square_x + square_width);

  bool bottom_y_overlap = (ball_y + radius <= square_y) && 
                          (old_y > square_height);                         


  if (top_x_overlap && top_y_overlap)
  {
    draw_rectangle(square_x, square_y, square_width, square_height, WHITE);

    // Ball doesn't fall into square
    ball_y = square_y - radius - 1;    
    terminated = true;
  }   

  if (left_x_overlap && left_y_overlap)
  {
    draw_rectangle(square_x, square_y, square_width, square_height, WHITE);

    // Ball doesn't fall into square
    ball_x = square_x - radius - 1;    
    terminated = true;
  }

  if (right_x_overlap && right_y_overlap)
  {
    draw_rectangle(square_x, square_y, square_width, square_height, WHITE);

    // Ball doesn't fall into square
    ball_x = square_x + radius - 1;    
    terminated = true;
  }

  if (bottom_x_overlap && bottom_y_overlap)
  {
    draw_rectangle(square_x, square_y, square_width, square_height, WHITE);

    // Ball doesn't fall into square
    ball_y = square_x + radius - 1;    
    terminated = true;
  }


}









