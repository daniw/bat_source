/*
 * menu.h
 *
 * Single source of truth for the menu tree: which modes appear in the home
 * carousel, in what order, their display names, accent colors and icons.
 * Replaces the old fragile "(index +/- N) % 6" arithmetic in statemachine.c
 * that silently left STATEMACHINE_MODE_SETTINGS unreachable from the menu.
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

#include "stdint.h"
#include "statemachine.h"
#include "ugui.h"

/* Number of modes shown in the home carousel. */
#define MENU_ORDER_LENGTH 9

typedef void (*menu_icon_draw_fn)(int16_t cx, int16_t cy, int16_t r, UG_COLOR color);

typedef struct {
	statemachine_modes_t mode;
	UG_COLOR accent;
	menu_icon_draw_fn draw_icon;
} menu_entry_t;

/* Carousel order: 60V, 10A, Resistance 4W/2W, Isolation, Voltmeter, Ampmeter, Charge, Settings. */
extern const menu_entry_t menu_order[MENU_ORDER_LENGTH];

/* Returns the carousel entry at [0, MENU_ORDER_LENGTH). Index is wrapped. */
const menu_entry_t *menu_entry_at(uint8_t index);

/* Returns the carousel entry for a given mode, or NULL if the mode isn't in the carousel. */
const menu_entry_t *menu_entry_for_mode(statemachine_modes_t mode);

/* Display name for any statemachine_modes_t value (sized for all 11 enum values --
 * the old DISPLAY_MODE_STRINGS[7] array this replaces was out-of-bounds for SETTINGS). */
const char *menu_name_for_mode(statemachine_modes_t mode);

#endif /* INC_MENU_H_ */
