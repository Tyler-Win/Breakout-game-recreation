// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Ryanne Geiger
//
//         VERSION:  0.1
//
//       FILE NAME:  booster.h
//
//-----------------------------------------------------------------------------
#ifndef __BOOSTER_H__
#define __BOOSTER_H__

// Defines for the pushbutton 1, pushbutton 2, joystick select on boosterpack
#define PB1_PORT                                                    (GPIO_PORTA)
#define PB1_MASK                                                       (1 << 11)
#define PB1_IOMUX                                                (IOMUX_PINCM22)
#define PB2_PORT                                                    (GPIO_PORTA)
#define PB2_MASK                                                       (1 << 12)
#define PB2_IOMUX                                                (IOMUX_PINCM34)
#define PB3_PORT                                                    (GPIO_PORTA)
#define PB3_MASK                                                       (1 << 26)
#define PB3_IOMUX                                                (IOMUX_PINCM59)


// --------------------------------------------------------------------------
// Prototype for boosterpack support functions
// --------------------------------------------------------------------------

// RGB
void enable_rgb_timers(void);
void disable_rgb_timers(void);
// RGB
void init_booster_leds(void);
void timer_g7_init(void);
void timer_a1_init(void);
// RGB
void red_rgb_enable_init(void);
void red_rgb_tone(uint16_t frequency);
void green_rgb_enable_init(void);
void green_rgb_tone(uint16_t frequency);
void blue_rgb_enable_init(void);
void blue_rgb_tone(uint16_t frequency);
void rgb_dc(uint8_t red, uint8_t green, uint8_t blue);

// Buzzer
void buzzer_pwm_enable(void);
void buzzer_pwm_disable(void);
void buzzer_init(void);
void buzzer_tone(uint16_t frequency);

// Boost Buttons
void boost_pb_init(void);
bool boost_pb_down(uint8_t pb_idx);
bool boost_pb_up(uint8_t pb_idx); 

// Other 
void enable_joystick_and_tilt(void);
void accelerometer_tilt(void);
void input_microphone(void);
uint8_t joystick_x();
uint8_t joystick_y();

#endif // __BOOSTER_H__