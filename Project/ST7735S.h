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


#ifndef __ST7735S_H__
#define __ST7735S_H__

#define LP_SPI_CLK_PORT                                             (GPIO_PORTB)
#define LP_SPI_CLK_MASK                                                 (1 << 9)
#define LP_SPI_CLK_IOMUX                                         (IOMUX_PINCM26)
#define LP_SPI_CLK_PFMODE                                                    (3)
#define LP_SPI_MOSI_PORT                                            (GPIO_PORTB)
#define LP_SPI_MOSI_MASK                                                (1 << 8)
#define LP_SPI_MOSI_IOMUX                                        (IOMUX_PINCM25)
#define LP_SPI_MOSI_PFMODE                                                   (3)
#define LP_SPI_MISO_PORT                                            (GPIO_PORTB)
#define LP_SPI_MISO_MASK                                                (1 << 7)
#define LP_SPI_MISO_IOMUX                                        (IOMUX_PINCM24)
#define LP_SPI_MISO_PFMODE                                                   (3)
#define LP_SPI_CS0_PORT                                             (GPIO_PORTB)
#define LP_SPI_CS0_MASK                                                 (1 << 6)
#define LP_SPI_CS0_IOMUX                                         (IOMUX_PINCM23)
#define LP_SPI_CS0_PFMODE                                                    (3)

#define ST7735S_REG_SEL_PORT                                        (GPIO_PORTA)
#define ST7735S_REG_SEL_MASK                                           (1 << 13)
#define ST7735S_REG_SEL_IOMUX                                    (IOMUX_PINCM35)

#define ST7735S_RESET_PORT                                          (GPIO_PORTB)
#define ST7735S_RESET_MASK                                             (1 << 15)
#define ST7735S_RESET_IOMUX                                      (IOMUX_PINCM32)

#define SPI_CTL1_CDMODE_1BYTE                         (1 << SPI_CTL1_CDMODE_OFS) 

#define PINCM_GPIO_PIN_FUNC                              ((uint32_t)0x00000001U)

// System Function Command List
#define LCD_NOP_CMD                                                       (0x00)
#define LCD_SWRESET_CMD                                                   (0x01)
#define LCD_SLPIN_CMD                                                     (0x10)
#define LCD_SLPOUT_CMD                                                    (0x11)
#define LCD_PTLON_CMD                                                     (0x12)
#define LCD_NORON_CMD                                                     (0x13)
#define LCD_INVOFF_CMD                                                    (0x20)
#define LCD_INVON_CMD                                                     (0x21)
#define LCD_GAMSET_CMD                                                    (0x26)
#define LCD_DISPOFF_CMD                                                   (0x28)
#define LCD_DISPON_CMD                                                    (0x29)
#define LCD_CASET_CMD                                                     (0x2A)
#define LCD_RASET_CMD                                                     (0x2B)
#define LCD_RAMWR_CMD                                                     (0x2C)
#define LCD_RGBSET_CMD                                                    (0x2D)
#define LCD_PTLAR_CMD                                                     (0x30)
#define LCD_SCRLAR_CMD                                                    (0x33)
#define LCD_TEOFF_CMD                                                     (0x34)
#define LCD_TEON_CMD                                                      (0x35)
#define LCD_MADCTL_CMD                                                    (0x36)
#define LCD_VSCSAD_CMD                                                    (0x37)
#define LCD_IDMOFF_CMD                                                    (0x38)
#define LCD_IDMON_CMD                                                     (0x39)
#define LCD_COLMOD_CMD                                                    (0x3A)

#define LCD_FRMCTR1_CMD                                                   (0xB1)
#define LCD_FRMCTR2_CMD                                                   (0xB2)
#define LCD_FRMCTR3_CMD                                                   (0xB3)
#define LCD_INVCTR_CMD                                                    (0xB4)

#define LCD_PWCTR1_CMD                                                    (0xC0)
#define LCD_PWCTR2_CMD                                                    (0xC1)
#define LCD_PWCTR3_CMD                                                    (0xC2)
#define LCD_PWCTR4_CMD                                                    (0xC3)
#define LCD_PWCTR5_CMD                                                    (0xC4)

#define LCD_VMCTR1_CMD                                                    (0xC5)

#define LCD_IFPF_12bit                                                    (0x03)
#define LCD_IFPF_16bit                                                    (0x05)
#define LCD_IFPF_18bit                                                    (0x06)

#define LCD_MADCTL_MY_MASK                                                (0x80)
#define LCD_MADCTL_MX_MASK                                                (0x40)
#define LCD_MADCTL_MV_MASK                                                (0x20)
#define LCD_MADCTL_ML_MASK                                                (0x10)
#define LCD_MADCTL_RGB_MASK                                               (0x08)
#define LCD_MADCTL_MH_MASK                                                (0x04)

#define LCD_GAMMA_CUREVE_1                                                (0x01)
#define LCD_GAMMA_CUREVE_2                                                (0x02)
#define LCD_GAMMA_CUREVE_3                                                (0x04)
#define LCD_GAMMA_CUREVE_4                                                (0x08)

#define LCD_WIDTH                                                            128
#define LCD_HEIGHT                                                           128
#define UINT5_MAX                                                             31
#define UINT6_MAX                                                             63

typedef struct{
  union {
    struct {
      uint16_t r:5;
      uint16_t g:6;
      uint16_t b:5;
    };

    uint8_t packet[2];
  };
} color565_t;

// ----------------------------------------------------------------------------
// Prototype for support functions
// ----------------------------------------------------------------------------
void ST7735S_init(void);
void ST7735S_write_command(uint8_t data);
void ST7735S_write_data(uint8_t data);
void ST7735S_write_color(color565_t color);

void ST7735S_set_addr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

color565_t RGB_to_color(uint8_t r, uint8_t g, uint8_t b);

void draw_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, color565_t color);

#endif /* __SPI_H__ */
