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
//    This program serves as a recreation of the popular Atari game breakout
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
// *****************************************************************************

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
#include "BG.h"


//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define HIGH_COLOR_TONE 100
#define LOW_COLOR_TONE 5
#define NO_COLOR_TONE 0
#define PB_2                                                  1
#define PB_1                                                  0

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
  Brick bricks[NUM_ROWS * NUM_COLS];
  int8_t lives;

  bool game_started;
  bool game_paused;
} GameState;

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------

void run_breakout(GameState *game);
void handle_pause_menu(GameState *game);
void update_paddle(GameState *game);
void update_ball(GameState *game);
void update_bricks(GameState *game, int8_t brick_index, color565_t color);
void set_bricks(GameState *game);
bool all_bricks_destroyed(GameState *game);
void display_win_screen(GameState *game);
void update_lives_led(uint8_t lives);
void play_brick_hit_sound();
void play_life_lost_sound();
void play_paddle_hit_sound();


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

  // Buzzer
  motor0_init();
  buzzer_init();  

  ST7735S_init();
  ST7735S_clear_lcd(WHITE);

  // Initialize the state of the game
  GameState game = {
      .paddle = {
          .x_pos = PADDLE_SPAWN_X,
          .y_pos = PADDLE_SPAWN_Y,
          .width = PADDLE_WIDTH,
          .height = PADDLE_HEIGHT,
          .x_old = PADDLE_OLD_X,
          .first_draw_done = false
      },
      .ball = {
          .x = BALL_SPAWN_X,
          .y = BALL_SPAWN_Y,
          .x_old = BALL_OLD_X,
          .y_old = BALL_OLD_Y,
          .vel_x = BALL_VEL_X,
          .vel_y = BALL_VEL_Y,
          .radius = BALL_RADIUS
      },
      .lives = LIVES,
      .game_started = false,
      .game_paused = true
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


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Initializes and updates one frame of the breakout game. The game runs
//    the first frame so the user can see the game and doesn't start until PB2
//    is pressed. When the game is unpaused the paddle, ball, bricks, and lives
//    are continue to update till the game is over. There is a 8 milisecond
//    delay coded in.
//
// INPUT PARAMETERS:
//    GameState *game - Pointer to the current game state struct
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------


void run_breakout(GameState *game)
{
  static bool screen_initialized = false;

  handle_pause_menu(game);

  // Draws the first frame before player presses play
  if (!screen_initialized) {
      update_paddle(game);
      update_ball(game);
      set_bricks(game);
      update_lives_led(game->lives);
      screen_initialized = true;
      write_string(FIRST_HALF_START_TEXT_X_POS, FIRST_HALF_START_TEXT_Y_POS, 
      FIRST_HALF_GAME_START_TEXT, RED, SPACING);
      write_string(SECOND_HALF_START_TEXT_X_POS, SECOND_HALF_START_TEXT_Y_POS,
      SECOND_HALF_GAME_START_TEXT, RED, SPACING);
  }

  // Game runs when unpaused
  if (game->game_started && !game->game_paused) {
      update_paddle(game);
      update_ball(game);
      set_bricks(game);
      update_lives_led(game->lives);

      if (all_bricks_destroyed(game)) {
        display_win_screen(game);
        game->game_paused = true;
      }
  }

  msec_delay(8);
}


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Updates the paddle position based on the joystick input and redraws the
//    paddle onto the LCD when it's position is changed. Speed of the paddle
//    varies based on how far left or right the joystick is moved. The paddle
//    movement is constrained to screen boundaries to prevent it from moving
//    off-screen
//
// INPUT PARAMETERS:
//    GameState *game - Pointer to the current game state struct
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Updates the ball position, handles collisions with paddle and screen
//    boundaries, and redraws the ball on the LCD. Life loss and ball reset 
//    behavior is also managed within this function when the ball reaches the
//    the bottom of the screen. When the ball hits the paddle or loses a life
//    a sound is played on the buzzer.
//
// INPUT PARAMETERS:
//    GameState *game - Pointer to the current game state struct
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------

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
  }

  // if the ball's x/y goes outside the top or bottom walls
  // then invert vel_y, add to ball_y, and randomize the balls 
  // x speed
  if (ball_ptr->y - radius <= 0)
  {
      ball_ptr->vel_y = -ball_ptr->vel_y;
      ball_ptr->y += ball_ptr->vel_y;
  }

  // Hits floor. Game over
  if (ball_ptr->y + radius >= SCREEN_HEIGHT)
  {
    if (game->lives > 0)
    {
      game->lives--;
      play_life_lost_sound();
    }
    update_lives_led(game->lives);

    if (game->lives > 0)
    {
      ball_ptr->x = BALL_SPAWN_X;
      ball_ptr->y = BALL_SPAWN_Y;
      ball_ptr->vel_x = 0.5f;
      ball_ptr->vel_y = -0.5f;
      draw_circle(ball_ptr->x_old, ball_ptr->y_old, radius, WHITE);
    } else {
      ball_ptr->x = -100;
      ball_ptr->y = -100;
      ball_ptr->vel_x = 0;
      ball_ptr->vel_y = 0;  
      draw_circle(ball_ptr->x_old, ball_ptr->y_old, radius, WHITE);
      disable_rgb_timers();
    }
  } else {
    // Draw ball at new position
    draw_circle(ball_ptr->x, ball_ptr->y, radius, PURPLE);     
  }          
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Handles drawing and collision for a single brick. This function draws
//    the brick once, detects collision between the brick and the ball, updates
//    the ball's velocity upon impact, and removes the brick when the ball
//    collides with it. 
//    
//
// INPUT PARAMETERS:
//    GameState *game - Pointer to the current game state struct
//    int8_T brick_index - Index of the brick we are accessing
//    color565_t color - Color of the brick
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------

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
  // Ensures we don't constantly redraw bricks each frame
  if (brick_ptr->alive && !brick_ptr->drawn){
    draw_rectangle(brick_x, brick_y, brick_width, brick_height, color);
    brick_ptr->drawn = true;
  }

  // Collision detection
  if (brick_ptr->alive) {
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
      brick_ptr->alive = false;
      play_brick_hit_sound();
      draw_rectangle(brick_x, brick_y, brick_width, brick_height, WHITE);
    } else if (left_hit || right_hit){
      ball_ptr->vel_x = -ball_ptr->vel_x;
      brick_ptr->alive = false;
      play_brick_hit_sound();
      draw_rectangle(brick_x, brick_y, brick_width, brick_height, WHITE);
    }    
  }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Updates the lives of the player based on if the ball hits the bottom
//    screen and misses collision with the paddle. green indicates 3 lives left,
//    yellow indicates 2 lives left, and red indicates 1 life left. When the
//    player loses all lives the LED turns off
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------

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
      disable_rgb_timers();
      write_full_image();
      break;      
  }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function enables the buzzer for a short duration when the player
//    breaks a brick with the ball. The buzzer plays a low frequency tone 
//    following a short delay and then turns off.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------

void play_brick_hit_sound() {
  motor0_pwm_enable();
  buzzer_tone(3000);
  msec_delay(75);
  buzzer_tone(0);
  motor0_pwm_disable();
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function enables the buzzer for a short duration when the player
//    hits the ball against the paddle. The buzzer plays a low frequency tone 
//    following a short delay and then turns off.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------

void play_paddle_hit_sound() {
  motor0_pwm_enable();
  buzzer_tone(2500);
  msec_delay(25);
  buzzer_tone(0);
  motor0_pwm_disable();
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function enables the buzzer for a short duration when the ball hits
//    against the bottom wall of the LCD. The buzzer plays a low frequency tone 
//    following a short delay and then turns off.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------

void play_life_lost_sound() {
  motor0_pwm_enable();
  buzzer_tone(1000);
  msec_delay(75);
  buzzer_tone(0);
  motor0_pwm_disable();
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function set's the color of the bricks when the program first
//    runs and accesses update_bricks which will check the state of the bricks
//    and potential collisions with the ball. 
//
// INPUT PARAMETERS:
//    GameState *game - Pointer to the current game state struct
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// ----------------------------------------------------------------------------

void set_bricks(GameState *game)
{
  color565_t brick_colors[NUM_ROWS] = {RED, ORANGE, BLUE, GREEN};
  int8_t brick_index = 0;
  for (int8_t row = 0; row < NUM_ROWS; row++) {
    for (int8_t col = 0; col < NUM_COLS; col++) {
        update_bricks(game, brick_index, brick_colors[row]);
        brick_index++;
    }
  }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function handles the pause menu functionality. When the game starts
//    the game is paused and the LED is off until PB1 is pressed, which starts
//    the game and turns on the RGB LED. The game can be paused using PB2.
//
// INPUT PARAMETERS:
//    GameState *game - Pointer to the current game state struct
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// ----------------------------------------------------------------------------

void handle_pause_menu(GameState *game)
{
    static bool last_pb2 = false;

    bool pb2 = boost_pb_down(PB_2);   // true when currently pressed

    // Detect rising edge (button pressed this frame but not last frame)
    if (pb2 && !last_pb2)
    {
        // Start game when PB2 pressed
        if (!game->game_started)
        {
            game->game_started = true;
            game->game_paused = false;
            enable_rgb_timers();
            
            write_string(FIRST_HALF_START_TEXT_X_POS, FIRST_HALF_START_TEXT_Y_POS, 
            FIRST_HALF_GAME_START_TEXT, WHITE, SPACING);
            
            write_string(SECOND_HALF_START_TEXT_X_POS, SECOND_HALF_START_TEXT_Y_POS,
            SECOND_HALF_GAME_START_TEXT, WHITE, SPACING);
        }
        else
        {
            // Toggle pause state
            game->game_paused = !game->game_paused;
            
            if (game->game_paused){
              write_string(PAUSE_X_POS, PAUSE_Y_POS, PAUSE_TEXT, RED, SPACING);
            } else {
              write_string(PAUSE_X_POS, PAUSE_Y_POS, PAUSE_TEXT, WHITE, SPACING);              
            }
                       
        }
    }
    
    last_pb2 = pb2;
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function checks if all the bricks are destroyed, if so the player
//    has won the game and the win state is activated
//
// INPUT PARAMETERS:
//    GameState *game - Pointer to the current game state struct
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// ----------------------------------------------------------------------------

bool all_bricks_destroyed(GameState *game){
  for (int i = 0; i < NUM_ROWS * NUM_COLS;  i++) {
    if (game->bricks[i].alive) {
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function displays the win screen to the user
//
// INPUT PARAMETERS:
//    GameState *game - Pointer to the current game state struct
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// ----------------------------------------------------------------------------

void display_win_screen(GameState *game) {
  game->ball.x = -100;
  game->ball.y = -100;
  game->ball.vel_x = 0;
  game->ball.vel_y = 0;

  rgb_dc(LOW_COLOR_TONE, LOW_COLOR_TONE, NO_COLOR_TONE);

  write_string(WIN_X_POS, WIN_Y_POS, WIN_TEXT, RED, SPACING);
}