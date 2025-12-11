// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Andrew DeFord
//
//         VERSION:  0.1
//
//       FILE NAME:  ST7735S.c
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

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "gfx.h"
#include "font8x8_basic.h"

void draw_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, 
                      color565_t color)
{
  ST7735S_set_addr(x, y, x + width, y + width);
  ST7735S_write_command(LCD_RAMWR_CMD);
  for(uint16_t i = 0; i < width * height; i++)
  {
    ST7735S_write_color(color);
  }
}

void draw_circle(uint8_t cx, uint8_t cy, uint8_t radius, color565_t color)
{
  for (int y = -radius; y <= radius; y++) {
    for (int x = -radius; x <= radius; x++) {
      if (x*x + y*y <= radius*radius) {
        ST7735S_set_addr(cx + x, cy + y, cx + x, cy + y);
        ST7735S_write_command(LCD_RAMWR_CMD);
        ST7735S_write_color(color);
      }
    }
  }
}

void write_full_image(){
  int i = 0;
  uint8_t HEIGHT = 128;
  uint8_t WIDTH = 128;
  extern const uint8_t image_data[][3];
  
  for (int y = 0; y < HEIGHT; y++) {
      for (int x = 0; x < WIDTH; x++) {
        uint8_t r = image_data[i][0];
        uint8_t g = image_data[i][1];
        uint8_t b = image_data[i][2];

        draw_pixel(x, y, RGB_to_color(r, g, b));
        i++;
        
      }
  }
}

void draw_char(uint8_t x_pos, uint8_t y_pos, char c, color565_t color){
  for (uint8_t row = 0; row < 8; row++) {
        uint8_t row_data = font8x8_basic[(uint8_t)c][row];
        for (uint8_t col = 0; col < 8; col++) {
            if (row_data & (1 << col)) {        // if the bit is 1, draw pixel
                draw_pixel(x_pos + col, y_pos + row, color);
            }
        }
    }
}

void write_string(uint8_t x_pos, uint8_t y_pos, char* user_string, color565_t color, uint8_t spacing){
  
  uint8_t cursor_x = x_pos;

    for (uint16_t i = 0; user_string[i] != '\0'; i++)
    {
        char c = user_string[i];

        draw_char(cursor_x, y_pos, c, color);

        cursor_x += spacing;   // <-- spacing between letters (depends on your font width)
    }
  
}

void draw_pixel(uint8_t x, uint8_t y, color565_t color)
{
  ST7735S_set_addr(x, y, x, y);      
  ST7735S_write_command(LCD_RAMWR_CMD);
  ST7735S_write_color(color);
}