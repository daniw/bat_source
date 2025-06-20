/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_ssd1309_basic.h
 * @brief     driver ssd1309 basic header file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2024-10-30
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2024/10/30  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#ifndef DRIVER_SSD1309_BASIC_H
#define DRIVER_SSD1309_BASIC_H

#include "driver_ssd1309_interface.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup ssd1309_example_driver ssd1309 example driver function
 * @brief    ssd1309 example driver modules
 * @ingroup  ssd1309_driver
 * @{
 */

/**
 * @brief ssd1309 basic example default definition
 */
#define SSD1309_BASIC_DEFAULT_DESELECT_LEVEL                   SSD1309_DESELECT_LEVEL_0P78                /**< set deselect level 0.78 */
#define SSD1309_BASIC_DEFAULT_LEFT_RIGHT_REMAP                 SSD1309_LEFT_RIGHT_REMAP_DISABLE           /**< disable remap */
#define SSD1309_BASIC_DEFAULT_PIN_CONF                         SSD1309_PIN_CONF_ALTERNATIVE               /**< set alternative */
#define SSD1309_BASIC_DEFAULT_PHASE1_PERIOD                    0x01                                       /**< set phase 1 */
#define SSD1309_BASIC_DEFAULT_PHASE2_PERIOD                    0x0F                                       /**< set phase F */
#define SSD1309_BASIC_DEFAULT_OSCILLATOR_FREQUENCY             0x08                                       /**< set 8 */
#define SSD1309_BASIC_DEFAULT_CLOCK_DIVIDE                     0x00                                       /**< set clock div 0 */
#define SSD1309_BASIC_DEFAULT_DISPLAY_OFFSET                   0x00                                       /**< set display offset */
#define SSD1309_BASIC_DEFAULT_MULTIPLEX_RATIO                  0x3F                                       /**< set ratio */
#define SSD1309_BASIC_DEFAULT_DISPLAY_MODE                     SSD1309_DISPLAY_MODE_NORMAL                /**< set normal mode */
//#define SSD1309_BASIC_DEFAULT_SCAN_DIRECTION                   SSD1309_SCAN_DIRECTION_COMN_1_START        /**< set scan 1 */
#define SSD1309_BASIC_DEFAULT_SCAN_DIRECTION                   SSD1309_SCAN_DIRECTION_COM0_START        /**< set scan 1 */
//#define SSD1309_BASIC_DEFAULT_SEGMENT                          SSD1309_SEGMENT_COLUMN_ADDRESS_127         /**< set column 127 */
#define SSD1309_BASIC_DEFAULT_SEGMENT                          SSD1309_SEGMENT_COLUMN_ADDRESS_0         /**< set column 127 */
#define SSD1309_BASIC_DEFAULT_CONTRAST                         0xCF                                       /**< set contrast 0xCF */
#define SSD1309_BASIC_DEFAULT_GPIO_CONFIG                      SSD1309_GPIO_OUTPUT_LOW                    /**< output low */
#define SSD1309_BASIC_DEFAULT_DISPLAY_START_LINE               0x00                                       /**< set start line 0 */
#define SSD1309_BASIC_DEFAULT_HIGH_COLUMN_START_ADDRESS        0x00                                       /**< set high start 0 */
#define SSD1309_BASIC_DEFAULT_LOW_COLUMN_START_ADDRESS         0x00                                       /**< set low start 0 */
#define SSD1309_BASIC_DEFAULT_PAGE_ADDRESS_RANGE_START         0x00                                       /**< set page range start */
#define SSD1309_BASIC_DEFAULT_PAGE_ADDRESS_RANGE_END           0x07                                       /**< set page range end */
#define SSD1309_BASIC_DEFAULT_COLUMN_ADDRESS_RANGE_START       0x00                                       /**< set range start */
#define SSD1309_BASIC_DEFAULT_COLUMN_ADDRESS_RANGE_END         0x7F                                       /**< set range end */

/**
 * @brief     basic example init
 * @param[in] interface interface type
 * @param[in] addr iic device address
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t ssd1309_basic_init(ssd1309_interface_t interface, ssd1309_address_t addr);

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t ssd1309_basic_deinit(void);

/**
 * @brief  basic example display on
 * @return status code
 *         - 0 success
 *         - 1 display on failed
 * @note   none
 */
uint8_t ssd1309_basic_display_on(void);

/**
 * @brief  basic example display off
 * @return status code
 *         - 0 success
 *         - 1 display off failed
 * @note   none
 */
uint8_t ssd1309_basic_display_off(void);

/**
 * @brief  basic example clear
 * @return status code
 *         - 0 success
 *         - 1 clear failed
 * @note   none
 */
uint8_t ssd1309_basic_clear(void);

/**
 * @brief     basic example write a point
 * @param[in] x coordinate x
 * @param[in] y coordinate y
 * @param[in] data written data
 * @return    status code
 *            - 0 success
 *            - 1 write point failed
 * @note      none
 */
uint8_t ssd1309_basic_write_point(uint8_t x, uint8_t y, uint8_t data);

/**
 * @brief      basic example read a point
 * @param[in]  x coordinate x
 * @param[in]  y coordinate y
 * @param[out] *data pointer to a data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read point failed
 * @note       none
 */
uint8_t ssd1309_basic_read_point(uint8_t x, uint8_t y, uint8_t *data);

/**
 * @brief     basic example draw a string
 * @param[in] x coordinate x
 * @param[in] y coordinate y
 * @param[in] *str pointer to a written string address
 * @param[in] len length of the string
 * @param[in] color display color
 * @param[in] font display font size
 * @return    status code
 *            - 0 success
 *            - 1 write string failed
 * @note      none
 */
uint8_t ssd1309_basic_string(uint8_t x, uint8_t y, char *str, uint16_t len, uint8_t color, ssd1309_font_t font);

/**
 * @brief     basic example fill a rectangle
 * @param[in] left left coordinate x
 * @param[in] top top coordinate y
 * @param[in] right right coordinate x
 * @param[in] bottom bottom coordinate y
 * @param[in] color display color
 * @return    status code
 *            - 0 success
 *            - 1 fill rect failed
 * @note      none
 */
uint8_t ssd1309_basic_rect(uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t color);

/**
 * @brief     basic example draw a picture
 * @param[in] left left coordinate x
 * @param[in] top top coordinate y
 * @param[in] right right coordinate x
 * @param[in] bottom bottom coordinate y
 * @param[in] *img pointer to a image buffer
 * @return    status code
 *            - 0 success
 *            - 1 draw picture failed
 * @note      none
 */
uint8_t ssd1309_basic_picture(uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t *img);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
