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
#include "ST7735S.h"
#include "gfx.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------


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

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------



int main(void) {
  // Initialize 
  clock_init_40mhz();
  launchpad_gpio_init();
  UART_init(115200);
  ST7735S_init();

  // ADC0 and ADC1
  enable_joystick_and_tilt();
  
  // Buzzer
  //motor0_init();
  //buzzer_init();
  //motor0_pwm_enable();

  // Push Buttons
  boost_pb_init();

  // RGBS
  //init_booster_leds();
  //enable_rgb_timers();

  // Uncomment song name to test with RGBs
  //potter();
  // smallworld();
  // mario();

  // Endless loop to prevent program from ending

  while (1) {
    uint16_t xPos = 50;
    uint16_t yPox = 50;
    bool terminate = 1;
    color565_t red = RGB_to_color(255, 0, 0);
    draw_rectangle(50, 50, 50, 50, red);



  }
}
/* main */

void movingRectangle(){
  uint16_t xPos = 50;
  uint16_t yPos = 50;
  bool terminate = false;

  while (!terminate){
    
  }


}
