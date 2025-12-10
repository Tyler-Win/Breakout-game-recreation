//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Tyler Huynh, Adnan Heganovic
//
//       LAB NAME:  Final Project
//
//      FILE NAME: breakout_game.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This file serves as a recreation of the popular Atari game breakout
//    designed to run on the LP-MSPM0G3507 LaunchPad with the use of the
//    BOOSTXL-EDUMKII. Utilizes the BOOSTXL-EDUMKII SDK developed by Ryanne 
//    Geiger.
// 
//    The objective of Breakout is to aim and hit the ball at the bricks
//    located at the top of the screen using your paddle located at the bottom
//    and remove all of them before losing all 3 lives. 
//
//    How to play:
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
//
//    
//
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
    int8_t radius;
} Ball;

typedef struct {
    int16_t x;
    int16_t y;
    bool alive;
    bool drawn;
} Brick;

typedef struct{
  Ball ball;
  Paddle paddle;
  Brick bricks[16];
  int8_t lives;
} GameState;

void run_breakout(GameState *game);
void update_paddle(GameState *game);
void update_ball(GameState *game);
void update_bricks(GameState *game, int8_t brick_index, color565_t color);
void update_lives_led(uint8_t lives);
void play_brick_hit_sound();
void play_life_lost_sound();
void play_paddle_hit_sound();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define PADDLE_WIDTH 40
#define PADDLE_HEIGHT 8
#define BALL_RADIUS 5
#define BRICK_WIDTH 29.5
#define BRICK_HEIGHT 10
#define BUFFER 47

#define ROW_1 2
#define ROW_2 14
#define ROW_3 26

#define COLUMN_1 2
#define COLUMN_2 34
#define COLUMN_3 66
#define COLUMN_4 98

#define NUM_ROWS 4
#define NUM_COLS 4
#define BRICK_SPACING_X 2
#define BRICK_SPACING_Y 2
#define BRICK_START_X 2
#define BRICK_START_Y 2

#define HIGH_COLOR_TONE 100
#define MID_COLOR_TONE 50
#define LOW_COLOR_TONE 5
#define NO_COLOR_TONE 0

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
  init_booster_leds();
  enable_rgb_timers();

  // Buzzer
  motor0_init();
  buzzer_init();  

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
      .lives = 3
  };

  int8_t brick_index = 0;

  // Sets each individual brick
  for (int8_t row = 0; row < NUM_ROWS; row++) {
    for (int8_t col = 0; col < NUM_COLS; col++) {
      game.bricks[brick_index].x = BRICK_START_X + 
                                   col * (BRICK_WIDTH + BRICK_SPACING_X);
      game.bricks[brick_index].y = BRICK_START_Y + 
                                   row * (BRICK_HEIGHT + BRICK_SPACING_Y);
      game.bricks[brick_index].alive = true;
      game.bricks[brick_index].drawn = false;
      brick_index++;
    }
  }
  while (1) {
    run_breakout(&game);
  }
}
/* main */


void run_breakout(GameState *game)
{
  update_paddle(game);
  update_ball(game);

  // Adds color to bricks
  color565_t brick_colors[NUM_ROWS] = {RED, ORANGE, BLUE, GREEN};
  int8_t brick_index = 0;
  for (int8_t row = 0; row < NUM_ROWS; row++) {
      for (int8_t col = 0; col < NUM_COLS; col++) {
          update_bricks(game, brick_index, brick_colors[row]);
          brick_index++;
      }
  }
  update_lives_led(game->lives);

  msec_delay(8);
}


void update_paddle(GameState *game)
{
  Paddle *paddle_ptr = &game->paddle;

  // Local aliases for paddle
  int16_t y_pos = paddle_ptr->y_pos;
  int16_t width = paddle_ptr->width;
  int16_t height = paddle_ptr->height;

  int16_t fast_paddle_speed = TWO_PPF;
  int16_t slow_paddle_speed = ONE_PPF;
  int16_t joy_x = ADC0_in(X_AXIS);

  // Move paddle based on joystick input
  if (joy_x > FAR_RIGHT_JOYSTICK_VALUE) 
  {
    paddle_ptr->x_pos  += fast_paddle_speed;
  } 
  else if (joy_x > RIGHT_JOYSTICK_VALUE) 
  {
    paddle_ptr->x_pos  += slow_paddle_speed;
  } 
  else if (joy_x < FAR_LEFT_JOYSTICK_VALUE) 
  {      
    paddle_ptr->x_pos  -= fast_paddle_speed;
  } 
  else if (joy_x < LEFT_JOYSTICK_VALUE) 
  {
    paddle_ptr->x_pos  -= slow_paddle_speed;
  }

  // Clamp to screen boundaries
  if (paddle_ptr->x_pos < 0)
  {
    paddle_ptr->x_pos  = 0;
  }

  if (paddle_ptr->x_pos  > SCREEN_WIDTH - BUFFER)
  {
    paddle_ptr->x_pos  = SCREEN_WIDTH - BUFFER;
  }

  // Draw first time
  if (!paddle_ptr->first_draw_done) {
      draw_rectangle(paddle_ptr->x_pos , y_pos, width, height, RED);
      paddle_ptr->first_draw_done = true;
      paddle_ptr->x_old = paddle_ptr->x_pos ;
  }

  // Only redraw if position changed
  if (paddle_ptr->x_pos != paddle_ptr->x_old) {
      if (paddle_ptr->x_pos > paddle_ptr->x_old) {
          // moving right: erase left, draw right
          draw_rectangle(paddle_ptr->x_old, y_pos, height, width, WHITE);
          draw_rectangle(paddle_ptr->x_pos  + width - 1, y_pos, 
                        height, width, RED);
      } else {
          // moving left: erase right, draw left
          draw_rectangle(paddle_ptr->x_old + width - 1, y_pos, 
                        height, width, WHITE);
          draw_rectangle(paddle_ptr->x_pos , y_pos, height, width, RED);
      }
      paddle_ptr->x_old = paddle_ptr->x_pos ;
  }
}

void update_ball(GameState *game)
{
  Ball *ball_ptr = &game->ball;
  Paddle *paddle_ptr = &game->paddle;

  // Local aliases for ball 
  int8_t radius = ball_ptr->radius;

  // Local aliases for paddle
  int16_t paddle_x = paddle_ptr->x_pos;
  int16_t paddle_y = paddle_ptr->y_pos;
  int16_t paddle_width = paddle_ptr->width;
  int16_t paddle_height = paddle_ptr->height;
  
  // Erase old ball
  draw_circle(ball_ptr->x_old, ball_ptr->y_old, radius, WHITE);

  // Update old position
  ball_ptr->x_old = ball_ptr->x;
  ball_ptr->y_old = ball_ptr->y;

  // Move ball
  ball_ptr->x += ball_ptr->vel_x;
  ball_ptr->y += ball_ptr->vel_y;

  // Ball & Paddle collision check
  bool x_overlap = (ball_ptr->x + radius >= paddle_x) &&
                   (ball_ptr->x - radius <= paddle_x + paddle_width);
  
  bool y_overlap = (ball_ptr->y + radius >= paddle_y) && 
                   (ball_ptr->y - radius <= paddle_y + paddle_height);


  if (x_overlap && y_overlap)
  {
    ball_ptr->vel_y = -ball_ptr->vel_y;

    // change vel_x based on where ball hits paddle
    float hit_position = (ball_ptr->x - (paddle_x + PADDLE_WIDTH/2));

    ball_ptr->vel_x += hit_position * 0.02;

    // Ball won't travel into paddle
    ball_ptr->y = paddle_y - radius - 1;

    play_paddle_hit_sound();
  }      

  // if the ball's x/y goes outside the left or right walls
  // then invert vel_x, add to ball_x, and randomize the balls 
  // y speed
  if (ball_ptr->x - radius <= 0 || ball_ptr->x + radius >= SCREEN_WIDTH)
  {
      ball_ptr->vel_x = -ball_ptr->vel_x;
      ball_ptr->x += ball_ptr->vel_x;
      ball_ptr->vel_y += ((rand() % 4) - 2) * 0.1;  // slight variation
  }

  // if the ball's x/y goes outside the top or bottom walls
  // then invert vel_y, add to ball_y, and randomize the balls 
  // x speed
  if (ball_ptr->y - radius <= 0)
  {
      ball_ptr->vel_y = -ball_ptr->vel_y;
      ball_ptr->y += ball_ptr->vel_y;
      ball_ptr->vel_x += ((rand() % 4) - 2) * 0.1;  // slight variation
  }

  // Hits floor. Game over
  if (ball_ptr->y + radius >= SCREEN_HEIGHT)
  {
    if (game->lives > 0)
    {
      game->lives--;
    }
    update_lives_led(game->lives);
    play_life_lost_sound();

    if (game->lives > 0)
    {
      ball_ptr->x = BALL_SPAWN_X;
      ball_ptr->y = BALL_SPAWN_Y;
      ball_ptr->vel_x = 0.5f;
      ball_ptr->vel_y = -0.5f;
      draw_circle(ball_ptr->x_old, ball_ptr->y_old, radius, WHITE);
    } else {
      draw_circle(ball_ptr->x_old, ball_ptr->y_old, radius, WHITE);
      ball_ptr->x = -100;
      ball_ptr->y = -100;
      ball_ptr->vel_x = 0;
      ball_ptr->vel_y = 0;  
    }
  } else {
    // Draw ball at new position
    draw_circle(ball_ptr->x, ball_ptr->y, radius, PURPLE);     
  }          
}

// updates bricks
void update_bricks(GameState *game, int8_t brick_index, color565_t color)
{
  Ball *ball_ptr = &game->ball;
  Brick *brick_ptr = &game->bricks[brick_index];

  // Local aliases for ball
  int8_t radius = ball_ptr->radius;  
  float ball_x = ball_ptr->x;
  float ball_y = ball_ptr->y;
  float ball_old_x = ball_ptr->x_old;
  float ball_old_y = ball_ptr->y_old;

  // Local aliases for brick
  int16_t brick_x = brick_ptr->x;
  int16_t brick_y = brick_ptr->y;
  float brick_width = BRICK_WIDTH;
  int8_t brick_height = BRICK_HEIGHT;

  // if brick isn't drawn then draw it
  if (brick_ptr->alive && !brick_ptr->drawn){
    draw_rectangle(brick_x, brick_y, brick_width, brick_height, color);
    brick_ptr->drawn = true;
  }

  if (brick_ptr->alive) {

    // Collision detection
    // top
    bool top_hit = (ball_x + radius >= brick_x) &&
                    (ball_x - radius <= brick_x + brick_width) &&
                    (ball_old_y + radius <= brick_y) &&
                    (ball_y + radius >= brick_y);

    // Bottom
    bool bottom_hit = (ball_x + radius >= brick_x) &&
                      (ball_x - radius <= brick_x + brick_width) &&
                      (ball_old_y - radius >= brick_y + brick_height) &&
                      (ball_y - radius <= brick_y + brick_height);

    // Left
    bool left_hit = (ball_y + radius >= brick_y) &&
                    (ball_y - radius <= brick_y + brick_height) &&
                    (ball_old_x + radius <= brick_x) &&
                    (ball_x + radius >= brick_x);

    // Right
    bool right_hit = (ball_y + radius >= brick_y) &&
                      (ball_y - radius <= brick_y + brick_height) &&
                      (ball_old_x - radius >= brick_x + brick_width) &&
                      (ball_x - radius <= brick_x + brick_width);

    // If any side is hit then erase the rectangle                  
    if (top_hit || bottom_hit) {
      ball_ptr->vel_y = -ball_ptr->vel_y;
      ball_ptr->vel_y += ((rand() % 4) - 2) * 0.1;  // slight variation
      brick_ptr->alive = false;
      play_brick_hit_sound();
      draw_rectangle(brick_x, brick_y, brick_width, brick_height, WHITE);
    } else if (left_hit || right_hit){
      ball_ptr->vel_x = -ball_ptr->vel_x;
      ball_ptr->vel_x += ((rand() % 4) - 2) * 0.1;  // slight variation
      brick_ptr->alive = false;
      play_brick_hit_sound();
      draw_rectangle(brick_x, brick_y, brick_width, brick_height, WHITE);
    }    
  }
}


// Updates leds lives
void update_lives_led(uint8_t lives)
{
  switch (lives)
  {
    case 3:
      rgb_dc(NO_COLOR_TONE, LOW_COLOR_TONE, NO_COLOR_TONE);
      break;
    case 2:
      rgb_dc(LOW_COLOR_TONE, LOW_COLOR_TONE, NO_COLOR_TONE); 
      break;
    case 1:
      rgb_dc(LOW_COLOR_TONE, NO_COLOR_TONE, NO_COLOR_TONE); 
      break;
    default:
      rgb_dc(NO_COLOR_TONE, NO_COLOR_TONE, NO_COLOR_TONE);
      break;      
  }
}

// Plays on brick hit
void play_brick_hit_sound() {
  motor0_pwm_enable();
  buzzer_tone(3000);
  msec_delay(75);
  buzzer_tone(0);
  motor0_pwm_disable();
}

// Plays on paddle hit
void play_paddle_hit_sound() {
  motor0_pwm_enable();
  buzzer_tone(2500);
  msec_delay(25);
  buzzer_tone(0);
  motor0_pwm_disable();
}

// Plays on life lost
void play_life_lost_sound() {
  motor0_pwm_enable();
  buzzer_tone(1000);
  msec_delay(75);
  buzzer_tone(0);
  motor0_pwm_disable();
}
