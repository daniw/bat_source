/*
 * statemachine.c
 *
 *  Created on: Aug 9, 2025
 *      Author: ahorat
 */

#include "statemachine.h"
#include "aux_io_ctrl.h"
//#include "display.h"
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

	adc_convert_data();

	// Handle Buttons press
	if (gpio_readBtnOk())
		ok_button_pressed += 1;
	else if (gpio_readBtnEsc())
		esc_button_pressed += 1;
	else {
		ok_button_pressed = 0;
		esc_button_pressed = 0;
	}
	if (gpio_readBtnOut()) {
		if (out_button_pressed < 2)
			out_button_pressed += 1;
	} else
		out_button_pressed = 0;

	// Select transition to next state
	switch (statemachine_handle.current_mode) {
	case STATEMACHINE_IDLE:
		temp = (tim_encoder_read() / 2) % 6 + 1;
		if (temp != statemachine_handle.current_menu_index) {
			statemachine_handle.current_menu_index = temp;
			//display_show_menu();

		}
		if (ok_button_pressed == 1) {
			statemachine_switchfromIdle(statemachine_handle.current_menu_index);
		}

		if (esc_button_pressed == 1)
			statemachine_handle.current_mode = STATEMACHINE_MODE_SHUTDOWN;
		break;
	case STATEMACHINE_MODE_60V_OUT:
		if (out_button_pressed == 1) {
			ctrl_main_start_ctrl(statemachine_handle.current_mode);
			aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 1);
		} else if (out_button_pressed == 0) {
			ctrl_main_stop_control();
			aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 0);
		}
		if (esc_button_pressed == 1) {
			ctrl_main_stop_control();
			statemachine_switchtoIdle();
		}
		break;

	case STATEMACHINE_MODE_10A_OUT:		//display_draw_measure_data();
		if (out_button_pressed == 1) {
			ctrl_main_start_ctrl(statemachine_handle.current_mode);
			aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 1);
		} else if (out_button_pressed == 0) {
			ctrl_main_stop_control();
			aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 0);
		}
		if (esc_button_pressed == 1) {
			ctrl_main_stop_control();
			statemachine_switchtoIdle();
		}
		break;
	case STATEMACHINE_MODE_RESISTANCE_1A:
	case STATEMACHINE_MODE_RESISTANCE_1mA:
	case STATEMACHINE_MODE_ISOMETER:

		if (esc_button_pressed == 1) {
			statemachine_switchtoIdle();
		}
		break;
	case STATEMACHINE_MODE_VOLTMETER:
		if (esc_button_pressed == 1) {
			ctrl_main_stop_control();
			statemachine_switchtoIdle();
		}
		break;

	case STATEMACHINE_MODE_SETTINGS:
		temp = (tim_encoder_read() / 2) % STATEMACHINE_SETTINGS_MODE_LENGTH;

		//display_show_settings(temp);

		if (esc_button_pressed == 1 && statemachine_handle.settings_mode == 0)
			statemachine_switchtoIdle();
		else if (esc_button_pressed == 1)
			statemachine_handle.settings_mode = 0;
		else if (ok_button_pressed)
			statemachine_handle.settings_mode = temp;

		break;

	case STATEMACHINE_MODE_SHUTDOWN:
		gpio_shutdown();

	default:
		break;
	}

	//display_draw();
}

void statemachine_switchfromIdle(statemachine_modes_t mode) {
	printf("Switch from Idle to %d\r\n", mode);

	switch (mode) {
	case STATEMACHINE_MODE_60V_OUT:
		ui_ctrl_ledOutOn();

		adc_configure_mode(mode);
		statemachine_handle.current_mode = mode;

		/*display_draw_basic_layout();
		 display_draw_measure_layout();
		 display_draw_measure_data();*/
		break;

	case STATEMACHINE_MODE_10A_OUT:
		ui_ctrl_ledOutOn();
		adc_configure_mode(mode);
		statemachine_handle.current_mode = mode;

		/*display_draw_basic_layout();
		 display_draw_measure_layout();
		 display_draw_measure_data();*/
		break;

	case STATEMACHINE_MODE_RESISTANCE_1A:
		ui_ctrl_ledOutOn();
		ui_ctrl_ledSenseOn();

		statemachine_handle.current_mode = mode;
		ctrl_main_start_ctrl(statemachine_handle.current_mode);
		aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 1);

		//display_draw_basic_layout();
		//display_draw_measure_layout();
		//display_draw_measure_data();
		break;

	case STATEMACHINE_MODE_RESISTANCE_1mA:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode = mode;
		ctrl_main_start_ctrl(statemachine_handle.current_mode);
		aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 1);

		//display_draw_basic_layout();
		//display_draw_measure_layout();
		//display_draw_measure_data();
		break;

	case STATEMACHINE_MODE_ISOMETER:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode = mode;

		//display_draw_basic_layout();
		//display_draw_measure_layout();
		//display_draw_measure_data();
		break;

	case STATEMACHINE_MODE_VOLTMETER:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode = mode;
		//
		//display_draw_basic_layout();
		//display_draw_measure_layout();
		//display_draw_measure_data();
		break;

	case STATEMACHINE_MODE_SETTINGS:
		//display_draw_basic_layout();
		//display_show_settings(0);
		statemachine_handle.current_mode = mode;
		break;
	case 0:
		statemachine_switchtoIdle();
		break;
	default:
		break;
	}
	aux_io_ctrl_set_config(mode);
	tim_encoder_reset(0);
}

void statemachine_switchtoIdle(void) {
	printf("Switch to Idle\r\n");

	aux_io_ctrl_set_config(STATEMACHINE_IDLE);
	ctrl_main_stop_control();
	aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 0);
	tim_encoder_reset(62);
	statemachine_handle.current_mode = STATEMACHINE_IDLE;
	ui_ctrl_ledOutOff();
	ui_ctrl_ledSenseOff();
//display_draw_basic_layout();

}
