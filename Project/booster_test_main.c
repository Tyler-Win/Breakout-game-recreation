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

void pushbutton_test();
uint8_t filter_adc_val(uint8_t raw_val);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------

#define HIGH__JOYSTICK_VALUE                                  2500
#define LOW_JOYSTICK_VALUE                                    1500
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
}

void pushbutton_test()
{
    buzzer_pwm_disable();


    if (boost_pb_down(PB_2)){
        rgb_dc(NO_COLOR_TONE, HIGH_COLOR_TONE, NO_COLOR_TONE);
    }
    
    if (boost_pb_up(PB_2)){
        rgb_dc(NO_COLOR_TONE, NO_COLOR_TONE, HIGH_COLOR_TONE);
    }
}

void paddle_object()
{
  static uint8_t x_old = 50;
  uint8_t x_new;
  uint8_t y_pos = 40;
  uint8_t paddle_width = 50;
  uint8_t paddle_height = 30;

  color565_t bg_color = RGB_to_color(255, 255, 255);
  color565_t paddle_color = RGB_to_color(254, 0, 0);
  color565_t object = RGB_to_color(0, 0, 255);

  draw_rectangle(x_old, y_pos, paddle_width, paddle_height, bg_color);
  draw_rectangle(30, 30, 50, 50, object);

  while (1) 
  {
    x_new = filter_adc_val(joystick_x());
    x_new = (x_new * (320 - paddle_width)) / 255;

    int delta = x_new - x_old;

    if (delta > DEADZONE || delta < -DEADZONE)
    {

      draw_rectangle(x_old, y_pos, paddle_width, paddle_height, bg_color);

      draw_rectangle(x_new, y_pos, paddle_width, paddle_height, paddle_color);

      x_old = x_new;
    }

    msec_delay(5);
  }
}

uint8_t filter_adc_val(uint8_t raw_val)
{
  uint8_t filter_val = 0;
  filter_val = raw_val / 4;
  return filter_val;       
}

