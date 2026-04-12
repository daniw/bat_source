#include "lcd.h"
#include "string.h"
#include <stdio.h>

/* Arg count, CMD, Args if any */
#if defined USE_ST7735
const uint8_t init_cmd[] = {
    0,  CMD_SLPOUT,
//  3,  CMD_FRMCTR1, 0x01, 0x2C, 0x2D,                     // Standard frame rate
//  3,  CMD_FRMCTR2, 0x01, 0x2C, 0x2D,                     // Standard frame rate
//  6,  CMD_FRMCTR3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,   // Standard frame rate
    3,  CMD_FRMCTR1, 0x01, 0x01, 0x01,                     // Max
    3,  CMD_FRMCTR2, 0x01, 0x01, 0x01,                     // Max
    6,  CMD_FRMCTR3, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,   // Max frame rate
    1,  CMD_INVCTR,  0x07,
    3,  CMD_PWCTR1,  0xA2, 0x02, 0x84,
    1,  CMD_PWCTR2,  0xC5,
    2,  CMD_PWCTR3,  0x0A, 0x00,
    2,  CMD_PWCTR4,  0x8A, 0x2A,
    2,  CMD_PWCTR5,  0x8A, 0xEE,
    1,  CMD_VMCTR1,  0x0E,
    1,  CMD_INVOFF,  0x00,
    1,  CMD_COLMOD,  0x05,
    2,  CMD_CASET,   0x00, LCD_WIDTH-1,
    2,  CMD_RASET,   0x00, LCD_HEIGHT-1,
    1,  CMD_MADCTL,  LCD_ROTATION_CMD,
    16, CMD_GMCTRP1, 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10,
    16, CMD_GMCTRN1, 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,
    0,  CMD_NORON,
};
#elif defined USE_ST7789
#ifdef LCD_DC
const uint8_t init_cmd[] = {
#else
const uint16_t init_cmd[] = {
#endif
    0,  DC_BIT_CMD | CMD_SLPOUT,
    1,  DC_BIT_CMD | CMD_COLMOD,  DC_BIT_DATA | CMD_COLOR_MODE_16bit,
    5,  DC_BIT_CMD | CMD_PORCTRL, DC_BIT_DATA | 0x0C, DC_BIT_DATA | 0x0C, DC_BIT_DATA | 0x00, DC_BIT_DATA | 0x33, DC_BIT_DATA | 0x33,   // Standard porch
  //5,  DC_BIT_CMD | CMD_PORCTRL, DC_BIT_DATA | 0x01, 0x01, 0x00, 0x11, 0x11,   // Minimum porch (7% faster screen refresh rate)
    1,  DC_BIT_CMD | CMD_GCTRL,   DC_BIT_DATA | 0x35,                           // Gate Control, Default value
    1,  DC_BIT_CMD | CMD_VCOMS,   DC_BIT_DATA | 0x19,                           // VCOM setting 0.725v (default 0.75v for 0x20)
    1,  DC_BIT_CMD | CMD_LCMCTRL, DC_BIT_DATA | 0X2C,                           // LCMCTRL, Default value
    1,  DC_BIT_CMD | CMD_VDVVRHEN,DC_BIT_DATA | 0x01,                           // VDV and VRH command Enable, Default value
    1,  DC_BIT_CMD | CMD_VRHS,    DC_BIT_DATA | 0x12,                           // VRH set, +-4.45v (default +-4.1v for 0x0B)
    1,  DC_BIT_CMD | CMD_VDVS,    DC_BIT_DATA | 0x20,                           // VDV set, Default value
    1,  DC_BIT_CMD | CMD_FRCTRL2, DC_BIT_DATA | 0x0F,                           // Frame rate control in normal mode, Default refresh rate (60Hz)
  //1,  DC_BIT_CMD | CMD_FRCTRL2, DC_BIT_DATA | 0x01,                           // Frame rate control in normal mode, Max refresh rate (111Hz)
    2,  DC_BIT_CMD | CMD_PWCTRL1, DC_BIT_DATA | 0xA4, DC_BIT_DATA | 0xA1,
    1,  DC_BIT_CMD | CMD_MADCTL,  DC_BIT_DATA | LCD_ROTATION_CMD,
    14, DC_BIT_CMD | CMD_GMCTRP1, DC_BIT_DATA | 0xD0, DC_BIT_DATA | 0x04, DC_BIT_DATA | 0x0D, DC_BIT_DATA | 0x11, DC_BIT_DATA | 0x13, DC_BIT_DATA | 0x2B, DC_BIT_DATA | 0x3F, DC_BIT_DATA | 0x54, DC_BIT_DATA | 0x4C, DC_BIT_DATA | 0x18, DC_BIT_DATA | 0x0D, DC_BIT_DATA | 0x0B, DC_BIT_DATA | 0x1F, DC_BIT_DATA | 0x23,
    14, DC_BIT_CMD | CMD_GMCTRN1, DC_BIT_DATA | 0xD0, DC_BIT_DATA | 0x04, DC_BIT_DATA | 0x0C, DC_BIT_DATA | 0x11, DC_BIT_DATA | 0x13, DC_BIT_DATA | 0x2C, DC_BIT_DATA | 0x3F, DC_BIT_DATA | 0x44, DC_BIT_DATA | 0x51, DC_BIT_DATA | 0x2F, DC_BIT_DATA | 0x1F, DC_BIT_DATA | 0x1F, DC_BIT_DATA | 0x20, DC_BIT_DATA | 0x23,
    0,  DC_BIT_CMD | CMD_INVON,
    0,  DC_BIT_CMD | CMD_NORON
};
#endif


char str[32];

static void LCD_Update(void);
typedef struct{
  int8_t spi_sz;
  int8_t dma_mem_inc;
}config_t;

config_t config = {
    .spi_sz = -1,
    .dma_mem_inc = -1,
};

#ifdef LCD_LOCAL_FB
static uint16_t fb[LCD_WIDTH*LCD_HEIGHT];
#endif

static UG_GUI gui;
static UG_DEVICE device = {
    .x_dim = LCD_WIDTH,
    .y_dim = LCD_HEIGHT,
#ifdef LCD_LOCAL_FB
    .pset = LCD_DrawPixelFB,
#else
    .pset = LCD_DrawPixel,
#endif
    .flush = LCD_Update,
};

#define mode_16bit    1
#define mode_8bit     0
/*
 * @brief Sets SPI interface word size (0=8bit, 1=16 bit)
 * @param none
 * @return none
 */

static void setSPI_Size(int8_t size){
  if(config.spi_sz!=size){
    __HAL_SPI_DISABLE(&LCD_HANDLE);
    config.spi_sz=size;
    if(size==mode_16bit){
      LCD_HANDLE.Init.DataSize = SPI_DATASIZE_16BIT;
      //LCD_HANDLE.Instance->CR1 |= SPI_CR1_;
      LCD_HANDLE.Instance->CR2 &= ~( SPI_CR2_DS_Msk);
      LCD_HANDLE.Instance->CR2 |= (LCD_HANDLE.Init.DataSize & SPI_CR2_DS_Msk);

    }
    else{
      LCD_HANDLE.Init.DataSize = SPI_DATASIZE_8BIT;
      //LCD_HANDLE.Instance->CR1 &= ~(SPI_CR1_DFF);
      LCD_HANDLE.Instance->CR2 &= ~( SPI_CR2_DS_Msk);
      LCD_HANDLE.Instance->CR2 |= (LCD_HANDLE.Init.DataSize & SPI_CR2_DS_Msk);
    }
    __HAL_SPI_ENABLE(&LCD_HANDLE);
  }

}

#define DMA_Min_Pixels    32             // Don't use DMA for small transfers? Setting this to 1 will always use DMA

#ifdef USE_DMA
#define mem_increase      1
#define mem_fixed         0

/**
 * @brief Configures DMA/ SPI interface
 * @param memInc Enable/disable memory address increase
 * @param mode16 Enable/disable 16 bit mode (disabled = 8 bit)
 * @return none
 */
static void setDMAMemMode(uint8_t memInc, uint8_t size)
{
  setSPI_Size(size);
  if(config.dma_sz!=size || config.dma_mem_inc!=memInc){
    config.dma_sz =size;
    config.dma_mem_inc = memInc;
    __HAL_DMA_DISABLE(LCD_HANDLE.hdmatx);;
#ifdef DMA_SxCR_EN
    while((LCD_HANDLE.hdmatx->Instance->CR & DMA_SxCR_EN) != RESET);
#elif defined DMA_CCR_EN
    while((LCD_HANDLE.hdmatx->Instance->CCR & DMA_CCR_EN) != RESET);
#endif
    if(memInc==mem_increase){
      LCD_HANDLE.hdmatx->Init.MemInc = DMA_MINC_ENABLE;
#ifdef DMA_SxCR_EN
      LCD_HANDLE.hdmatx->Instance->CR |= DMA_SxCR_MINC;
#elif defined DMA_CCR_EN
      LCD_HANDLE.hdmatx->Instance->CCR |= DMA_CCR_MINC;
#endif
    }
    else{
      LCD_HANDLE.hdmatx->Init.MemInc = DMA_MINC_DISABLE;
#ifdef DMA_SxCR_EN
      LCD_HANDLE.hdmatx->Instance->CR &= ~(DMA_SxCR_MINC);
#elif defined DMA_CCR_EN
      LCD_HANDLE.hdmatx->Instance->CCR &= ~(DMA_CCR_MINC);
#endif
    }

    if(size==mode_16bit){
      LCD_HANDLE.hdmatx->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      LCD_HANDLE.hdmatx->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
#ifdef DMA_SxCR_EN
      LCD_HANDLE.hdmatx->Instance->CR = (LCD_HANDLE.hdmatx->Instance->CR & ~(DMA_SxCR_PSIZE_Msk | DMA_SxCR_MSIZE_Msk)) |
                                                   (1<<DMA_SxCR_PSIZE_Pos | 1<<DMA_SxCR_MSIZE_Pos);
#elif defined DMA_CCR_EN
      LCD_HANDLE.hdmatx->Instance->CCR = (LCD_HANDLE.hdmatx->Instance->CCR & ~(DMA_CCR_PSIZE_Msk | DMA_CCR_MSIZE_Msk)) |
                                                   (1<<DMA_CCR_PSIZE_Pos | 1<<DMA_CCR_MSIZE_Pos);
#endif

    }
    else{
      LCD_HANDLE.hdmatx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      LCD_HANDLE.hdmatx->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
#ifdef DMA_SxCR_EN
      LCD_HANDLE.hdmatx->Instance->CR = (LCD_HANDLE.hdmatx->Instance->CR & ~(DMA_SxCR_PSIZE_Msk | DMA_SxCR_MSIZE_Msk));
#elif defined DMA_CCR_EN
      LCD_HANDLE.hdmatx->Instance->CCR = (LCD_HANDLE.hdmatx->Instance->CCR & ~(DMA_CCR_PSIZE_Msk | DMA_CCR_MSIZE_Msk));
#endif
    }
  }
}
#endif

/**
 * @brief Write command to ST7735 controller
 * @param cmd -> command to write
 * @return none
 */
static void LCD_WriteCommand(uint8_t *cmd, uint8_t argc)
{
  setSPI_Size(mode_8bit);
#ifdef LCD_DC
  LCD_PIN(LCD_DC,RESET);

#ifdef LCD_CS
  LCD_PIN(LCD_CS,RESET);
#endif
  HAL_SPI_Transmit(&LCD_HANDLE, cmd, 1, HAL_MAX_DELAY);
  if(argc){
    LCD_PIN(LCD_DC,SET);
    HAL_SPI_Transmit(&LCD_HANDLE, (cmd+1), argc, HAL_MAX_DELAY);
  }
#ifdef LCD_CS
  LCD_PIN(LCD_CS,SET);
#endif
#else
#ifdef LCD_CS
  LCD_PIN(LCD_CS,RESET);
#endif
  HAL_SPI_Transmit(&LCD_HANDLE, (cmd), argc/2, HAL_MAX_DELAY);
#ifdef LCD_CS
  LCD_PIN(LCD_CS,SET);
#endif
#endif
}

/**
 * @brief Write data to ST7735 controller
 * @param buff -> pointer of data buffer
 * @param buff_size -> size of the data buffer in pixels (meaning 1 pixel =1)!!
 * @return none
 */
static void LCD_WriteData(uint8_t *buff, size_t buff_size) {
#ifdef LCD_DC
  LCD_PIN(LCD_DC,SET);
#endif
#ifdef LCD_CS
	LCD_PIN(LCD_CS, RESET);
#endif

  // split data in small chunks because HAL can't send more than 64K at once

  while (buff_size > 0) {
    uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;
#ifdef USE_DMA
    if(buff_size>DMA_Min_Pixels){
      HAL_SPI_Transmit_DMA(&LCD_HANDLE, buff, chunk_size);
      while(HAL_DMA_GetState(LCD_HANDLE.hdmatx)!=HAL_DMA_STATE_READY);
      if(config.dma_mem_inc==mem_increase){
        if(config.dma_sz==mode_16bit)
          buff += chunk_size;
        else
          buff += chunk_size*2;
      }
    }
    else{
      HAL_SPI_Transmit(&LCD_HANDLE, buff, chunk_size, HAL_MAX_DELAY);
      if(config.spi_sz==mode_16bit)
        buff += chunk_size;
      else
        buff += chunk_size*2;
    }
#else
    HAL_SPI_Transmit(&LCD_HANDLE, buff, chunk_size, HAL_MAX_DELAY);
#endif
    buff_size -= chunk_size;
  }
#ifdef LCD_CS
	LCD_PIN(LCD_CS, SET);
#endif
}

/**
 * @brief Write data to ST7735 controller, simplify for 8bit data.
 * data -> data to write
 * @return none
 */

static void LCD_ReadCmd(uint8_t cmd, uint8_t *data, uint8_t count)
{
  setSPI_Size(mode_8bit);
  //LCD_PIN(LCD_CS,RESET);
  LCD_PIN(LCD_DC,RESET);
  HAL_SPI_Transmit(&LCD_HANDLE, &cmd, sizeof(cmd), HAL_MAX_DELAY);
  LCD_PIN(LCD_DC,SET);
  HAL_SPI_Receive(&LCD_HANDLE, data, count, HAL_MAX_DELAY);
  //LCD_PIN(LCD_CS,SET);
}

/**
 * @brief Set the rotation direction of the display
 * @param m -> rotation parameter(please refer it in ST7735.h)
 * @return none
 */
void LCD_SetRotation(uint8_t m)
{
#ifdef LCD_DC
  uint8_t cmd[] = { CMD_MADCTL, 0};
#else
  uint16_t cmd[] = {DC_BIT_CMD | CMD_MADCTL, 0};
#endif
  m = m % 4; // can't be higher than 3

  switch (m)
  {
  case 0:
#ifdef LCD_160X80
    cmd[1] = DC_BIT_DATA | CMD_MADCTL_MX | CMD_MADCTL_MY | CMD_MADCTL_BGR;
#else
    cmd[1] = DC_BIT_DATA | CMD_MADCTL_MX | CMD_MADCTL_MY | CMD_MADCTL_RGB;
#endif
    break;
  case 1:
#if LCD_160X80
    cmd[1] = DC_BIT_DATA | CMD_MADCTL_MY | CMD_MADCTL_MV | CMD_MADCTL_BGR;
#else
    cmd[1] = DC_BIT_DATA | CMD_MADCTL_MY | CMD_MADCTL_MV | CMD_MADCTL_RGB;
#endif
    break;
  case 2:
#if LCD_160X80
    cmd[1] = DC_BIT_DATA | CMD_MADCTL_BGR;
#else
    cmd[1] = DC_BIT_DATA | CMD_MADCTL_RGB;
#endif
    break;
  case 3:
#if LCD_160X80
    cmd[1] = DC_BIT_DATA | CMD_MADCTL_MX | CMD_MADCTL_MV | CMD_MADCTL_BGR;
#else
    cmd[1] = DC_BIT_DATA | CMD_MADCTL_MX | CMD_MADCTL_MV | CMD_MADCTL_RGB;
#endif
    break;
  }
  LCD_WriteCommand(cmd, sizeof(cmd));
}


/**
 * @brief Set address of DisplayWindow
 * @param xi&yi -> coordinates of window
 * @return none
 */
static void LCD_SetAddressWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
	int16_t x_start = x0 + LCD_X_SHIFT, x_end = x1 + LCD_X_SHIFT;
	int16_t y_start = y0 + LCD_Y_SHIFT, y_end = y1 + LCD_Y_SHIFT;

	/* Column Address set */
	{
#ifdef LCD_DC
    uint8_t cmd[] = { CMD_CASET, x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF };
#else
		uint16_t cmd[] = { DC_BIT_CMD | CMD_CASET, DC_BIT_DATA | (x_start >> 8), DC_BIT_DATA | (x_start & 0xFF), DC_BIT_DATA | (x_end >> 8), DC_BIT_DATA
				| (x_end & 0xFF) };
#endif
		LCD_WriteCommand(cmd, sizeof(cmd));
	}
	/* Row Address set */
	{
#ifdef LCD_DC
	    uint8_t cmd[] = { CMD_RASET, y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF };
#else
		uint16_t cmd[] = { DC_BIT_CMD | CMD_RASET, DC_BIT_DATA | (y_start >> 8), DC_BIT_DATA | (y_start & 0xFF), DC_BIT_DATA | (y_end >> 8), DC_BIT_DATA
				| (y_end & 0xFF) };
#endif
		LCD_WriteCommand(cmd, sizeof(cmd));
	}
	{
		/* Write to RAM */
#ifdef LCD_DC
	    uint8_t cmd[] = { CMD_RAMWR };
#else
		uint16_t cmd[] = { DC_BIT_CMD | CMD_RAMWR };
#endif
		LCD_WriteCommand(cmd, sizeof(cmd));
	}
}


/**
 * @brief Address and draw a Pixel
 * @param x&y -> coordinate to Draw
 * @param color -> color of the Pixel
 * @return none
 */
void LCD_DrawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x < 0) || (x > LCD_WIDTH-1) ||
     (y < 0) || (y > LCD_HEIGHT-1))
    return;

  LCD_SetAddressWindow(x, y, x, y);

#ifdef LCD_DC
  uint8_t data[2] = {( color >> 8), (color & 0xFF)};
  LCD_PIN(LCD_DC,SET);
#else
  uint16_t data[2] = {DC_BIT_DATA|( color >> 8), DC_BIT_DATA|(color & 0xFF)};
#endif

#ifdef LCD_CS
  LCD_PIN(LCD_CS,RESET);
#endif
  HAL_SPI_Transmit(&LCD_HANDLE, data, sizeof(data), HAL_MAX_DELAY);
#ifdef LCD_CS
  LCD_PIN(LCD_CS,SET);
#endif
}

#ifdef LCD_LOCAL_FB
void LCD_DrawPixelFB(int16_t x, int16_t y, uint16_t color)
{
  if ((x < 0) || (x >= LCD_WIDTH) ||
     (y < 0) || (y >= LCD_HEIGHT)) return;

  fb[x+(y*LCD_WIDTH)] = color;
}
#endif

void LCD_FillPixels(uint32_t pixels, uint16_t color){
#ifdef USE_DMA
#ifndef LCD_DC
  uint16_t fill_color [2]= {(DC_BIT_DATA|(color>>8)),(DC_BIT_DATA|(color& 0xFF ))};
  if(pixels>DMA_Min_Pixels)
    LCD_WriteData((uint8_t*)&fill_color, pixels);
#else
  if(pixels>DMA_Min_Pixels)
    LCD_WriteData((uint8_t*)&color, pixels);
#endif
  else{
#endif

#ifdef LCD_DC
	    uint16_t fill[DMA_Min_Pixels];                                                                // Use a pixel buffer for faster filling, removes overhead.
	    for(uint32_t t=0;t<(pixels<DMA_Min_Pixels ? pixels : DMA_Min_Pixels);t++){                     // Fill the buffer with the color
    	fill[t] = color;
#else
	 uint16_t fill[2*DMA_Min_Pixels];                                                                // Use a pixel buffer for faster filling, removes overhead.
	 for(uint32_t t=0;t<(pixels<DMA_Min_Pixels ? pixels : DMA_Min_Pixels);t++){                     // Fill the buffer with the color
	    fill[2*t]   = (DC_BIT_DATA|(color>>8));
	    fill[2*t+1] = (DC_BIT_DATA|(color& 0xFF ));
#endif
    }
	// ToDo: Doublecheck this here
    while(pixels){                                                                                // Send 64 pixel blocks
      uint32_t sz = (pixels<DMA_Min_Pixels ? pixels : DMA_Min_Pixels);
      LCD_WriteData((uint8_t*)fill, sz);
      pixels-=sz;
    }
#ifdef USE_DMA
  }
#endif
}

/**
 * @brief Set address of DisplayWindow and returns raw pixel draw for uGUI driver acceleration
 * @param xi&yi -> coordinates of window
 * @return none
 */
void(*LCD_FillArea(int16_t x0, int16_t y0, int16_t x1, int16_t y1))(uint32_t, uint16_t){
  if(x0==-1){ // ToDo: What is this for?
#ifdef USE_DMA
    setDMAMemMode(mem_increase, mode_8bit);
#else
    setSPI_Size(mode_8bit);                                                          // Set SPI to 8 bit
#endif
    return NULL;
  }
  LCD_SetAddressWindow(x0,y0,x1,y1);
#ifdef USE_DMA
    setDMAMemMode(mem_fixed, mode_16bit);
#else
    setSPI_Size(mode_16bit);                                                          // Set SPI to 16 bit
#endif
#ifdef LCD_DC
  LCD_PIN(LCD_DC,SET);
#endif
  return LCD_FillPixels;
}


/**
 * @brief Fill an Area with single color
 * @param xSta&ySta -> coordinate of the start point
 * @param xEnd&yEnd -> coordinate of the end point
 * @param color -> color to Fill with
 * @return none
 */
int8_t LCD_Fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
  uint32_t pixels = (uint32_t)(xEnd-xSta+1)*(yEnd-ySta+1);
  LCD_SetAddressWindow(xSta, ySta, xEnd, yEnd);
#ifdef USE_DMA
    setDMAMemMode(mem_fixed, mode_14bit);
#else
    setSPI_Size(mode_16bit);
#endif
  LCD_FillPixels(pixels, color);
#ifdef USE_DMA
  setDMAMemMode(mem_increase, mode_14bit);
#else
  setSPI_Size(mode_8bit);
#endif
  return UG_RESULT_OK;
}


/**
 * @brief Draw an Image on the screen
 * @param x&y -> start point of the Image
 * @param w&h -> width & height of the Image to Draw
 * @param data -> pointer of the Image array
 * @return none
 */
void LCD_DrawImage(uint16_t x, uint16_t y, UG_BMP* bmp)
{
	return;
  uint16_t w = bmp->width;
  uint16_t h = bmp->height;
  if ((x > LCD_WIDTH-1) || (y > LCD_HEIGHT-1))
    return;
  if ((x + w - 1) > LCD_WIDTH-1)
    return;
  if ((y + h - 1) > LCD_HEIGHT-1)
    return;
  if(bmp->bpp!=BMP_BPP_16)
    return;
  LCD_SetAddressWindow(x, y, x + w - 1, y + h - 1);

  #ifdef USE_DMA
  setDMAMemMode(mem_increase, mode_14bit);                                                            // Set SPI and DMA to 16 bit, enable memory increase
  #else
  setSPI_Size(mode_16bit);                                                                            // Set SPI to 16 bit
  #endif
  LCD_WriteData((uint8_t*)bmp->p, w*h);
#ifdef USE_DMA
  setDMAMemMode(mem_increase, mode_14bit);                                                            // Set SPI and DMA to 16 bit, enable memory increase
#else
	setSPI_Size(mode_8bit);                                                                            // Set SPI to 16 bit
#endif
  }

/**
 * @brief Accelerated line draw using filling (Only for vertical/horizontal lines)
 * @param x1&y1 -> coordinate of the start point
 * @param x2&y2 -> coordinate of the end point
 * @param color -> color of the line to Draw
 * @return none
 */
int8_t LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {

  if(x0==x1){                                   // If horizontal
    if(y0>y1) swap(y0,y1);
  }
  else if(y0==y1){                              // If vertical
    if(x0>x1) swap(x0,x1);
  }
  else{                                         // Else, return fail, draw using software
    return UG_RESULT_FAIL;
  }

  LCD_Fill(x0,y0,x1,y1,color);               // Draw using acceleration
  return UG_RESULT_OK;
}
void LCD_PutChar(uint16_t x, uint16_t y, char ch, UG_FONT* font, uint16_t color, uint16_t bgcolor){
  UG_FontSelect(font);
  UG_PutChar(ch, x, y, color, bgcolor);
}

void LCD_PutStr(uint16_t x, uint16_t y,  char *str, UG_FONT* font, uint16_t color, uint16_t bgcolor){
  UG_FontSelect(font);
  UG_SetForecolor(color);
  UG_SetBackcolor(bgcolor);
  UG_PutString(x, y, str);
}
/**
 * @brief Invert Fullscreen color
 * @param invert -> Whether to invert
 * @return none
 */
void LCD_InvertColors(uint8_t invert)
{
#ifdef LCD_DC
  uint8_t cmd[] = { (invert ? CMD_INVON /* INVON */ : CMD_INVOFF /* INVOFF */) };
#else
  uint16_t cmd[] = { DC_BIT_CMD | (invert ? CMD_INVON /* INVON */ : CMD_INVOFF /* INVOFF */) };
#endif
  LCD_WriteCommand(cmd, sizeof(cmd));
}

/*
 * @brief Open/Close tearing effect line
 * @param tear -> Whether to tear
 * @return none
 */
void LCD_TearEffect(uint8_t tear)
{
#ifdef LCD_DC
	  uint8_t cmd[] = { (tear ? 0x35 /* TEON */ : 0x34 /* TEOFF */) };
#else
	  uint16_t cmd[] = {  DC_BIT_CMD | (tear ? 0x35 /* TEON */ : 0x34 /* TEOFF */) };
#endif
  LCD_WriteCommand(cmd, sizeof(cmd));
}

void LCD_setPower(uint8_t power)
{
#ifdef LCD_DC
	  uint8_t cmd[] = { (power ? CMD_DISPON /* TEON */ : CMD_DISPOFF /* TEOFF */) };
#else
	  uint16_t cmd[] = {  DC_BIT_CMD | (power ? CMD_DISPON /* TEON */ : CMD_DISPOFF /* TEOFF */) };
#endif
  LCD_WriteCommand(cmd, sizeof(cmd));
}

static void LCD_Update(void)
{
#ifdef LCD_LOCAL_FB
  setSPI_Size(mode_8bit);
  LCD_SetAddressWindow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
  #ifdef USE_DMA
  setDMAMemMode(mem_increase, mode_16bit);                                                            // Set SPI and DMA to 16 bit, enable memory increase
  #else
  setSPI_Size(mode_16bit);                                                                            // Set SPI to 16 bit
  #endif
  LCD_WriteData((uint8_t*)fb, LCD_WIDTH*LCD_HEIGHT);
#endif
//  #ifdef USE_DMA
//  setDMAMemMode(mem_increase, mode_14bit);                                                            // Set SPI and DMA to 16 bit, enable memory increase
//  #else
//  setSPI_Size(mode_8bit);                                                                            // Set SPI to 16 bit
//  #endif
}
/**
 * @brief Initialize ST7735 controller
 * @param none
 * @return none
 */

void LCD_init(void)
{
	uint8_t read_init[76];
#ifdef LCD_CS
  LCD_PIN(LCD_CS,SET);
#endif
#ifdef LCD_RST
  LCD_PIN(LCD_RST,RESET);
  HAL_Delay(1);
  LCD_PIN(LCD_RST,SET);
  HAL_Delay(200);
#endif
  UG_Init(&gui, &device);
#ifndef LCD_LOCAL_FB
  UG_DriverRegister(DRIVER_DRAW_LINE, LCD_DrawLine);
  UG_DriverRegister(DRIVER_FILL_FRAME, LCD_Fill);
  UG_DriverRegister(DRIVER_FILL_AREA, LCD_FillArea);
  UG_DriverRegister(DRIVER_DRAW_BMP, LCD_DrawImage);
#endif
  UG_FontSetHSpace(0);
  UG_FontSetVSpace(0);
  LCD_ReadCmd(0x04, read_init, 5);
  
  for(uint16_t i=0; i<sizeof(init_cmd); ){
#ifdef LCD_DC
    LCD_WriteCommand((uint8_t*)&init_cmd[i+1], (init_cmd[i]));
#else
    LCD_WriteCommand((uint8_t*)&init_cmd[i+1], 2*(init_cmd[i]+1));
#endif
    //LCD_ReadCmd(init_cmd[i+1], &read_init[i+2], init_cmd[i]);
    i += init_cmd[i]+2;
  }
  UG_FillScreen(C_BLACK);               //  Clear screen
  LCD_setPower(ENABLE);
  UG_Update();
}


#define DEFAULT_FONT FONT_8X12

static uint32_t draw_time=0;
static void clearTime(void){
  draw_time=HAL_GetTick();
}
static void printTime(void){
  char str[8];
  uint8_t t = UG_FontGetTransparency();
  sprintf(str,"%lums",HAL_GetTick()-draw_time);
  UG_FontSelect(DEFAULT_FONT);
  UG_SetForecolor(C_YELLOW);
  UG_SetBackcolor(C_BLACK);
  UG_FontSetTransparency(0);
  UG_PutString(10, 15, str);
  UG_FontSetTransparency(t);
  UG_Update();
}
/**
 * @brief A Simple test function for ST7735
 * @param  none
 * @return  none
 */



#define MAX_OBJECTS 10
#define DEMO_FLASH_KB 256

static UG_WINDOW window_1;
static UG_BUTTON button_1;
static UG_TEXTBOX textbox_1;
static UG_OBJECT obj_buff_wnd_1[MAX_OBJECTS];
static UG_PROGRESS pgb;

void LCD_Test(void)
{

  int16_t x=40,y=40,rad=20,count=0;
  int8_t xadd=2,yadd=2,dstep=2;
  uint8_t r=0,g=0,b=0, t;
  uint32_t start, show;

  UG_FontSetTransparency(0);

  UG_FillScreen(C_BLACK);

  LCD_PutStr(50, 56, "STARTING TEST", DEFAULT_FONT, C_GREEN, C_RED);
  HAL_Delay(1500);

  show=start=HAL_GetTick();
  t = UG_FontGetTransparency();
  while(HAL_GetTick()-start<4000){
    UG_FillFrame(x-rad, y-rad, x+rad, y+rad, C_BLACK);
    x+=xadd;
    y+=yadd;
    if(x-rad<1){
      xadd=dstep;
      x+=xadd;
    }
    else if(x+rad>(LCD_WIDTH-1)){
      xadd=-dstep;
      x+=xadd;
    }

    if(y-rad<1){
      yadd=dstep;
      y+=yadd;
    }
    else if(y+rad>(LCD_HEIGHT-1)){
      yadd=-dstep;
      y+=yadd;
    }

    UG_FillCircle(x, y, rad, C_WHITE);
    UG_FontSetTransparency(0);
    LCD_PutStr(0, 0, str, DEFAULT_FONT, C_RED, C_BLACK);
    UG_FontSetTransparency(t);
    UG_Update();
    count++;
    if(HAL_GetTick()-show>999){
      show=HAL_GetTick();
      sprintf(str,"%dFPS",count);
      count=0;
    }
  }

  UG_FillScreen(C_RED);
  UG_Update();
  HAL_Delay(500);
  UG_FillScreen(C_GREEN);
  UG_Update();
  HAL_Delay(500);
  UG_FillScreen(C_BLUE);
  UG_Update();
  HAL_Delay(500);
  UG_FillScreen(C_BLACK);
  UG_Update();
  HAL_Delay(500);
  UG_FillScreen(C_WHITE);
  UG_Update();
  HAL_Delay(500);

  /*for(r=0; r<32;r++){                                       // R++, G=0, B=0
    UG_FillScreen((uint16_t)r<<11 | g<<5 | b);
    UG_Update();
  }
  r=31;
  for(g=0; g<64;g+=2){                                      // R=31, G++, B=0
    UG_FillScreen((uint16_t)r<<11 | g<<5 | b);
    UG_Update();
  }
  g=63;
  for(r=28; r;r--){                                         // R--, Gmax, B=0
    UG_FillScreen((uint16_t)r<<11 | g<<5 | b);
    UG_Update();
  }
  for(b=0; b<32;b++){                                       // R=0, Gmax, B++
    UG_FillScreen((uint16_t)r<<11 | g<<5 | b);
    UG_Update();
  }
  b=31;
  for(g=56; g;g-=2){                                        // R=0, G--, Bmax
    UG_FillScreen((uint16_t)r<<11 | g<<5 | b);
    UG_Update();
  }
  for(r=0; r<32;r++){                                       // R++, G=0, Bmax
    UG_FillScreen((uint16_t)r<<11 | g<<5 | b);
    UG_Update();
  }
  r=31;
  for(g=0; g<64;g+=2){                                      // Rmax, G++, Bmax
    UG_FillScreen((uint16_t)r<<11 | g<<5 | b);
    UG_Update();
  }

  return;*/


  clearTime();
  UG_FillScreen(C_WHITE);
  printTime();
  LCD_PutStr(10, 5, "Fill", DEFAULT_FONT, C_RED, C_WHITE);
  UG_Update();
  HAL_Delay(1000);

  UG_FillScreen(C_BLACK);
  LCD_PutStr(10, 5, "Font", DEFAULT_FONT, C_RED, C_BLACK);
  clearTime();
  LCD_PutStr(10, 35, "Hello Steve!", DEFAULT_FONT, C_CYAN, C_BLACK);
  LCD_PutStr(10, 55, "Hello Steve!", DEFAULT_FONT, C_ORANGE_RED, C_BLACK);
  LCD_PutStr(10, 75, "Hello Steve!", DEFAULT_FONT, C_LIME_GREEN, C_BLACK);
  LCD_PutStr(10, 95, "Hello Steve!", DEFAULT_FONT, C_HOT_PINK, C_BLACK);
  UG_Update();
  printTime();
  HAL_Delay(1000);

#if DEMO_FLASH_KB >=128
  UG_FillScreen(C_BLACK);
  LCD_PutStr(10, 5, "Big font", DEFAULT_FONT, C_RED, C_BLACK);
  clearTime();
  LCD_PutStr(10, 35, "Hello", FONT_arial_49X57, C_CYAN, C_BLACK);
  LCD_PutStr(10, 80, "Steve!", FONT_arial_49X57, C_CYAN, C_BLACK);
  UG_Update();
  printTime();
  HAL_Delay(1000);
#endif

  UG_FillScreen(C_RED);
  LCD_PutStr(10, 5, "Line", DEFAULT_FONT, C_YELLOW, C_RED);
  clearTime();
  UG_DrawLine(10, 30, 10, 100, C_WHITE);
  UG_DrawLine(10, 30, 100, 30, C_WHITE);
  UG_DrawLine(10, 30, 100, 100, C_WHITE);
  UG_Update();
  printTime();
  HAL_Delay(1000);

  UG_FillScreen(C_RED);
  LCD_PutStr(10, 5, "Rect", DEFAULT_FONT, C_YELLOW, C_RED);
  clearTime();
  UG_DrawFrame(10, 30, 100, 100, C_WHITE);
  UG_Update();
  printTime();
  HAL_Delay(1000);

  UG_FillScreen(C_RED);
  LCD_PutStr(10, 5, "Filled Rect", DEFAULT_FONT, C_YELLOW, C_RED);
  clearTime();
  UG_FillFrame(10, 30, 100, 100, C_WHITE);
  UG_Update();
  printTime();
  HAL_Delay(1000);

  UG_FillScreen(C_RED);
  LCD_PutStr(10, 5, "Mesh.", DEFAULT_FONT, C_YELLOW, C_RED);
  clearTime();
  UG_DrawMesh(10, 30, 100, 100, 5, C_WHITE);
  UG_Update();
  printTime();
  HAL_Delay(1000);

  UG_FillScreen(C_RED);
  LCD_PutStr(10, 5, "Circle", DEFAULT_FONT, C_YELLOW, C_RED);
  clearTime();
  UG_DrawCircle(45, 65, 30, C_WHITE);
  UG_Update();
  printTime();
  HAL_Delay(1000);

  UG_FillScreen(C_RED);
  LCD_PutStr(10, 5, "Filled Circle", DEFAULT_FONT, C_YELLOW, C_RED);
  UG_Update();
  clearTime();
  UG_FillCircle(45, 65, 30, C_WHITE);
  UG_Update();
  printTime();
  HAL_Delay(1000);

  UG_FillScreen(C_RED);
  LCD_PutStr(10, 5, "Triangle", DEFAULT_FONT, C_YELLOW, C_RED);
  clearTime();
  UG_DrawTriangle(10, 30, 100, 30, 50, 100, C_WHITE);
  UG_Update();
  printTime();
  HAL_Delay(1000);

  UG_FillScreen(C_RED);
  clearTime();
  LCD_PutStr(10, 5, "Filled Tri.", DEFAULT_FONT, C_YELLOW, C_RED);
  UG_FillTriangle(10, 30, 100, 30, 50, 100, C_WHITE);
  UG_Update();
  printTime();
  HAL_Delay(1000);

  clearTime();
  // Create the window
  //UG_WindowCreate(&window_1, obj_buff_wnd_1, MAX_OBJECTS, window_1_callback);
  // Window Title
  UG_WindowSetTitleText(&window_1, "Test Window");
  UG_WindowSetTitleTextFont(&window_1, DEFAULT_FONT);
  UG_WindowSetTitleHeight(&window_1, 15);
  UG_WindowSetXStart(&window_1, 0);
  UG_WindowSetYStart(&window_1, 0);
  UG_WindowSetXEnd(&window_1, LCD_WIDTH-1);
  UG_WindowSetYEnd(&window_1, LCD_HEIGHT-1);

  // Create Buttons
  UG_ButtonCreate(&window_1, &button_1, BTN_ID_0, 5, 5, 70, 35);
  //Label Buttons
  UG_ButtonSetFont(&window_1,BTN_ID_0,DEFAULT_FONT);
  UG_ButtonSetForeColor(&window_1,BTN_ID_0, C_BLACK);
  UG_ButtonSetBackColor(&window_1, BTN_ID_0, C_LIGHT_GRAY);
  UG_ButtonSetText(&window_1,BTN_ID_0,"3D Btn");

  // Create Textbox
  UG_TextboxCreate(&window_1, &textbox_1, TXB_ID_0, 10, 40, LCD_WIDTH-20, 65);
  UG_TextboxSetFont(&window_1, TXB_ID_0, DEFAULT_FONT);
  UG_TextboxSetText(&window_1, TXB_ID_0, "Some Text");
  UG_TextboxSetBackColor(&window_1, TXB_ID_0, C_LIGHT_YELLOW);
  UG_TextboxSetForeColor(&window_1, TXB_ID_0, C_BLACK);
  UG_TextboxSetAlignment(&window_1, TXB_ID_0, ALIGN_CENTER);

  // Create progress bar
  UG_ProgressCreate(&window_1, &pgb, PGB_ID_0, 10, 72, LCD_WIDTH-20, 85);
  UG_ProgressSetForeColor(&window_1, PGB_ID_0, C_ROYAL_BLUE);

  UG_WindowShow(&window_1);
  UG_Update();
  printTime();
  HAL_Delay(1000);
  uint32_t btn_time,progress_time,now;
  uint8_t i=0, j=0,u=0;
  btn_time = progress_time = HAL_GetTick();
  while(1){
    now=HAL_GetTick();
    if(now-btn_time>199){
      btn_time=now;
      u=1;
      i++;
      UG_TouchUpdate((i&1 ? 10 : -1), (i&1 ? 31 : -1), OBJ_TOUCH_STATE_CHANGED | (i&1 ? OBJ_TOUCH_STATE_IS_PRESSED : 0));
      if(i==9){
        UG_ButtonSetText(&window_1,BTN_ID_0,"2D Btn");
        UG_ButtonSetStyle(&window_1, BTN_ID_0, BTN_STYLE_2D);
      }
      if(i>19){
        break;
      }
    }
    if(now-progress_time>19){
      progress_time=now;
      u=1;
      if(++j>100)
        j=0;
      UG_ProgressSetProgress(&window_1, PGB_ID_0, j);
    }
    if(u){
      u=0;
      UG_Update();
    }
  }
  UG_WindowHide(&window_1);
  UG_WindowDelete(&window_1);
  UG_Update();
  t = UG_FontGetTransparency();
#if DEMO_FLASH_KB >=64
  UG_FillScreen(0x4b10);
  clearTime();
  UG_DrawBMP((LCD_WIDTH-fry.width)/2, (LCD_HEIGHT-fry.height)/2, &fry);
  UG_Update();
  printTime();
  UG_FontSetTransparency(0);
  LCD_PutStr(10, 5, "Image", DEFAULT_FONT, C_YELLOW, C_RED);
  UG_Update();
  UG_FontSetTransparency(t);
#endif
  HAL_Delay(1000);
}


