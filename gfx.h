// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Andrew DeFord
//
//         VERSION:  0.1
//
//       FILE NAME:  ST7735S.h
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    
//
//-----------------------------------------------------------------------------
// DISCLAIMER
//    
// *****************************************************************************
//******************************************************************************

#ifndef __GFX_H__
#define __GFX_H__

#include "ST7735S.h"

// ----------------------------------------------------------------------------
// Prototype for support functions
// ----------------------------------------------------------------------------

void draw_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, 
                      color565_t color);

void draw_circle(uint8_t cx, uint8_t cy, uint8_t radius, color565_t color);

void write_full_image();

void draw_char(uint8_t x_pos, uint8_t y_pos, char c, color565_t color);

void write_string(uint8_t x_pos, uint8_t y_pos, char* user_string, color565_t color, uint8_t spacing);

void draw_pixel(uint8_t x, uint8_t y, color565_t color);

#endif /* __SPI_H__ */
