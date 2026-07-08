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

// Modes reachable from the IDLE button/encoder menu. CHARGE is deliberately
// excluded - it's auto-entered/exited based on detected voltage, not
// user-selected (see statemachine_step()'s STATEMACHINE_IDLE case).
static const statemachine_modes_t idle_menu_modes[] = {
	STATEMACHINE_MODE_60V_OUT, STATEMACHINE_MODE_10A_OUT,
	STATEMACHINE_MODE_RESISTANCE_1A, STATEMACHINE_MODE_RESISTANCE_1mA,
	STATEMACHINE_MODE_ISOMETER, STATEMACHINE_MODE_VOLTMETER,
	STATEMACHINE_MODE_AMPMETER,
};
#define IDLE_MENU_MODE_COUNT (sizeof(idle_menu_modes) / sizeof(idle_menu_modes[0]))

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
		temp = idle_menu_modes[(tim_encoder_read() / 2) % IDLE_MENU_MODE_COUNT];
		if (temp != statemachine_handle.current_menu_index) {
			statemachine_handle.current_menu_index = temp;
			//display_show_menu();

		}
		if (ok_button_pressed == 1) {
			statemachine_switchfromIdle(statemachine_handle.current_menu_index);
		}

		if (esc_button_pressed == 1)
			statemachine_handle.current_mode = STATEMACHINE_MODE_SHUTDOWN;

		// CHARGE is auto-entered, not menu/button-selected: whenever idle and
		// a 15-25V supply is detected at the output with no BMS fault
		// latched, start charging on our own.
		if (adc_data.converted.v_term >= 15000 && adc_data.converted.v_term <= 25000
				&& !(bms.SafetyRegisters.safetyStatusA || bms.SafetyRegisters.safetyStatusB)) {
			statemachine_switchfromIdle(STATEMACHINE_MODE_CHARGE);
		}
		break;
	case STATEMACHINE_MODE_60V_OUT:
		if (out_button_pressed == 1) {
			ctrl_main_start_ctrl(statemachine_mode_to_ctrl_mode(statemachine_handle.current_mode));
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
			ctrl_main_start_ctrl(statemachine_mode_to_ctrl_mode(statemachine_handle.current_mode));
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

	case STATEMACHINE_MODE_AMPMETER:
		if (esc_button_pressed == 1) {
			statemachine_switchtoIdle();
		}
		break;

	case STATEMACHINE_MODE_CHARGE:
		// End-of-charge, BMS fault, or the external supply being removed
		// (14V exit vs. the 15V entry threshold - hysteresis so the mode
		// doesn't flap right at the boundary) all stop charging the same
		// way. BMS data only refreshes once a second (EVENT_BMS_TIMER)
		// regardless, so checking it here at the 100ms tick loses nothing
		// vs. checking in the fast ADC-ISR-driven control loop.
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
		ctrl_main_start_ctrl(statemachine_mode_to_ctrl_mode(statemachine_handle.current_mode));
		aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 1);

		//display_draw_basic_layout();
		//display_draw_measure_layout();
		//display_draw_measure_data();
		break;

	case STATEMACHINE_MODE_RESISTANCE_1mA:
		ui_ctrl_ledOutOn();

		statemachine_handle.current_mode = mode;
		ctrl_main_start_ctrl(statemachine_mode_to_ctrl_mode(statemachine_handle.current_mode));
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

	case STATEMACHINE_MODE_CHARGE:
		// Interlock (refuse if a fault is already latched) lives in the
		// auto-detect check in statemachine_step()'s IDLE case, since entry
		// here *is* the auto-detect firing - starts immediately, like
		// RESISTANCE_1A/1mA, since charging isn't a "hold a button" action.
		ui_ctrl_ledOutOn();
		statemachine_handle.current_mode = mode;
		ctrl_main_start_ctrl(statemachine_mode_to_ctrl_mode(statemachine_handle.current_mode));
		aux_io_ctrl_manual_set_io(GPIO_CONV_CTRL_EN, 1);
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
		hrtim_sek_force_short();
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
	hrtim_sek_restore(); // no-op unless AMPMETER left the SEK half-bridge shorted
	tim_encoder_reset(62);
	statemachine_handle.current_mode = STATEMACHINE_IDLE;
	ui_ctrl_ledOutOff();
	ui_ctrl_ledSenseOff();
//display_draw_basic_layout();

}
