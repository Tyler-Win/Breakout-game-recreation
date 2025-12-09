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

typedef struct {
    int16_t x_pos;
    int16_t y_pos;
    int16_t width;
    int16_t height;
    int16_t x_old;
    bool first_draw_done;
} Paddle;

typedef struct {
    float x;
    float y;
    float x_old;
    float y_old;
    float vel_x;
    float vel_y;
    uint8_t radius;
} Ball;

typedef struct {
    int16_t x;
    int16_t y;
    bool alive;
} Brick;

typedef struct{
  Ball ball;
  Paddle paddle;
  Brick bricks[15];
} GameState;

void run_breakout(GameState *game);
void update_paddle(GameState *game);
void update_ball(GameState *game);
void update_bricks(GameState *game, int brick_index, color565_t color);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define PADDLE_WIDTH 40
#define PADDLE_HEIGHT 8
#define BALL_RADIUS 5
#define BRICK_WIDTH 20
#define BRICK_HEIGHT 10

#define ROW_1 5
#define ROW_2 20
#define ROW_3 35

#define COLUMN_1 5
#define COLUMN_2 30
#define COLUMN_3 55
#define COLUMN_4 80
#define COLUMN_5 105
#define COLUMN_6 130

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
  ST7735S_clear_lcd(WHITE);

  GameState game = {
      .paddle = {
          .x_pos = 40,
          .y_pos = PADDLE_SPAWN_Y,
          .width = PADDLE_WIDTH,
          .height = PADDLE_HEIGHT,
          .x_old = -1,
          .first_draw_done = false
      },
      .ball = {
          .x = BALL_SPAWN_X,
          .y = BALL_SPAWN_Y,
          .x_old = 0,
          .y_old = 0,
          .vel_x = 0.5f,
          .vel_y = -0.5f,
          .radius = BALL_RADIUS
      },
      .bricks = {

      }
  };

  // First row
  game.bricks[1].x = COLUMN_1;
  game.bricks[1].y = ROW_1;
  game.bricks[1].alive = true;

  game.bricks[2].x = COLUMN_2;
  game.bricks[2].y = ROW_1;
  game.bricks[2].alive = true;

  game.bricks[3].x = COLUMN_3;
  game.bricks[3].y = ROW_1;
  game.bricks[3].alive = true;

  game.bricks[4].x = COLUMN_4;
  game.bricks[4].y = ROW_1;
  game.bricks[4].alive = true;

  game.bricks[5].x = COLUMN_5;
  game.bricks[5].y = ROW_1;
  game.bricks[5].alive = true;  

  game.bricks[6].x = COLUMN_1;
  game.bricks[6].y = ROW_2;
  game.bricks[6].alive = true;  

  game.bricks[7].x = COLUMN_2;
  game.bricks[7].y = ROW_2;
  game.bricks[7].alive = true;  

  game.bricks[8].x = COLUMN_3;
  game.bricks[8].y = ROW_2;
  game.bricks[8].alive = true;    

  game.bricks[9].x = COLUMN_4;
  game.bricks[9].y = ROW_2;
  game.bricks[9].alive = true;  

  game.bricks[10].x = COLUMN_5;
  game.bricks[10].y = ROW_2;
  game.bricks[10].alive = true;

  game.bricks[11].x = COLUMN_1;
  game.bricks[11].y = ROW_3;
  game.bricks[11].alive = true;  

  game.bricks[12].x = COLUMN_2;
  game.bricks[12].y = ROW_3;
  game.bricks[12].alive = true;  

  game.bricks[13].x = COLUMN_3;
  game.bricks[13].y = ROW_3;
  game.bricks[13].alive = true;    

  game.bricks[14].x = COLUMN_4;
  game.bricks[14].y = ROW_3;
  game.bricks[14].alive = true;  

  game.bricks[15].x = COLUMN_5;
  game.bricks[15].y = ROW_3;
  game.bricks[15].alive = true;        
  

  while (1) {
    run_breakout(&game);
  }
}
/* main */


void run_breakout(GameState *game)
{
  update_paddle(game);
  update_ball(game);

  for (int i = 0; i <= 5; i++) {
    update_bricks(game, i, RED);
  }

  for (int i = 6; i <= 10; i++) {
    update_bricks(game, i, ORANGE);
  }

  for (int i = 11; i <= 15; i++) {
    update_bricks(game, i, BLUE);
  }
  msec_delay(8);
}


void update_paddle(GameState *game)
{
  Paddle *paddle = &game->paddle;

  // Local aliases for paddle
  int16_t *x_pos = &paddle->x_pos;
  int16_t *x_old = &paddle->x_old;
  int16_t y_pos = paddle->y_pos;
  int16_t width = paddle->width;
  int16_t height = paddle->height;
  bool *first_draw_done = &paddle->first_draw_done;

  int16_t fast_paddle_speed = TWO_PPF;
  int16_t slow_paddle_speed = ONE_PPF;
  int16_t joy_x = ADC0_in(X_AXIS);

  // Move paddle based on joystick input
  if (joy_x > FAR_RIGHT_JOYSTICK_VALUE) 
  {
    *x_pos += fast_paddle_speed;
  } 
  else if (joy_x > RIGHT_JOYSTICK_VALUE) 
  {
    *x_pos += slow_paddle_speed;
  } 
  else if (joy_x < FAR_LEFT_JOYSTICK_VALUE) 
  {      
    *x_pos -= fast_paddle_speed;
  } 
  else if (joy_x < LEFT_JOYSTICK_VALUE) 
  {
    *x_pos -= slow_paddle_speed;
  }

  // Clamp to screen boundaries
  if (*x_pos < 0)
  {
    *x_pos = 0;
  }

  if (*x_pos > SCREEN_WIDTH - 47)
  {
    *x_pos = SCREEN_WIDTH - 47;
  }

  // Draw first time
  if (!*first_draw_done) {
      draw_rectangle(*x_pos, y_pos, width, height, RED);
      *first_draw_done = true;
      *x_old = *x_pos;
  }

  // Only redraw if position changed
  if (*x_pos != *x_old) {
      if (*x_pos > *x_old) {
          // moving right: erase left, draw right
          draw_rectangle(*x_old, y_pos, height, width, WHITE);
          draw_rectangle(*x_pos + width - 1, y_pos, height, width, RED);
      } else {
          // moving left: erase right, draw left
          draw_rectangle(*x_old + width - 1, y_pos, height, width, WHITE);
          draw_rectangle(*x_pos, y_pos, height, width, RED);
      }
      *x_old = *x_pos;
  }
}

void update_ball(GameState *game)
{
  Ball *ball_ptr = &game->ball;
  Paddle *paddle_ptr = &game->paddle;

  // Local aliases for ball 
  float *x_pos = &ball_ptr->x;
  float *y_pos = &ball_ptr->y;
  float *x_old_pos = &ball_ptr->x_old;
  float *y_old_pos = &ball_ptr->y_old;
  float *vel_x = &ball_ptr->vel_x;  
  float *vel_y = &ball_ptr->vel_y;
  int8_t radius = ball_ptr->radius;

  // Local aliases for paddle
  int16_t paddle_x = paddle_ptr->x_pos;
  int16_t paddle_y = paddle_ptr->y_pos;
  int16_t paddle_width = paddle_ptr->width;
  int16_t paddle_height = paddle_ptr->height;
  

  draw_circle(*x_pos, *y_pos, radius, WHITE);

  *x_old_pos = *x_pos;
  *y_old_pos = *y_pos;

  // update ball position
  *x_pos += *vel_x;
  *y_pos += *vel_y;

  // Ball & Paddle collision check
  bool x_overlap = (*x_pos + radius >= paddle_x) &&
                   (*x_pos - radius <= paddle_x + paddle_width);
  
  bool y_overlap = (*y_pos + radius >= paddle_y) && (*y_old_pos < paddle_y);


  if (x_overlap && y_overlap)
  {
    *vel_y = -*vel_y;

    // change vel_x based on where ball hits paddle
    float hit_position = (*x_pos - (paddle_x + PADDLE_WIDTH/2));
    *vel_x += hit_position * 0.02;

    // Ball won't travel into paddle
    *y_pos = paddle_y - radius - 1;
  }      

  // if the ball's x/y goes outside the left or right walls
  // then invert vel_x, add to ball_x, and randomize the balls 
  // y speed
  if (*x_pos - radius <= 0 || *x_pos + radius >= SCREEN_WIDTH)
  {
      *vel_x = -*vel_x;
      *x_pos += *vel_x;
      *vel_y += ((rand() % 4) - 2) * 0.1;  // slight variation
  }

  // if the ball's x/y goes outside the top or bottom walls
  // then invert vel_y, add to ball_y, and randomize the balls 
  // x speed
  if (*y_pos - radius <= 0)
  {
      *vel_y = -*vel_y;
      *y_pos += *vel_y;
      *vel_x += ((rand() % 4) - 2) * 0.1;  // slight variation
  }

  // Hits floor. Game over
  if (*y_pos + radius >= SCREEN_HEIGHT)
  {
    *x_pos = BALL_SPAWN_X;
    *y_pos = BALL_SPAWN_Y;
    *vel_x = 0.5f;
    *vel_y = -0.5f;
    draw_circle(*x_pos, *y_pos, radius, WHITE);
  } else {
    // Draw ball at new position
    draw_circle(*x_pos, *y_pos, radius, PURPLE);     
  }          
}

void update_bricks(GameState *game, int brick_index, color565_t color)
{
  Ball *ball_ptr = &game->ball;
  Brick *brick_ptr = &game->bricks[brick_index];

  // Local aliases for ball
  float *ball_x = &ball_ptr->x;
  float *ball_y = &ball_ptr->y;
  float *ball_old_x = &ball_ptr->x_old;
  float *ball_old_y = &ball_ptr->y_old;
  float *vel_x = &ball_ptr->vel_x;  
  float *vel_y = &ball_ptr->vel_y;
  int8_t radius = ball_ptr->radius;

  int16_t brick_x = brick_ptr->x;
  int16_t brick_y = brick_ptr->y;
  bool *alive = &brick_ptr->alive;
  uint8_t brick_width = BRICK_WIDTH;
  uint8_t brick_height = BRICK_HEIGHT;

  if (*alive) {
    draw_rectangle(brick_x, brick_y, brick_width, brick_height, color);

    // Collision detection
    // top
    bool top_hit = (*ball_x + radius >= brick_x) &&
                    (*ball_x - radius <= brick_x + brick_width) &&
                    (*ball_old_y + radius <= brick_y) &&
                    (*ball_y + radius >= brick_y);

    // Bottom
    bool bottom_hit = (*ball_x + radius >= brick_x) &&
                      (*ball_x - radius <= brick_x + brick_width) &&
                      (*ball_old_y - radius >= brick_y + brick_height) &&
                      (*ball_y - radius <= brick_y + brick_height);

    // Left
    bool left_hit = (*ball_y + radius >= brick_y) &&
                    (*ball_y - radius <= brick_y + brick_height) &&
                    (*ball_old_x + radius <= brick_x) &&
                    (*ball_x + radius >= brick_x);

    // Right
    bool right_hit = (*ball_y + radius >= brick_y) &&
                      (*ball_y - radius <= brick_y + brick_height) &&
                      (*ball_old_x - radius >= brick_x + brick_width) &&
                      (*ball_x - radius <= brick_x + brick_width);

    if (top_hit || bottom_hit) {
      *vel_y = -*vel_y;
      *alive = false;
      draw_rectangle(brick_x, brick_y, brick_width, brick_height, WHITE);
    } else if (left_hit || right_hit){
      *vel_x = -*vel_x;
      *alive = false;
      draw_rectangle(brick_x, brick_y, brick_width, brick_height, WHITE);
    }    
  }
}
