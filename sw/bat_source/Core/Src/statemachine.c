/*
 * statemachine.c
 *
 *  Created on: Aug 9, 2025
 *      Author: ahorat
 */

#include "statemachine.h"
#include "aux_io_ctrl.h"
#include "display.h"
#include "menu.h"
#include "input.h"
#include "timer.h"
#include "ctrl_main.h"
#include "stm32g4xx_hal.h"
#include "adc.h"
#include "ui_ctrl.h"
#include "tim.h"
#include "gpio.h"

statemachine_t statemachine_handle;
uint16_t ok_button_pressed;
uint16_t esc_button_pressed;
uint16_t out_button_pressed;
extern ctrl_main_t ctrl_main_handle;

void statemachine_switchfromIdle(statemachine_modes_t mode);
void statemachine_switchtoIdle(void);
static void statemachine_apply_encoder_setpoint(void);

void statemachine_init(void) {
	statemachine_handle.output_on = 0;
	statemachine_handle.current_mode = STATEMACHINE_IDLE;
	statemachine_handle.current_menu_index = 0;
	statemachine_handle.settings_mode = STATEMACHINE_SETTINGS_MODE_MENU;
	ok_button_pressed = 0;
	esc_button_pressed = 0;

	input_init();
	input_encoder_reset(62);
	display_init();

	timer_add(STATEMACHINE_STEP_PERIOD_mS, TIMER_TYPE_TICK, EVENT_SM_STEP, 0);
}

/* Feeds the live encoder value into ctrl_main's poti_reference so the setpoint
 * tracks the encoder while a mode is running (dial in the value, hold OUT to
 * apply it -- ctrl_main_apply_reference() only takes effect once the control
 * loop is actually running). */
static void statemachine_apply_encoder_setpoint(void) {
	ctrl_main_handle.poti_reference = input_encoder_read();
}

void statemachine_step(void) {
	uint8_t temp;

	adc_convert_data();
	ui_ctrl_step();

	// Handle Buttons press
	if (input_btn_ok())
		ok_button_pressed += 1;
	else if (input_btn_esc())
		esc_button_pressed += 1;
	else {
		ok_button_pressed = 0;
		esc_button_pressed = 0;
	}
	if (input_btn_out()) {
		if (out_button_pressed < 2)
			out_button_pressed += 1;
	} else
		out_button_pressed = 0;

	// Select transition to next state
	switch (statemachine_handle.current_mode) {
	case STATEMACHINE_IDLE:
		temp = (input_encoder_read() / 2) % MENU_ORDER_LENGTH;
		if (temp != statemachine_handle.current_menu_index) {
			statemachine_handle.current_menu_index = temp;
			display_show_idle(temp);
		}
		if (ok_button_pressed == 1) {
			statemachine_switchfromIdle(menu_entry_at(temp)->mode);
		}

		if (esc_button_pressed == 1)
			statemachine_handle.current_mode = STATEMACHINE_MODE_SHUTDOWN;
		break;

	case STATEMACHINE_MODE_60V_OUT:
	case STATEMACHINE_MODE_10A_OUT:
		statemachine_apply_encoder_setpoint();
		if (out_button_pressed == 1) {
			statemachine_handle.output_on = 1;
			ctrl_main_start_ctrl(
					statemachine_handle.current_mode == STATEMACHINE_MODE_60V_OUT ?
							CTRL_MODE_60V : CTRL_MODE_10A);
			aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 1);
		} else if (out_button_pressed == 0) {
			statemachine_handle.output_on = 0;
			ctrl_main_stop_control();
			aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 0);
		}
		display_update_mode(statemachine_handle.current_mode,
				statemachine_handle.output_on);
		if (esc_button_pressed == 1) {
			ctrl_main_stop_control();
			statemachine_switchtoIdle();
		}
		break;

	case STATEMACHINE_MODE_RESISTANCE_1A:
	case STATEMACHINE_MODE_RESISTANCE_1mA:
		display_update_mode(statemachine_handle.current_mode, 1);
		if (esc_button_pressed == 1) {
			ctrl_main_stop_control();
			statemachine_switchtoIdle();
		}
		break;

	case STATEMACHINE_MODE_ISOMETER:
		statemachine_apply_encoder_setpoint();
		display_update_mode(statemachine_handle.current_mode, 1);
		if (esc_button_pressed == 1) {
			ctrl_main_stop_control();
			statemachine_switchtoIdle();
		}
		break;

	case STATEMACHINE_MODE_VOLTMETER:
		display_update_mode(statemachine_handle.current_mode, 1);
		if (esc_button_pressed == 1) {
			ctrl_main_stop_control();
			statemachine_switchtoIdle();
		}
		break;

	case STATEMACHINE_MODE_AMPMETER:
		display_update_mode(statemachine_handle.current_mode, 1);
		if (esc_button_pressed == 1) {
			ctrl_main_stop_control();
			statemachine_switchtoIdle();
		}
		break;

	case STATEMACHINE_MODE_CHARGE:
		/* Latched start/stop (OK toggles) instead of hold-to-enable: a charge
		 * cycle can run for hours, so requiring a held button is impractical. */
		if (ok_button_pressed == 1) {
			statemachine_handle.output_on = !statemachine_handle.output_on;
			if (statemachine_handle.output_on) {
				ctrl_main_start_ctrl(CTRL_MODE_CHARGE);
				aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 1);
			} else {
				ctrl_main_stop_control();
				aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 0);
			}
		}
		display_update_mode(statemachine_handle.current_mode,
				statemachine_handle.output_on);
		if (esc_button_pressed == 1) {
			statemachine_handle.output_on = 0;
			ctrl_main_stop_control();
			aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 0);
			statemachine_switchtoIdle();
		}
		break;

	case STATEMACHINE_MODE_SETTINGS:
		if (statemachine_handle.settings_mode == STATEMACHINE_SETTINGS_MODE_MENU) {
			temp = STATEMACHINE_SETTINGS_MODE_BMS
					+ (input_encoder_read() / 2)
							% (STATEMACHINE_SETTINGS_MODE_LENGTH - 1);
			if (temp != statemachine_handle.current_menu_index) {
				statemachine_handle.current_menu_index = temp;
				display_show_settings_list(temp);
			}
			if (ok_button_pressed == 1) {
				statemachine_handle.settings_mode =
						(statemachine_settings_modes_t) statemachine_handle.current_menu_index;
				display_enter_settings_detail(statemachine_handle.settings_mode);
			}
			if (esc_button_pressed == 1)
				statemachine_switchtoIdle();
		} else {
			display_update_settings_detail(statemachine_handle.settings_mode);
			if (esc_button_pressed == 1) {
				statemachine_handle.settings_mode = STATEMACHINE_SETTINGS_MODE_MENU;
				display_show_settings_list(statemachine_handle.current_menu_index);
			}
		}
		break;

	case STATEMACHINE_MODE_SHUTDOWN:
		gpio_shutdown();

	default:
		break;
	}
}

void statemachine_switchfromIdle(statemachine_modes_t mode) {
	printf("Switch from Idle to %d\r\n", mode);

	switch (mode) {
	case STATEMACHINE_MODE_60V_OUT:
		ui_ctrl_ledOutOn();

		adc_configure_mode(mode);
		statemachine_handle.current_mode = mode;
		statemachine_handle.output_on = 0;
		display_enter_mode(mode);
		break;

	case STATEMACHINE_MODE_10A_OUT:
		ui_ctrl_ledOutOn();
		adc_configure_mode(mode);
		statemachine_handle.current_mode = mode;
		statemachine_handle.output_on = 0;
		display_enter_mode(mode);
		break;

	case STATEMACHINE_MODE_RESISTANCE_1A:
		ui_ctrl_ledOutOn();
		ui_ctrl_ledSenseOn();

		statemachine_handle.current_mode = mode;
		adc_configure_mode(mode);
		ctrl_main_start_ctrl(CTRL_MODE_RESISTANCE_1A);
		aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 1);
		display_enter_mode(mode);
		break;

	case STATEMACHINE_MODE_RESISTANCE_1mA:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode = mode;
		adc_configure_mode(mode);
		ctrl_main_start_ctrl(CTRL_MODE_RESISTANCE_1mA);
		aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 1);
		display_enter_mode(mode);
		break;

	case STATEMACHINE_MODE_ISOMETER:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode = mode;
		adc_configure_mode(mode);
		ctrl_main_start_ctrl(CTRL_MODE_ISOMETER);
		display_enter_mode(mode);
		break;

	case STATEMACHINE_MODE_VOLTMETER:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode = mode;
		adc_configure_mode(mode);
		display_enter_mode(mode);
		break;

	case STATEMACHINE_MODE_AMPMETER:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode = mode;
		adc_configure_mode(mode);
		display_enter_mode(mode);
		break;

	case STATEMACHINE_MODE_CHARGE:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode = mode;
		statemachine_handle.output_on = 0;
		adc_configure_mode(mode);
		display_enter_mode(mode);
		break;

	case STATEMACHINE_MODE_SETTINGS:
		statemachine_handle.current_mode = mode;
		statemachine_handle.settings_mode = STATEMACHINE_SETTINGS_MODE_MENU;
		statemachine_handle.current_menu_index = STATEMACHINE_SETTINGS_MODE_BMS;
		display_show_settings_list(statemachine_handle.current_menu_index);
		break;
	case 0:
		statemachine_switchtoIdle();
		break;
	default:
		break;
	}
	aux_io_ctrl_set_config(mode);
	input_encoder_reset(0);
}

void statemachine_switchtoIdle(void) {
	printf("Switch to Idle\r\n");

	aux_io_ctrl_set_config(STATEMACHINE_IDLE);
	ctrl_main_stop_control();
	aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 0);
	input_encoder_reset(62);
	statemachine_handle.current_mode = STATEMACHINE_IDLE;
	statemachine_handle.current_menu_index = 0xFF; /* force a redraw on the next tick */
	ui_ctrl_ledOutOff();
	ui_ctrl_ledSenseOff();
}
