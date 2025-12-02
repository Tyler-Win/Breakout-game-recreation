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

void draw_circle (uint8_t center_x, uint8_t center_y, uint8_t x, uint8_t y)
{
  ST7735S_set_addr(x, y, center_x + x, center_y + y);
  ST7735S_write_command(LCD_RAMWR_CMD);
  ST7735S_write_color(RGB_to_color(255, 0, 0));

  ST7735S_set_addr(x, y, center_x - x, center_y + y);
  ST7735S_write_command(LCD_RAMWR_CMD);
  ST7735S_write_color(RGB_to_color(255, 0, 0));

  ST7735S_set_addr(x, y, center_x + x, center_y - y);
  ST7735S_write_command(LCD_RAMWR_CMD);
  ST7735S_write_color(RGB_to_color(255, 0, 0));

  ST7735S_set_addr(x, y, center_x - x, center_y - y);
  ST7735S_write_command(LCD_RAMWR_CMD);
  ST7735S_write_color(RGB_to_color(255, 0, 0));

  ST7735S_set_addr(x, y, center_x + y, center_y + x);
  ST7735S_write_command(LCD_RAMWR_CMD);
  ST7735S_write_color(RGB_to_color(255, 0, 0));

  ST7735S_set_addr(x, y, center_x - y, center_y + x);
  ST7735S_write_command(LCD_RAMWR_CMD);
  ST7735S_write_color(RGB_to_color(255, 0, 0));

  ST7735S_set_addr(x, y, center_x + y, center_y - x);
  ST7735S_write_command(LCD_RAMWR_CMD);
  ST7735S_write_color(RGB_to_color(255, 0, 0));

  ST7735S_set_addr(x, y, center_x - y, center_y - x);
  ST7735S_write_command(LCD_RAMWR_CMD);
  ST7735S_write_color(RGB_to_color(255, 0, 0));  
}

// Function for circle-generation
// using Bresenham's algorithm
void circleBres(int xc, int yc, int r){
    int x = 0, y = r;
    int d = 3 - 2 * r;
    draw_circle(xc, yc, x, y);
    while (y >= x){
      
        // check for decision parameter
        // and correspondingly 
        // update d, y
        if (d > 0) {
            y--; 
            d = d + 4 * (x - y) + 10;
        }
        else
            d = d + 4 * x + 6;

        // Increment x after updating decision parameter
        x++;
        
        // Draw the circle using the new coordinates
        draw_circle(xc, yc, x, y);
        msec_delay(50);
    }
}
