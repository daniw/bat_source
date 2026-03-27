/*
 * terminal_indicator.h
 *
 *  Created on: Mar 25, 2026
 *      Author: DWI
 */

#ifndef INC_TERMINAL_INDICATOR_H_
#define INC_TERMINAL_INDICATOR_H_

#include "i2c.h"

#define TERMINAL_INDICATOR_ADDRESS         (0x44)
#define TERMINAL_INDICATOR_REG_EN          (0x00)
#define TERMINAL_INDICATOR_REG_MAX_CURRENT (0x01)
#define TERMINAL_INDICATOR_REG_OUT_EN      (0x02)
#define TERMINAL_INDICATOR_REG_FADE        (0x03)
#define TERMINAL_INDICATOR_REG_EXP         (0x04)
#define TERMINAL_INDICATOR_REG_SHUTDOWN    (0x0D)
#define TERMINAL_INDICATOR_REG_RESET       (0x0E)
#define TERMINAL_INDICATOR_REG_UPDATE      (0x0F)
#define TERMINAL_INDICATOR_REG_FLAG_CLR    (0x13)
#define TERMINAL_INDICATOR_REG_OUT0_DC     (0x14)
#define TERMINAL_INDICATOR_REG_OUT1_DC     (0x15)
#define TERMINAL_INDICATOR_REG_OUT2_DC     (0x16)
//#define TERMINAL_INDICATOR_REG_OUT4_DC     (0x17) // Prepared for backlight driver
#define TERMINAL_INDICATOR_REG_OUT0_PWM    (0x18)
#define TERMINAL_INDICATOR_REG_OUT1_PWM    (0x19)
#define TERMINAL_INDICATOR_REG_OUT2_PWM    (0x1A)
//#define TERMINAL_INDICATOR_REG_OUT3_PWM    (0x21) // Prepared for backlight driver
#define TERMINAL_INDICATOR_REG_FLAG        (0x40)

#define TERMINAL_INDICATOR_CMD_SHUTDOWN    (0x33)
#define TERMINAL_INDICATOR_CMD_RESET       (0xCC)
#define TERMINAL_INDICATOR_CMD_UPDATE      (0x55)

#define TERMINAL_INDICATOR_MASK_FLAG_POR   (0x01)
#define TERMINAL_INDICATOR_MASK_FLAG_TSD   (0x02)


void terminal_indicator_init();
void terminal_indicator_deinit();
void terminal_indicator_start();
void terminal_indicator_set_fade();

#endif /* INC_TERMINAL_INDICATOR_H_ */
