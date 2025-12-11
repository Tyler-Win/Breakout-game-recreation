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
//    and the BOOSTXL-EDUMKII buzzer, push buttons, joystick, and RGBs.
//    This program
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
#define PB_2                                                  0
#define PB_1                                                  1
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

  // ADC0 and ADC1
  enable_joystick_and_tilt();
  
  // Buzzer

  // Push Buttons
  boost_pb_init();

  // RGBS
  init_booster_leds();
  enable_rgb_timers();

  // Uncomment song name to test with RGBs
  
  // smallworld();
  // mario();

  // Endless loop to prevent program from ending

  while (1) {
    // Once song is finished, test push buttons. RBGs will light up
    buzzer_pwm_disable();
    // Red, Green, Blue. Colors blend
    rgb_dc(boost_pb_down(PB_1) ? BRIGHTNESS : 0,
           boost_pb_down(PB_2) ? BRIGHTNESS : 0,
           boost_pb_down(PB_JOYSTICK) ? BRIGHTNESS : 0);

    // Test joystick controls with RGBs
    // right - red
    if (ADC0_in(X_AXIS) > HIGH__JOYSTICK_VALUE) {
      rgb_dc(HIGH_COLOR_TONE, NO_COLOR_TONE, NO_COLOR_TONE);
      // left - green
    } else if (ADC0_in(X_AXIS) < LOW_JOYSTICK_VALUE) {
      rgb_dc(NO_COLOR_TONE, HIGH_COLOR_TONE, NO_COLOR_TONE);
      // up - blue
    } else if (ADC1_in(Y_AXIS) > HIGH__JOYSTICK_VALUE) {
      rgb_dc(NO_COLOR_TONE, NO_COLOR_TONE, HIGH_COLOR_TONE);
      // down - white
    } else if (ADC1_in(Y_AXIS) < LOW_JOYSTICK_VALUE) {
      rgb_dc(LOW_COLOR_TONE, LOW_COLOR_TONE, LOW_COLOR_TONE);
      // middle - off
    } else {
      rgb_dc(NO_COLOR_TONE, NO_COLOR_TONE, NO_COLOR_TONE);
    }
  }
}
/* main */
