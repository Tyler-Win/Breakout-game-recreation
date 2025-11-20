// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//    DESIGNER NAME:  Ryanne Geiger
//
//          VERSION:  0.1
//
//        FILE NAME:  booster.c
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


//-----------------------------------------------------------------------------
// Define symbolic constants used by program
//-----------------------------------------------------------------------------
#define ACTIVE_LOW                                      0
#define ACTIVE_HIGH                                     1

// Define a structure to hold led configuration data
typedef struct
{
  uint8_t  port_id;
  uint32_t bit_mask;
  uint16_t pin_cm;
  uint8_t  polarity;
} gpio_boost_struct;


// Define the configuration data for the push buttons on the BOOSTXL-EDUMKII
const gpio_boost_struct pb_config_data[] = {
        {PB1_PORT, PB1_MASK, PB1_IOMUX, ACTIVE_HIGH},
        {PB2_PORT, PB2_MASK, PB2_IOMUX, ACTIVE_HIGH},
        {PB3_PORT, PB3_MASK, PB3_IOMUX, ACTIVE_HIGH}
};

//***************************************************************************
//***************************************************************************
//******                    RGB Managment functions                    ******
//***************************************************************************
//***************************************************************************

//-----------------------------------------------------------------------------
// DESCRIPTION:
//   This function enables the timers A1 and G7 for use of the RGBs
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
void enable_rgb_timers() {
  TIMA1->COUNTERREGS.CTRCTL |= (GPTIMER_CTRCTL_EN_ENABLED);
  TIMG7->COUNTERREGS.CTRCTL |= (GPTIMER_CTRCTL_EN_ENABLED);
} /* enable_rgb_timers */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function disables the timers A1 and G7 for the RGBs
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
void disable_rgb_timers() {
  TIMA1->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_MASK);
  TIMG7->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_MASK);
} /* disbale_rgb_timers*/

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Initializes the RGB LEDs on the booster pack by configuring GPIO pins
//    and timer modules for PWM control of red, green, and blue LEDs.
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
void init_booster_leds() {
  red_rgb_enable_init();
  green_rgb_enable_init();
  blue_rgb_enable_init();
  // red
  GPIOB->DOESET31_0 = (1 << 1);
  // green/blue
  GPIOA->DOESET31_0 = (1 << 28) | (1 << 31);

  timer_a1_init();
  timer_g7_init();
} /* init_booster_leds*/

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Initializes Timer Group 7 (TIMG7) for PWM signal generation
//    with specific clock, prescaler, and compare settings for RGB LED control.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    Configures TIMG7 timer registers for PWM operation
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void timer_g7_init() {
  // Reset TIMG7
  TIMG7->GPRCM.RSTCTL =
      (GPTIMER_RSTCTL_KEY_UNLOCK_W | GPTIMER_RSTCTL_RESETSTKYCLR_CLR |
       GPTIMER_RSTCTL_RESETASSERT_ASSERT);

  // Enable power to TIMG7
  TIMG7->GPRCM.PWREN =
      (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);

  clock_delay(24);

  TIMG7->CLKSEL =
      (GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE | GPTIMER_CLKSEL_MFCLK_SEL_DISABLE |
       GPTIMER_CLKSEL_LFCLK_SEL_DISABLE);

  TIMG7->CLKDIV = GPTIMER_CLKDIV_RATIO_DIV_BY_8;

  // set the pre-scale count value that divides select clock by PCNT+1
  // TimerClock = BusCock / (DIVIDER * (PRESCALER))
  // 200,000 Hz = 40,000,000 Hz / (8 * (24 + 1))
  TIMG7->COMMONREGS.CPS = GPTIMER_CPS_PCNT_MASK & 0x18;

  // Set action for compare
  // On Zero, set output LOW; On Compares up, set output HIGH
  TIMG7->COUNTERREGS.CCACT_01[0] =
      (GPTIMER_CCACT_01_FENACT_DISABLED | GPTIMER_CCACT_01_CC2UACT_DISABLED |
       GPTIMER_CCACT_01_CC2DACT_DISABLED | GPTIMER_CCACT_01_CUACT_CCP_LOW |
       GPTIMER_CCACT_01_CDACT_DISABLED | GPTIMER_CCACT_01_LACT_DISABLED |
       GPTIMER_CCACT_01_ZACT_CCP_HIGH);

  TIMG7->COUNTERREGS.CCACT_01[1] =
      (GPTIMER_CCACT_01_FENACT_DISABLED | GPTIMER_CCACT_01_CC2UACT_DISABLED |
       GPTIMER_CCACT_01_CC2DACT_DISABLED | GPTIMER_CCACT_01_CUACT_CCP_LOW |
       GPTIMER_CCACT_01_CDACT_DISABLED | GPTIMER_CCACT_01_LACT_DISABLED |
       GPTIMER_CCACT_01_ZACT_CCP_HIGH);

  // set timer reload value
  TIMG7->COUNTERREGS.LOAD = GPTIMER_LOAD_LD_MASK & (100 - 1);

  // set timer compare value
  TIMG7->COUNTERREGS.CC_01[0] = GPTIMER_CC_01_CCVAL_MASK & 50;
  TIMG7->COUNTERREGS.CC_01[1] = GPTIMER_CC_01_CCVAL_MASK & 50;

  // set compare control for PWM func with output initially low
  TIMG7->COUNTERREGS.OCTL_01[0] =
      (GPTIMER_OCTL_01_CCPIV_LOW | GPTIMER_OCTL_01_CCPOINV_NOINV |
       GPTIMER_OCTL_01_CCPO_FUNCVAL);
  TIMG7->COUNTERREGS.OCTL_01[1] =
      (GPTIMER_OCTL_01_CCPIV_LOW | GPTIMER_OCTL_01_CCPOINV_NOINV |
       GPTIMER_OCTL_01_CCPO_FUNCVAL);

  TIMG7->COUNTERREGS.CCCTL_01[0] = GPTIMER_CCCTL_01_CCUPD_IMMEDIATELY;
  TIMG7->COUNTERREGS.CCCTL_01[1] = GPTIMER_CCCTL_01_CCUPD_IMMEDIATELY;

  // When enabled load 0, set timer to count up
  TIMG7->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_CVAE_ZEROVAL |
                              GPTIMER_CTRCTL_REPEAT_REPEAT_1 |
                              GPTIMER_CTRCTL_CM_UP;

  TIMG7->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;

  // No interrupt is required
  TIMG7->CPU_INT.IMASK = GPTIMER_CPU_INT_IMASK_L_CLR;

  // set C0 and C1 as output
  TIMG7->COMMONREGS.CCPD =
      (GPTIMER_CCPD_C0CCP3_INPUT | GPTIMER_CCPD_C0CCP2_INPUT |
       GPTIMER_CCPD_C0CCP1_OUTPUT | GPTIMER_CCPD_C0CCP0_OUTPUT);
} /* timer_g7_init */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Initializes Timer A1 (TIMA1) for PWM signal generation
//    with specific clock, prescaler, and compare settings for RGB LED control.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    Configures TIMA1 timer registers for PWM operation
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void timer_a1_init() {
  // Reset TIMA1
  TIMA1->GPRCM.RSTCTL =
      (GPTIMER_RSTCTL_KEY_UNLOCK_W | GPTIMER_RSTCTL_RESETSTKYCLR_CLR |
       GPTIMER_RSTCTL_RESETASSERT_ASSERT);

  // Enable power to TIMA1
  TIMA1->GPRCM.PWREN =
      (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);

  clock_delay(24);

  TIMA1->CLKSEL =
      (GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE | GPTIMER_CLKSEL_MFCLK_SEL_DISABLE |
       GPTIMER_CLKSEL_LFCLK_SEL_DISABLE);

  TIMA1->CLKDIV = GPTIMER_CLKDIV_RATIO_DIV_BY_8;

  // set the pre-scale count value that divides select clock by PCNT+1
  // TimerClock = BusCock / (DIVIDER * (PRESCALER))
  // 200,000 Hz = 40,000,000 Hz / (8 * (24 + 1))
  TIMA1->COMMONREGS.CPS = GPTIMER_CPS_PCNT_MASK & 0x18;

  // Set action for compare
  // On Zero, set output LOW; On Compares up, set output HIGH
  TIMA1->COUNTERREGS.CCACT_01[1] =
      (GPTIMER_CCACT_01_FENACT_DISABLED | GPTIMER_CCACT_01_CC2UACT_DISABLED |
       GPTIMER_CCACT_01_CC2DACT_DISABLED | GPTIMER_CCACT_01_CUACT_CCP_LOW |
       GPTIMER_CCACT_01_CDACT_DISABLED | GPTIMER_CCACT_01_LACT_DISABLED |
       GPTIMER_CCACT_01_ZACT_CCP_HIGH);

  // set timer reload value
  TIMA1->COUNTERREGS.LOAD = GPTIMER_LOAD_LD_MASK & (100 - 1);

  // set timer compare value
  TIMA1->COUNTERREGS.CC_01[1] = GPTIMER_CC_01_CCVAL_MASK & 50;

  // set compare control for PWM func with output initially low
  TIMA1->COUNTERREGS.OCTL_01[1] =
      (GPTIMER_OCTL_01_CCPIV_LOW | GPTIMER_OCTL_01_CCPOINV_NOINV |
       GPTIMER_OCTL_01_CCPO_FUNCVAL);
  //
  TIMA1->COUNTERREGS.CCCTL_01[1] = GPTIMER_CCCTL_01_CCUPD_IMMEDIATELY;

  // When enabled load 0, set timer to count up
  TIMA1->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_CVAE_ZEROVAL |
                              GPTIMER_CTRCTL_REPEAT_REPEAT_1 |
                              GPTIMER_CTRCTL_CM_UP;

  TIMA1->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;

  // No interrupt is required
  TIMA1->CPU_INT.IMASK = GPTIMER_CPU_INT_IMASK_L_CLR;

  // set C1 as output
  TIMA1->COMMONREGS.CCPD =
      (GPTIMER_CCPD_C0CCP3_INPUT | GPTIMER_CCPD_C0CCP2_INPUT |
       GPTIMER_CCPD_C0CCP1_OUTPUT | GPTIMER_CCPD_C0CCP0_INPUT);
  ;
} /* timer_a1_init*/

//-----------------------------------------------------------------------------
// DESCRIPTION:
//   Sets the duty cycle for red, green, and blue LEDs using PWM
//   by adjusting compare values in timer modules.
//
// INPUT PARAMETERS:
//    red (uint8_t):   Percentage of red LED brightness (0-100)
//    green (uint8_t): Percentage of green LED brightness (0-100)
//    blue (uint8_t):  Percentage of blue LED brightness (0-100)
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void rgb_dc(uint8_t red, uint8_t green, uint8_t blue) {
  uint32_t red_threshold = (TIMA1->COUNTERREGS.LOAD * red) / 100;
  uint32_t green_threshold = (TIMG7->COUNTERREGS.LOAD * green) / 100;
  uint32_t blue_threshold = (TIMG7->COUNTERREGS.LOAD * blue) / 100;

  TIMA1->COUNTERREGS.CC_01[1] = GPTIMER_CC_01_CCVAL_MASK & red_threshold;
  TIMG7->COUNTERREGS.CC_01[0] = GPTIMER_CC_01_CCVAL_MASK & green_threshold;
  TIMG7->COUNTERREGS.CC_01[1] = GPTIMER_CC_01_CCVAL_MASK & blue_threshold;
} /* rgb_dc */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Configures the IOMUX pin for the red RGB LED to use Timer A1
//    capture/compare output pin.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    Configures IOMUX pin for red LED PWM control
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void red_rgb_enable_init() {
  IOMUX->SECCFG.PINCM[IOMUX_PINCM13] =
      IOMUX_PINCM13_PF_TIMA1_CCP1 | IOMUX_PINCM_PC_CONNECTED;
} /* red_rgb_enable_init */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Sets the frequency for the red RGB LED by adjusting the timer
//    reload and compare values to generate a PWM signal.
//
// INPUT PARAMETERS:
//    frequency (uint16_t): Desired frequency for the red LED tone (Hz)
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void red_rgb_tone(uint16_t frequency) {
  // clock freq is 20khz so 20,000 / frequency = desired frequency
  // set timer reload value
  TIMA1->COUNTERREGS.LOAD = GPTIMER_LOAD_LD_MASK & ((200000 / 100) - 1);
  // set 50% duty cycle
  uint32_t threshold = (TIMA1->COUNTERREGS.LOAD * frequency) / 150;
  TIMA1->COUNTERREGS.CC_01[1] = GPTIMER_CC_01_CCVAL_MASK & threshold;
} /* red_rgb_tone */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Configures the IOMUX pin for the green RGB LED to use Timer Group 7
//    capture/compare output pin.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    Configures IOMUX pin for green LED PWM control
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void green_rgb_enable_init(void) {
  IOMUX->SECCFG.PINCM[IOMUX_PINCM3] =
      IOMUX_PINCM3_PF_TIMG7_CCP0 | IOMUX_PINCM_PC_CONNECTED;
} /* green_rgb_enable_init */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Sets the frequency for the green RGB LED by adjusting the timer
//    reload and compare values to generate a PWM signal.
//
// INPUT PARAMETERS:
//    frequency (uint16_t): Desired frequency for the green LED tone (Hz)
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void green_rgb_tone(uint16_t frequency) {
  // clock freq is 20khz so 20,000 / frequency = desired frequency
  // set timer reload value
  TIMA0->COUNTERREGS.LOAD = GPTIMER_LOAD_LD_MASK & ((200000 / 100) - 1);

  // set 50% duty cycle
  uint32_t threshold = (TIMA0->COUNTERREGS.LOAD * frequency) / 150;
  TIMA0->COUNTERREGS.CC_23[1] = GPTIMER_CC_23_CCVAL_MASK & threshold;
} /* green_rgb_tone */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Configures the IOMUX pin for the blue RGB LED to use Timer Group 7
//    capture/compare output pin.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    Configures IOMUX pin for blue LED PWM control
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void blue_rgb_enable_init(void) {
  IOMUX->SECCFG.PINCM[IOMUX_PINCM6] =
      IOMUX_PINCM6_PF_TIMG7_CCP1 | IOMUX_PINCM_PC_CONNECTED;
} /* blue_rgb_enable_init */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Sets the frequency for the blue RGB LED by adjusting the timer
//    reload and compare values to generate a PWM signal.
//
// INPUT PARAMETERS:
//    frequency (uint16_t): Desired frequency for the blue LED tone (Hz)
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void blue_rgb_tone(uint16_t frequency) {
  // clock freq is 20khz so 20,000 / frequency = desired frequency
  // set timer reload value
  TIMA0->COUNTERREGS.LOAD = GPTIMER_LOAD_LD_MASK & ((200000 / 100) - 1);

  // set 50% duty cycle
  uint32_t threshold = (TIMA0->COUNTERREGS.LOAD * frequency) / 150;
  TIMA0->COUNTERREGS.CC_23[1] = GPTIMER_CC_23_CCVAL_MASK & threshold;
} /* blue_rgb_tone */

//***************************************************************************
//***************************************************************************
//******                   Buzzer Managment functions                  ******
//***************************************************************************
//***************************************************************************

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Enables the Timer A0 (TIMA0) counter, starting PWM signal generation
//    for the buzzer. This activates the previously configured timer,
//    allowing it to generate the PWM output based on the initialization
//    settings.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//    Activates the timer counter, beginning PWM signal output
//
// RETURN:
//  none
// -----------------------------------------------------------------------------
void buzzer_pwm_enable(void) {
  TIMA0->COUNTERREGS.CTRCTL |= (GPTIMER_CTRCTL_EN_ENABLED);
} /* buzzer_pwm_enable */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Disables the Timer A0 (TIMA0) counter, stopping PWM signal generation
//    for the buzzer. This immediately halts the timer's operation,
//    effectively silencing the buzzer and preventing further PWM output.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//   Deactivates the timer counter, stopping PWM signal output
//
// RETURN:
//  none
// -----------------------------------------------------------------------------
void buzzer_pwm_disable(void) {
  TIMA0->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_MASK);
} /* buzzer_pwm_disable */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Initializes the buzzer using Timer A0 (TIMA0) in PWM mode.
//    Configures the timer with specific clock, prescaler, and compare
//    settings to prepare for generating audio signals.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    Configures TIMA0 timer registers for PWM buzzer output
//    - Resets and enables timer
//    - Sets clock source and divider
//    - Configures timer compare and output control registers
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void buzzer_init() {
  // Reset TIMA0
  TIMA0->GPRCM.RSTCTL =
      (GPTIMER_RSTCTL_KEY_UNLOCK_W | GPTIMER_RSTCTL_RESETSTKYCLR_CLR |
       GPTIMER_RSTCTL_RESETASSERT_ASSERT);

  // Enable power to TIMA0
  TIMA0->GPRCM.PWREN =
      (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);

  clock_delay(24);

  TIMA0->CLKSEL =
      (GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE | GPTIMER_CLKSEL_MFCLK_SEL_DISABLE |
       GPTIMER_CLKSEL_LFCLK_SEL_DISABLE);

  TIMA0->CLKDIV = GPTIMER_CLKDIV_RATIO_DIV_BY_8;

  // set the pre-scale count value that divides select clock by PCNT+1
  // TimerClock = BusCock / (DIVIDER * (PRESCALER))
  // 200,000 Hz = 40,000,000 Hz / (8 * (24 + 1))
  TIMA0->COMMONREGS.CPS = GPTIMER_CPS_PCNT_MASK & 0x18;

  // Set action for compare
  // On Zero, set output HIGH; On Compares up, set output LOW
  TIMA0->COUNTERREGS.CCACT_23[0] =
      (GPTIMER_CCACT_23_FENACT_DISABLED | GPTIMER_CCACT_23_CC2UACT_DISABLED |
       GPTIMER_CCACT_23_CC2DACT_DISABLED | GPTIMER_CCACT_23_CUACT_CCP_LOW |
       GPTIMER_CCACT_23_CDACT_DISABLED | GPTIMER_CCACT_23_LACT_DISABLED |
       GPTIMER_CCACT_23_ZACT_CCP_HIGH);

  // set timer reload value
  TIMA0->COUNTERREGS.LOAD = GPTIMER_LOAD_LD_MASK & (100 - 1);

  // set timer compare value
  TIMA0->COUNTERREGS.CC_23[0] = GPTIMER_CC_23_CCVAL_MASK & 50;

  // set compare control for PWM func with output initially low
  TIMA0->COUNTERREGS.OCTL_23[0] =
      (GPTIMER_OCTL_23_CCPIV_LOW | GPTIMER_OCTL_23_CCPOINV_NOINV |
       GPTIMER_OCTL_23_CCPO_FUNCVAL);
  //
  TIMA0->COUNTERREGS.CCCTL_23[0] = GPTIMER_CCCTL_23_CCUPD_IMMEDIATELY;

  // When enabled load 0, set timer to count up
  TIMA0->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_CVAE_ZEROVAL |
                              GPTIMER_CTRCTL_REPEAT_REPEAT_1 |
                              GPTIMER_CTRCTL_CM_UP;

  TIMA0->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;

  // No interrupt is required
  TIMA0->CPU_INT.IMASK = GPTIMER_CPU_INT_IMASK_L_CLR;

  // set C0 as output
  TIMA0->COMMONREGS.CCPD =
      (GPTIMER_CCPD_C0CCP3_INPUT | GPTIMER_CCPD_C0CCP2_OUTPUT |
       GPTIMER_CCPD_C0CCP1_INPUT | GPTIMER_CCPD_C0CCP0_INPUT);
  ;

} /* buzzer_init */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Generates a tone on the buzzer by configuring Timer A0 (TIMA0)
//    to produce a PWM signal at a specific frequency. This allows
//    playing musical notes or creating sound effects by dynamically
//    adjusting the timer's reload and compare values.
//
// INPUT PARAMETERS:
//    frequency (uint16_t): Desired tone frequency in Hertz (Hz).
//    Determines the pitch of the sound produced by the buzzer.
//    Typical range: 20 Hz to 20,000 Hz (human hearing range)
//
// OUTPUT PARAMETERS:
//    Modifies TIMA0 timer registers to:
//    - Set timer reload value based on desired frequency
//    - Configure 50% duty cycle for symmetric PWM signal
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void buzzer_tone(uint16_t frequency) {
  // clock freq is 20khz so 20,000 / frequency = desired frequency
  // set timer reload value
  TIMA0->COUNTERREGS.LOAD = GPTIMER_LOAD_LD_MASK & ((200000 / frequency) - 1);

  // set 50% duty cycle
  uint32_t threshold = (TIMA0->COUNTERREGS.LOAD * 50) / 150;
  TIMA0->COUNTERREGS.CC_23[0] = GPTIMER_CC_23_CCVAL_MASK & threshold;
} /* buzzer_tone */

//***************************************************************************
//***************************************************************************
//******         BoosterPack PushButtons Managment functions           ******
//***************************************************************************
//***************************************************************************

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Initializes the push buttons (PBs) on the booster pack by
//    configuring the GPIO pin multiplexing settings. This function
//    sets up the input pin configuration for three push buttons,
//    enabling input functionality and ensuring proper GPIO connection.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    Configures IOMUX pin settings for push buttons:
//    - Enables input functionality
//    - Sets pin to GPIO mode
//    - Connects pin to GPIO peripheral
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void boost_pb_init(void) {
  uint32_t gpio_pincm =
      IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED | PINCM_GPIO_PIN_FUNC;

  for (uint8_t sw_idx = 0; sw_idx < 3; sw_idx++) {
    IOMUX->SECCFG.PINCM[pb_config_data[sw_idx].pin_cm] = gpio_pincm;
  } /* for */

} /* boost_pb_init */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads the current state of the pushbuttons on the CSC202
//    Expansion Board and returns TRUE if the button is down.
//
// INPUT PARAMETERS:
//    pb_idx - an 8-bit value that represent the pushbutton index
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    0 if pushbutton is up
//    1 if pushbutton is down
// -----------------------------------------------------------------------------
bool boost_pb_down(uint8_t pb_idx) {
  bool pb_status = false;
  uint8_t pb_value = 0;

  if (pb_config_data[pb_idx].port_id == GPIO_PORTA) {
    uint32_t gpio_val = GPIOA->DIN31_0;
    pb_value = ((GPIOA->DIN31_0 & pb_config_data[pb_idx].bit_mask) ==
                pb_config_data[pb_idx].bit_mask);
  } /* if */
  // Boost module does not use GPIOB

  // IOMUX has inversion enabled so used !polarity
  if (pb_value != pb_config_data[pb_idx].polarity) {
    pb_status = true;
  } /* if */

  return (pb_status);

} /* boost_pb_down */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads the current state of the DIP switches on the CSC202
//    Expansion Board and returns TRUE if the button is up.
//
// INPUT PARAMETERS:
//    pb_idx - an 8-bit value that represent the pushbutton index
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    0 if pushbutton is down
//    1 if pushbutton is up
// -----------------------------------------------------------------------------
bool boost_pb_up(uint8_t pb_idx) {
  return !boost_pb_down(pb_idx);
} /* boost_pb_up */

//***************************************************************************
//***************************************************************************
//******                   JoyStick Managment functions                ******
//***************************************************************************
//***************************************************************************

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Initializes Analog-to-Digital Converters (ADC0 and ADC1) for joystick
//    and tilt sensor input. This function configures both ADC modules to use
//    the internal reference voltage and ground reference.
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
void enable_joystick_and_tilt() {
  ADC0_init(ADC12_MEMCTL_VRSEL_INTREF_VSSA);
  ADC1_init(ADC12_MEMCTL_VRSEL_INTREF_VSSA);
} /* enable_joystick_and_tilt */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function uses the input from ADC0, channel 2 to read the x-axis
//    joystick value
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
uint8_t joystick_x() 
{ 
  uint8_t x_val = ADC0_in(2); 
  return x_val;
} /* joystick_x */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function uses the input from ADC1, channel 3 to read the y-axis
//    joystick value
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
uint8_t joystick_y() { 
  uint8_t y_val = ADC1_in(3); 
  return y_val;
} /* joystick_y */

//***************************************************************************
//***************************************************************************
//******                Acceleromter Managment functions               ******
//***************************************************************************
//***************************************************************************

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function uses the input from ADC0, channel 7 to read the y-axis
//    accelerometer value, ADC1, channel 6 to read the x-axis accelerometer
//    value, and ADC1, channel 5 to read the z-axis accelerometer value
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
void accelerometer_tilt() {
  // y-axis
  ADC0_in(7);
  // x-axis
  ADC1_in(6);
  // z-axis
  ADC1_in(5);
} /* accelerometer_tilt */

//***************************************************************************
//***************************************************************************
//******                Microphone Managment functions                 ******
//***************************************************************************
//***************************************************************************

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function uses ADC0, channel 5 to read the microphone value
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
void input_microphone() { ADC0_in(5); } /* input_microphone */