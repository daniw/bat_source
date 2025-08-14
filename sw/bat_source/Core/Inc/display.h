/*
 * display.h
 *
 *  Created on: Aug 9, 2025
 *      Author: ahorat
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "stdint.h"

uint8_t display_init(void);


void display_clear(void);
void display_debug_state(void);
void display_draw_basic_layout(void);
void display_draw_battery(uint8_t percentage);
void display_draw_measure_layout(void);
void display_draw_measure_data(void);
void display_show_menu(void);
void display_draw(void) ;





#endif /* INC_DISPLAY_H_ */
