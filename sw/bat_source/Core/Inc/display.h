/*
 * display.h
 *
 * UI rendering for the 320x240 color TFT (ST7789, via lcd.h/ugui.h). Replaces
 * the old display.c/display_icons.h, which targeted a monochrome SSD1309
 * OLED whose driver no longer exists in this repo.
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "stdint.h"
#include "statemachine.h"

void display_init(void);

/* Home / carousel screen. menu_index is the raw carousel position (0..MENU_ORDER_LENGTH-1). */
void display_show_idle(uint8_t menu_index);

/* Static chrome (title, labels, units, footer) for a mode's detail screen -- call once on entry. */
void display_enter_mode(statemachine_modes_t mode);

/* Live data refresh for a mode's detail screen -- call every statemachine_step() tick while active.
 * output_active selects the accent-border "energized" indicator (hold-to-enable / latched modes). */
void display_update_mode(statemachine_modes_t mode, uint8_t output_active);

/* Settings: vertical list of sub-screens. */
void display_show_settings_list(uint8_t submenu_index);
void display_enter_settings_detail(uint8_t submenu_index);
void display_update_settings_detail(uint8_t submenu_index);

#endif /* INC_DISPLAY_H_ */
