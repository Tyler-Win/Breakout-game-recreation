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
#include "ti/devices/msp/peripherals/hw_iomux.h"
#include "ti/devices/msp/peripherals/hw_spi.h"
#include "ST7735S.h"


void send_init_commands(void)
{
  //Pulse reset
  GPIOB->DOUT31_0 &= (~ST7735S_RESET_MASK);
  msec_delay(50);
  GPIOB->DOUT31_0 |= ST7735S_RESET_MASK;
  msec_delay(150);

  ST7735S_write_command(LCD_SWRESET_CMD);
  msec_delay(200);

  ST7735S_write_command(LCD_SLPOUT_CMD);
  msec_delay(200);

  ST7735S_write_command(LCD_COLMOD_CMD);
  ST7735S_write_data(LCD_IFPF_16bit);
  msec_delay(10);

  ST7735S_write_command(LCD_MADCTL_CMD);
  ST7735S_write_data(LCD_MADCTL_MY_MASK | LCD_MADCTL_MX_MASK);

  ST7735S_set_addr(0, 0, LCD_WIDTH, LCD_HEIGHT);
  ST7735S_write_command(LCD_RAMWR_CMD);
  for (uint16_t i = 0; i < (LCD_WIDTH * LCD_HEIGHT); i++)
  {
    ST7735S_write_color(RGB_to_color(0xFF, 0xFF, 0xFF));
  }

  ST7735S_write_command(LCD_IDMOFF_CMD);
  ST7735S_write_command(LCD_NORON_CMD);

  ST7735S_write_command(LCD_DISPON_CMD);
  msec_delay(500);
}

void ST7735S_init(void)
{
    // Reset SPI1
  SPI1->GPRCM.RSTCTL = (GPTIMER_RSTCTL_KEY_UNLOCK_W | 
					    GPTIMER_RSTCTL_RESETSTKYCLR_CLR |
					    GPTIMER_RSTCTL_RESETASSERT_ASSERT);

  // Enable power to SPI1
  SPI1->GPRCM.PWREN = (GPTIMER_PWREN_KEY_UNLOCK_W | 
                       GPTIMER_PWREN_ENABLE_ENABLE);

  clock_delay(24);
  
  IOMUX->SECCFG.PINCM[LP_SPI_CLK_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                      LP_SPI_CLK_PFMODE);

  IOMUX->SECCFG.PINCM[LP_SPI_MOSI_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                      LP_SPI_MOSI_PFMODE);

  IOMUX->SECCFG.PINCM[LP_SPI_MISO_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                     IOMUX_PINCM_INENA_ENABLE | LP_SPI_MISO_PFMODE);

  IOMUX->SECCFG.PINCM[LP_SPI_CS0_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                      LP_SPI_CS0_PFMODE);

  // Select BusClk (SysClk) source for SPI module
  SPI1->CLKSEL = (SPI_CLKSEL_SYSCLK_SEL_ENABLE | SPI_CLKSEL_MFCLK_SEL_DISABLE |
                  SPI_CLKSEL_LFCLK_SEL_DISABLE);

  // Set clock division
  SPI1->CLKDIV = SPI_CLKDIV_RATIO_DIV_BY_1;

  #define PD0_CPUCLK_CLKDIV   2     // PD0 BUSCLK is half of CPUCLK
  #define PD1_CPUCLK_CLKDIV   1     // PD1 BUSCLK is same as CPUCLK

  // Both SPI modules are on PD1 
  uint32_t bus_clock = get_bus_clock_freq() / PD1_CPUCLK_CLKDIV;

  // Set clock prescaler to get final SPI clock frequency
  // SPIClk = (BusClock / (CLKDIV * (SCR+1)*2) = 40MHz/(8 * (1+1) * 2) = 1.25MHz
  
  SPI1->CLKCTL = SPI_CLKCTL_SCR_MINIMUM;
   
  

  // Configure SPI control register 0

  SPI1->CTL0 = (SPI_CTL0_CSCLR_DISABLE | SPI_CTL0_CSSEL_CSSEL_0 | 
                SPI_CTL0_SPH_FIRST | SPI_CTL0_SPO_LOW | 
                SPI_CTL0_PACKEN_DISABLED | SPI_CTL0_FRF_MOTOROLA_4WIRE | 
                SPI_CTL0_DSS_DSS_8);

  // Configure SPI control register 1
  SPI1->CTL1 = (SPI_CTL1_RXTIMEOUT_MINIMUM | SPI_CTL1_REPEATTX_DISABLE |
                SPI_CTL1_CDMODE_MINIMUM | SPI_CTL1_CDENABLE_DISABLE |
                SPI_CTL1_PTEN_DISABLE | SPI_CTL1_PES_DISABLE | 
                SPI_CTL1_PREN_DISABLE | SPI_CTL1_MSB_ENABLE |
                SPI_CTL1_POD_DISABLE | SPI_CTL1_CP_ENABLE | 
                SPI_CTL1_LBM_DISABLE | SPI_CTL1_ENABLE_ENABLE);

  //Setup Reset Pin as input
  IOMUX->SECCFG.PINCM[ST7735S_RESET_IOMUX] = (IOMUX_PINCM_PC_CONNECTED | 
                      PINCM_GPIO_PIN_FUNC);
  GPIOB->DOE31_0 |= ST7735S_RESET_MASK;

  //Setup Register Select
  IOMUX->SECCFG.PINCM[ST7735S_REG_SEL_IOMUX] = (IOMUX_PINCM_PC_CONNECTED | 
                      PINCM_GPIO_PIN_FUNC | IOMUX_PINCM_INENA_ENABLE);
  GPIOA->DOE31_0 |= ST7735S_REG_SEL_MASK;

  send_init_commands();
}

void ST7735S_write_command(uint8_t data)
{
  while((SPI1->STAT & SPI_STAT_BUSY_MASK) == SPI_STAT_BUSY_ACTIVE); 
  GPIOA->DOUT31_0 &= ~ST7735S_REG_SEL_MASK;

  SPI1->TXDATA = data;

  while((SPI1->STAT & SPI_STAT_BUSY_MASK) == SPI_STAT_BUSY_ACTIVE); 
  GPIOA->DOUT31_0 |= ST7735S_REG_SEL_MASK;
}

void ST7735S_write_data(uint8_t data)
{
  // Wait here until TX FIFO is not full
  while((SPI1->STAT & SPI_STAT_TNF_MASK) == SPI_STAT_TNF_FULL); 
  
  SPI1->TXDATA = data;
}

void ST7735S_write_color(color565_t color)
{
  ST7735S_write_data(color.packet[1]);
  ST7735S_write_data(color.packet[0]);
}

void ST7735S_set_addr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) 
{
  ST7735S_write_command(LCD_CASET_CMD);
  ST7735S_write_data(0x00);
  ST7735S_write_data(0x02 + x0);
  ST7735S_write_data(0x00);
  ST7735S_write_data(0x01 + x1);

  ST7735S_write_command(LCD_RASET_CMD);
  ST7735S_write_data(0x00);
  ST7735S_write_data(0x03 + y0); 
  ST7735S_write_data(0x00);
  ST7735S_write_data(0x02 + y1);
}

color565_t RGB_to_color(uint8_t r, uint8_t g, uint8_t b) 
{
   return (color565_t){ .r = ((float)r / (float)UINT8_MAX) * UINT5_MAX, 
                       .g = ((float)g / (float)UINT8_MAX) * UINT6_MAX, 
                       .b = ((float)b / (float)UINT8_MAX) * UINT5_MAX};

}
