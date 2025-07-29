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
 * @file      driver_ssd1309_basic.c
 * @brief     driver ssd1309 basic source file
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

#include "ds1309/driver_ssd1309_basic.h"

static ssd1309_handle_t gs_handle;        /**< ssd1309 handle */

/**
 * @brief     basic example init
 * @param[in] interface interface type
 * @param[in] addr iic device address
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t ssd1309_basic_init(ssd1309_interface_t interface, ssd1309_address_t addr)
{
    uint8_t res;
    
    /* link functions */
    DRIVER_SSD1309_LINK_INIT(&gs_handle, ssd1309_handle_t);
    DRIVER_SSD1309_LINK_IIC_INIT(&gs_handle, ssd1309_interface_iic_init);
    DRIVER_SSD1309_LINK_IIC_DEINIT(&gs_handle, ssd1309_interface_iic_deinit);
    DRIVER_SSD1309_LINK_IIC_WRITE(&gs_handle, ssd1309_interface_iic_write);
    DRIVER_SSD1309_LINK_SPI_INIT(&gs_handle, ssd1309_interface_spi_init);
    DRIVER_SSD1309_LINK_SPI_DEINIT(&gs_handle, ssd1309_interface_spi_deinit);
    DRIVER_SSD1309_LINK_SPI_WRITE_COMMAND(&gs_handle, ssd1309_interface_spi_write_cmd);
    DRIVER_SSD1309_LINK_SPI_COMMAND_DATA_GPIO_INIT(&gs_handle, ssd1309_interface_spi_cmd_data_gpio_init);
    DRIVER_SSD1309_LINK_SPI_COMMAND_DATA_GPIO_DEINIT(&gs_handle, ssd1309_interface_spi_cmd_data_gpio_deinit);
    DRIVER_SSD1309_LINK_SPI_COMMAND_DATA_GPIO_WRITE(&gs_handle, ssd1309_interface_spi_cmd_data_gpio_write);
    DRIVER_SSD1309_LINK_RESET_GPIO_INIT(&gs_handle, ssd1309_interface_reset_gpio_init);
    DRIVER_SSD1309_LINK_RESET_GPIO_DEINIT(&gs_handle, ssd1309_interface_reset_gpio_deinit);
    DRIVER_SSD1309_LINK_RESET_GPIO_WRITE(&gs_handle, ssd1309_interface_reset_gpio_write);
    DRIVER_SSD1309_LINK_DELAY_MS(&gs_handle, ssd1309_interface_delay_ms);
    DRIVER_SSD1309_LINK_DEBUG_PRINT(&gs_handle, ssd1309_interface_debug_print);

    /* set interface */
    res = ssd1309_set_interface(&gs_handle, interface);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set interface failed.\n");
        
        return 1;
    }
    
    /* set addr pin */
    res = ssd1309_set_addr_pin(&gs_handle, addr);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set addr failed.\n");
        
        return 1;
    }
    
    /* ssd1309 init */
    res = ssd1309_init(&gs_handle);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: init failed.\n");
        
        return 1;
    }
    
    /* disable mcu interface lock */
    res = ssd1309_set_mcu_interface_lock(&gs_handle, SSD1309_BOOL_FALSE);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set mcu interface lock failed.\n");
        (void)ssd1309_deinit(&gs_handle);

        return 1;
    }
    
    /* set default gpio config */
    res = ssd1309_set_gpio(&gs_handle, SSD1309_BASIC_DEFAULT_GPIO_CONFIG);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set gpio failed.\n");
        (void)ssd1309_deinit(&gs_handle);

        return 1;
    }
    
    /* close display */
    res = ssd1309_set_display(&gs_handle, SSD1309_DISPLAY_OFF);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set display failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set column address range */
    res = ssd1309_set_column_address_range(&gs_handle, SSD1309_BASIC_DEFAULT_COLUMN_ADDRESS_RANGE_START, SSD1309_BASIC_DEFAULT_COLUMN_ADDRESS_RANGE_END);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set column address range failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set page address range */
    res = ssd1309_set_page_address_range(&gs_handle, SSD1309_BASIC_DEFAULT_PAGE_ADDRESS_RANGE_START, SSD1309_BASIC_DEFAULT_PAGE_ADDRESS_RANGE_END);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set page address range failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set low column start address */
    res = ssd1309_set_low_column_start_address(&gs_handle, SSD1309_BASIC_DEFAULT_LOW_COLUMN_START_ADDRESS);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set low column start address failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set high column start address */
    res = ssd1309_set_high_column_start_address(&gs_handle, SSD1309_BASIC_DEFAULT_HIGH_COLUMN_START_ADDRESS);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set high column start address failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set display start line */
    res = ssd1309_set_display_start_line(&gs_handle, SSD1309_BASIC_DEFAULT_DISPLAY_START_LINE);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set display start line failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* deactivate scroll */
    res = ssd1309_deactivate_scroll(&gs_handle);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set deactivate scroll failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set contrast */
    res = ssd1309_set_contrast(&gs_handle, SSD1309_BASIC_DEFAULT_CONTRAST);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set contrast failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set segment remap */
    res = ssd1309_set_segment_remap(&gs_handle, SSD1309_BASIC_DEFAULT_SEGMENT);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set segment remap failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set scan direction */
    res = ssd1309_set_scan_direction(&gs_handle, SSD1309_BASIC_DEFAULT_SCAN_DIRECTION);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set scan direction failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set display mode */
    res = ssd1309_set_display_mode(&gs_handle, SSD1309_BASIC_DEFAULT_DISPLAY_MODE);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set display mode failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set multiplex ratio */
    res = ssd1309_set_multiplex_ratio(&gs_handle, SSD1309_BASIC_DEFAULT_MULTIPLEX_RATIO);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set multiplex ratio failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set display offset */
    res = ssd1309_set_display_offset(&gs_handle, SSD1309_BASIC_DEFAULT_DISPLAY_OFFSET);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set display offset failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set display clock */
    res = ssd1309_set_display_clock(&gs_handle, SSD1309_BASIC_DEFAULT_OSCILLATOR_FREQUENCY, SSD1309_BASIC_DEFAULT_CLOCK_DIVIDE);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set display clock failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set pre charge period */
    res = ssd1309_set_precharge_period(&gs_handle, SSD1309_BASIC_DEFAULT_PHASE1_PERIOD, SSD1309_BASIC_DEFAULT_PHASE2_PERIOD);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set pre charge period failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set hardware pins conf */
    res = ssd1309_set_com_pins_hardware_conf(&gs_handle, SSD1309_BASIC_DEFAULT_PIN_CONF, SSD1309_BASIC_DEFAULT_LEFT_RIGHT_REMAP);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set com pins hardware conf failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set deselect level 0.77 */
    res = ssd1309_set_deselect_level(&gs_handle, SSD1309_BASIC_DEFAULT_DESELECT_LEVEL);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set deselect level failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set page memory addressing mode */
    res = ssd1309_set_memory_addressing_mode(&gs_handle, SSD1309_MEMORY_ADDRESSING_MODE_VERTICAL);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set memory addressing level failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* entire display off */
    res = ssd1309_set_entire_display(&gs_handle, SSD1309_ENTIRE_DISPLAY_OFF);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set entire display failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* enable display */
    res = ssd1309_set_display(&gs_handle, SSD1309_DISPLAY_ON);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: set display failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    /* clear screen */
    res = ssd1309_clear(&gs_handle);
    if (res != 0)
    {
        ssd1309_interface_debug_print("ssd1309: clear failed.\n");
        (void)ssd1309_deinit(&gs_handle);
        
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t ssd1309_basic_deinit(void)
{
    /* deinit ssd1309 */
    if (ssd1309_deinit(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example display on
 * @return status code
 *         - 0 success
 *         - 1 display on failed
 * @note   none
 */
uint8_t ssd1309_basic_display_on(void)
{
    uint8_t res;
    
    /* display on */
    res = ssd1309_set_display(&gs_handle, SSD1309_DISPLAY_ON);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example display off
 * @return status code
 *         - 0 success
 *         - 1 display off failed
 * @note   none
 */
uint8_t ssd1309_basic_display_off(void)
{
    uint8_t res;
    
    /* display off */
    res = ssd1309_set_display(&gs_handle, SSD1309_DISPLAY_OFF);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example clear
 * @return status code
 *         - 0 success
 *         - 1 clear failed
 * @note   none
 */
uint8_t ssd1309_basic_clear(void)
{
    /* clear */
    if (ssd1309_clear(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

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
uint8_t ssd1309_basic_write_point(uint8_t x, uint8_t y, uint8_t data)
{
    uint8_t res;
    
    /* write point */
    res = ssd1309_write_point(&gs_handle, x, y, data);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

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
uint8_t ssd1309_basic_read_point(uint8_t x, uint8_t y, uint8_t *data)
{
    uint8_t res;
    
    /* read point in gram */
    res = ssd1309_read_point(&gs_handle, x, y, data);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

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
uint8_t ssd1309_basic_string(uint8_t x, uint8_t y, char *str, uint16_t len, uint8_t color, ssd1309_font_t font)
{
    uint8_t res;
    
    /* write string in gram */
    res = ssd1309_gram_write_string(&gs_handle, x, y, str, len, color, font);
    if (res != 0)
    {
        return 1;
    }
    
    /* update gram */
    if (ssd1309_gram_update(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

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
uint8_t ssd1309_basic_rect(uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t color)
{
    uint8_t res;
    
    /* fill rect in gram */
    res = ssd1309_gram_fill_rect(&gs_handle, left, top, right, bottom, color);
    if (res != 0)
    {
        return 1;
    }
    
    /* update gram */
    if (ssd1309_gram_update(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

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
uint8_t ssd1309_basic_picture(uint8_t left, uint8_t top, uint8_t right, uint8_t bottom, uint8_t *img)
{
    uint8_t res;
    
    /* draw picture in gram */
    res = ssd1309_gram_draw_picture(&gs_handle, left, top, right, bottom, img);
    if (res != 0)
    {
        return 1;
    }
    
    /* update gram */
    if (ssd1309_gram_update(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}
