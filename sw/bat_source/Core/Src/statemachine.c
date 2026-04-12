/*
 * statemachine.c
 *
 *  Created on: Aug 9, 2025
 *      Author: ahorat
 */

#include "statemachine.h"
#include "aux_io_ctrl.h"
#include "display.h"
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

void statemachine_init(void) {
	statemachine_handle.output_on = 0;
	statemachine_handle.current_mode = STATEMACHINE_IDLE;
	ok_button_pressed = 0;
	esc_button_pressed = 0;
	tim_encoder_reset(62);
	timer_add(STATEMACHINE_STEP_PERIOD_mS, TIMER_TYPE_TICK, EVENT_SM_STEP, 0);
}

void statemachine_step(void) {
	uint8_t temp;
	if (gpio_readBtnOk())
		ok_button_pressed += 1;
	else if (gpio_readBtnEsc())
		esc_button_pressed += 1;
	else {
		ok_button_pressed = 0;
		esc_button_pressed = 0;
	}

	if (gpio_readBtnOut())
		out_button_pressed += 1;
	else
		out_button_pressed = 0;

	switch (statemachine_handle.current_mode) {
	case STATEMACHINE_IDLE:
		temp = (tim_encoder_read() / 2) % 6 + 1;
		if (temp != statemachine_handle.current_menu_index) {
			statemachine_handle.current_menu_index = temp;
			display_show_menu();

		}
		if (ok_button_pressed == 1) {
			statemachine_switchfromIdle(statemachine_handle.current_menu_index );
		}

		if (esc_button_pressed == 1)
			statemachine_handle.current_mode = STATEMACHINE_MODE_SHUTDOWN;
		break;
	case STATEMACHINE_MODE_60V_OUT:
		display_draw_measure_data();
		if (out_button_pressed == 1) {
			ctrl_main_init();
			//aux_io_ctrl_manual_set_io(AUX_IO_CTRL_EN, 1);
			//pwm_enable();
		} else if (out_button_pressed == 0) {
			//pwm_disable();
			//aux_io_ctrl_manual_set_io(AUX_IO_CTRL_EN, 0);
		}

		if (esc_button_pressed == 1) {
			statemachine_switchtoIdle();
		}
		break;

	case STATEMACHINE_MODE_10A_OUT:
		display_draw_measure_data();
				if (esc_button_pressed == 1) {
					statemachine_switchtoIdle();
				}
				break;
	case STATEMACHINE_MODE_RESISTANCE_1A:
		display_draw_measure_data();
				if (esc_button_pressed == 1) {
					statemachine_switchtoIdle();
				}
				break;
	case STATEMACHINE_MODE_ISOMETER:
		display_draw_measure_data();
				if (esc_button_pressed == 1) {
					statemachine_switchtoIdle();
				}
				break;
	case STATEMACHINE_MODE_VOLTMETER:
		display_draw_measure_data();
				if (esc_button_pressed == 1) {
					statemachine_switchtoIdle();
				}
				break;

	case STATEMACHINE_MODE_SETTINGS:
		temp = (tim_encoder_read() / 2) % STATEMACHINE_SETTINGS_MODE_LENGTH;

		display_show_settings(temp);

		if (esc_button_pressed == 1 && statemachine_handle.settings_mode == 0)
			statemachine_switchtoIdle();
		else if (esc_button_pressed == 1)
			statemachine_handle.settings_mode = 0;
		else if (ok_button_pressed)
			statemachine_handle.settings_mode = temp;

		break;

	case STATEMACHINE_MODE_SHUTDOWN:
		if (esc_button_pressed == 0)
			gpio_shutdown();
	default:
		break;
	}

	display_draw();
}

void statemachine_switchfromIdle(statemachine_modes_t mode) {
	switch (mode) {
	case STATEMACHINE_MODE_60V_OUT:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode =mode;

		display_draw_basic_layout();
		display_draw_measure_layout();
		display_draw_measure_data();
		break;

	case STATEMACHINE_MODE_10A_OUT:
		ui_ctrl_ledOutOn();
		statemachine_handle.current_mode = mode;

		display_draw_basic_layout();
		display_draw_measure_layout();
		display_draw_measure_data();
		break;

	case STATEMACHINE_MODE_RESISTANCE_1A:
		ui_ctrl_ledOutOn();
		ui_ctrl_ledSenseOn();

		statemachine_handle.current_mode =mode;

		display_draw_basic_layout();
		display_draw_measure_layout();
		display_draw_measure_data();
		break;

	case STATEMACHINE_MODE_ISOMETER:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode =mode;

		display_draw_basic_layout();
		display_draw_measure_layout();
		display_draw_measure_data();
		break;

	case STATEMACHINE_MODE_VOLTMETER:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode = mode;

		display_draw_basic_layout();
		display_draw_measure_layout();
		display_draw_measure_data();
		break;

	case STATEMACHINE_MODE_SETTINGS:
		display_draw_basic_layout();
		display_show_settings(0);
		statemachine_handle.current_mode = mode;
		break;
	case 0:
		//statemachine_handle.current_mode =
		//		statemachine_handle.current_menu_index;
		break;
	default:
		break;
	}

	tim_encoder_reset(0);
}

void statemachine_switchtoIdle(void) {
	tim_encoder_reset(62);
	statemachine_handle.current_mode = STATEMACHINE_IDLE;
	ui_ctrl_ledOutOff();
	ui_ctrl_ledSenseOff();
	display_draw_basic_layout();

}
