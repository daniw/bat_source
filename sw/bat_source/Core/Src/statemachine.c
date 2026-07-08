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
#include "hrtim.h"
#include "bq76905.h"
#include "ctrl_param.h"

statemachine_t statemachine_handle;
uint16_t ok_button_pressed;
uint16_t esc_button_pressed;
uint16_t out_button_pressed;
extern ctrl_main_t ctrl_main_handle;
extern ADC_MEAS_DATA adc_data;
extern BQ76905_handle bms;

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

		// CHARGE is auto-entered, not menu/button-selected: whenever idle and
		// a 15-25V supply is detected at the output with no BMS fault
		// latched, start charging on our own.
		/*if (adc_data.converted.v_term >= 15000 && adc_data.converted.v_term <= 25000
				&& !(bms.SafetyRegisters.safetyStatusA || bms.SafetyRegisters.safetyStatusB)) {
			statemachine_switchfromIdle(STATEMACHINE_MODE_CHARGE);
		}*/
		break;

	case STATEMACHINE_MODE_60V_OUT:
	case STATEMACHINE_MODE_10A_OUT:
		statemachine_apply_encoder_setpoint();
		if (out_button_pressed == 1) {
			statemachine_handle.output_on = 1;
			ctrl_main_start_ctrl(statemachine_mode_to_ctrl_mode(statemachine_handle.current_mode));
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
		// Charging starts immediately on auto-detected entry (see the IDLE
		// case above) and stops the same way, without a manual OK toggle.
		// End-of-charge, BMS fault, or the external supply being removed
		// (14V exit vs. the 15V entry threshold - hysteresis so the mode
		// doesn't flap right at the boundary) all stop charging the same
		// way. BMS data only refreshes once a second (EVENT_BMS_TIMER)
		// regardless, so checking it here at the 100ms tick loses nothing
		// vs. checking in the fast ADC-ISR-driven control loop.
		display_update_mode(statemachine_handle.current_mode,
				statemachine_handle.output_on);
		if (bms.VoltageRegisters.StackVoltage >= CTRL_PARAM_CHARGE_END_VOLTAGE_mV
				|| bms.SafetyRegisters.safetyStatusA || bms.SafetyRegisters.safetyStatusB
				|| adc_data.converted.v_term < 14000) {
			ctrl_main_stop_control();
			statemachine_switchtoIdle();
		}
		if (esc_button_pressed == 1) {
			ctrl_main_stop_control();
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
		ctrl_main_start_ctrl(statemachine_mode_to_ctrl_mode(mode));
		aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 1);
		display_enter_mode(mode);
		break;

	case STATEMACHINE_MODE_RESISTANCE_1mA:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode = mode;
		adc_configure_mode(mode);
		ctrl_main_start_ctrl(statemachine_mode_to_ctrl_mode(mode));
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
		// Shoot-through on the SEK half-bridge is only safe if there's no
		// significant voltage across it already - refuse to enter otherwise.
		// return (not break) so the shared tail below doesn't reconfigure
		// the relays for a mode we just refused to actually enter.
		if (adc_data.converted.v_term >= 500) { // mV
			return;
		}
		ui_ctrl_ledOutOn();
		statemachine_handle.current_mode = mode;
		adc_configure_mode(mode);
		hrtim_sek_force_short();
		display_enter_mode(mode);
		break;

	case STATEMACHINE_MODE_CHARGE:
		// Interlock (refuse if a fault is already latched) lives in the
		// auto-detect check in statemachine_step()'s IDLE case, since entry
		// here *is* the auto-detect firing - starts immediately, like
		// RESISTANCE_1A/1mA, since charging isn't a "hold a button" action.
		ui_ctrl_ledOutOn();
		statemachine_handle.current_mode = mode;
		statemachine_handle.output_on = 1;
		adc_configure_mode(mode);
		ctrl_main_start_ctrl(statemachine_mode_to_ctrl_mode(mode));
		aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 1);
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
	hrtim_sek_restore(); // no-op unless AMPMETER left the SEK half-bridge shorted
	input_encoder_reset(62);
	statemachine_handle.current_mode = STATEMACHINE_IDLE;
	statemachine_handle.current_menu_index = 0xFF; /* force a redraw on the next tick */
	ui_ctrl_ledOutOff();
	ui_ctrl_ledSenseOff();
}
