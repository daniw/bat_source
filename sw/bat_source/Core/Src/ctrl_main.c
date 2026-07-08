/*
 * ctrl_main.c
 *
 *  Created on: Jul 30, 2025
 *      Author: ahorat
 */

#include "ctrl_main.h"
#include "ctrl_param.h"
#include "ctrl_PID_control.h"
#include "adc.h"
#include "hrtim.h"
#include <stdio.h>
#include "aux_io_ctrl.h"
#include "config_store.h"

ctrl_main_t ctrl_main_handle;
PID_controller_t ctrl_pi_voltage_buck;
PID_controller_t ctrl_pi_voltage_boost;
PID_controller_t ctrl_pi_current;

PID_controller_t ctrl_pi_flyback_voltage;
PID_controller_t ctrl_pi_flyback_current;

const uint16_t ctrl_main_iso_values[4] = { 125, 250, 500, 1000 };

/******************* Function Prototypes **************************/
void ctrl_main_apply_reference(uint16_t reference_poti_count);
void ctrl_main_ctrl_voltage_buck(uint16_t voltage_meas_mV,
		int16_t voltage_meas_accurate);
void ctrl_main_ctrl_voltage_boost(uint16_t voltage_meas_mV,
		int16_t voltage_meas_accurate);
void ctrl_main_ctrl_current(int16_t current_meas_mA,
		int16_t current_meas_accurate);

/**
 * Initializes the controllers
 */
void ctrl_main_init(void) {


	// P/I gains come from config_store, which is loaded from EEPROM (or
	// defaulted to the CTRL_PARAM_* constants) before ctrl_main_init() runs -
	// see config_store_init() in main(). Duty saturation limits stay as
	// hardcoded safety limits rather than field-tunable values.
	ctrl_PID_controller_init(&ctrl_pi_voltage_buck, config_store.calibration.voltage_buck_p,
	config_store.calibration.voltage_buck_i, 0, CTRL_PARAM_VOLTAGE_BUCK_DUTY_SAT_HIGH,
	CTRL_PARAM_VOLTAGE_BUCK_DUTY_SAT_LOW);



	ctrl_PID_controller_init(&ctrl_pi_voltage_boost, config_store.calibration.voltage_boost_p,
	config_store.calibration.voltage_boost_i, 0, CTRL_PARAM_VOLTAGE_BOOST_DUTY_SAT_HIGH,
	CTRL_PARAM_VOLTAGE_BOOST_DUTY_SAT_LOW);


	ctrl_PID_controller_init(&ctrl_pi_current, config_store.calibration.current_p,
	config_store.calibration.current_i, 0, CTRL_PARAM_CURRENT_DUTY_SAT_HIGH,
	CTRL_PARAM_CURRENT_DUTY_SAT_LOW);

	//ctrl_PID_controller_init(&ctrl_pi_flyback_voltage, CTRL_PARAM_HV_KP, CTRL_PARAM_BH_KI, 0, 0.5, 0);
	//ctrl_PID_controller_init(&ctrl_pi_flyback_current, kp, ki, kd, sat_high, sat_low)


}

/**
 * Maps a statemachine_modes_t to its ctrl_mode_t counterpart. The two enums
 * are positionally identical only up through ISOMETER and diverge after
 * that (STATEMACHINE_MODE_VOLTMETER=6 vs CTRL_MODE_CHARGE=6) - callers must
 * go through this instead of passing statemachine_handle.current_mode
 * directly to ctrl_main_start_ctrl(). Modes with no ctrl_mode_t counterpart
 * (AMPMETER, VOLTMETER, SETTINGS, SHUTDOWN, IDLE) correctly map to
 * CTRL_MODE_OFF, since none of them drive the control loop.
 * @param mode statemachine mode to translate.
 * @return the matching ctrl_mode_t, or CTRL_MODE_OFF if none applies.
 */
ctrl_mode_t statemachine_mode_to_ctrl_mode(statemachine_modes_t mode) {
	switch (mode) {
	case STATEMACHINE_MODE_60V_OUT:
		return CTRL_MODE_60V;
	case STATEMACHINE_MODE_10A_OUT:
		return CTRL_MODE_10A;
	case STATEMACHINE_MODE_RESISTANCE_1A:
		return CTRL_MODE_RESISTANCE_1A;
	case STATEMACHINE_MODE_RESISTANCE_1mA:
		return CTRL_MODE_RESISTANCE_1mA;
	case STATEMACHINE_MODE_ISOMETER:
		return CTRL_MODE_ISOMETER;
	case STATEMACHINE_MODE_CHARGE:
		return CTRL_MODE_CHARGE;
	default:
		return CTRL_MODE_OFF;
	}
}

void ctrl_main_start_ctrl(ctrl_mode_t mode) {
	switch (mode) {

	case CTRL_MODE_60V:
		ctrl_pi_voltage_boost.prev_I_action = 0.0;
		hrtim_set_freq(HRTIM_CHANNEL_PRIM, CTRL_PARAM_SW_FREQ_LOW);
		hrtim_set_freq(HRTIM_CHANNEL_SEK, CTRL_PARAM_SW_FREQ_HIGH);
		hrtim_set_duty(HRTIM_CHANNEL_PRIM, CTRL_PARAM_CONST_DUTY_LOW);
		hrtim_set_duty(HRTIM_CHANNEL_SEK, CTRL_PARAM_CONST_DUTY_LOW);
		hrtim_enable(HRTIM_CHANNEL_PRIM);
		hrtim_enable(HRTIM_CHANNEL_SEK);
		break;
	case CTRL_MODE_RESISTANCE_1A:
	case CTRL_MODE_RESISTANCE_1mA:
	case CTRL_MODE_10A:
	case CTRL_MODE_CHARGE:
		ctrl_pi_voltage_buck.prev_I_action = 0.0;
		ctrl_pi_current.prev_I_action = 0.0;
		hrtim_set_freq(HRTIM_CHANNEL_PRIM, CTRL_PARAM_SW_FREQ_HIGH);
		hrtim_set_freq(HRTIM_CHANNEL_SEK, CTRL_PARAM_SW_FREQ_LOW);
		hrtim_set_duty(HRTIM_CHANNEL_SEK, CTRL_PARAM_CONST_DUTY_LOW); // Needs to be low!!
		hrtim_set_duty(HRTIM_CHANNEL_PRIM, 0);
		hrtim_enable(HRTIM_CHANNEL_PRIM);
		hrtim_enable(HRTIM_CHANNEL_SEK);
		break;
	case CTRL_MODE_ISOMETER:
		break;

	case CTRL_MODE_OFF:
	default:
		break;
	}
	ctrl_main_handle.ramp = 0.0F;
	ctrl_main_handle.mode = mode;
}

void ctrl_main_stop_control(void) {
	ctrl_main_handle.mode = CTRL_MODE_OFF;
	hrtim_disable(HRTIM_CHANNEL_ALL);
}

/**
 * Main control task
 */
void ctrl_main_ctrl(ADC_MEAS_DATA *adc_data) {
	ctrl_main_apply_reference(ctrl_main_handle.poti_reference);
	switch (ctrl_main_handle.mode) {

	case CTRL_MODE_60V:
		ctrl_main_ctrl_voltage_boost(adc_data->converted.v_out,
				adc_data->converted.v_term_ext_mv);
		break;
	case CTRL_MODE_RESISTANCE_1A:
	case CTRL_MODE_RESISTANCE_1mA:
		ctrl_main_ctrl_voltage_buck(adc_data->converted.v_out,
				adc_data->converted.v_term_ext_mv);
		break;
	case CTRL_MODE_10A:
		ctrl_main_ctrl_current(adc_data->converted.i_out,
				adc_data->converted.i_out_ext_mA);

		break;
	case CTRL_MODE_CHARGE:
		// CC-then-stop: the end-of-charge/BMS-fault/voltage-removed stop
		// conditions are checked at the 100ms statemachine tick (BMS data
		// only refreshes once a second anyway), not here - this just runs
		// the same constant-current control law 10A_OUT uses.
		ctrl_main_ctrl_current(adc_data->converted.i_out,
				adc_data->converted.i_out_ext_mA);
		break;
	case CTRL_MODE_ISOMETER:
		//ctrl_main_ctrl_hv(adc_data->)
		break;

	case CTRL_MODE_OFF:
	default:
		break;
	}

}

/**
 * Apply the reference to the correct reference value depending on the control mode.
 * @param reference_poti_count Reference value of the poti (in count)
 */
void ctrl_main_apply_reference(uint16_t reference_poti_count) {
	switch (ctrl_main_handle.mode) {

	case CTRL_MODE_60V:
		ctrl_main_handle.voltage_reference_mV = reference_poti_count * 500; // 500mV Auflösung, Max range 63V
		break;
	case CTRL_MODE_10A:
		ctrl_main_handle.current_reference_mA = reference_poti_count * 100; // 100mA Auflösung, Max Range 10A
		break;
	case CTRL_MODE_CHARGE:
		ctrl_main_handle.current_reference_mA = CTRL_PARAM_CHARGE_CURRENT_mA;
		break;
	case CTRL_MODE_RESISTANCE_1A:
		ctrl_main_handle.voltage_reference_mV = CTRL_PARAM_IDLE_VOLTAGE_1A_mV;
		break;
	case CTRL_MODE_RESISTANCE_1mA:
		ctrl_main_handle.voltage_reference_mV = CTRL_PARAM_IDLE_VOLTAGE_1mA_mV;
		break;
	case CTRL_MODE_ISOMETER:
		ctrl_main_handle.voltage_iso_reference_V =
				ctrl_main_iso_values[reference_poti_count & 0x3];
		break;

	case CTRL_MODE_OFF:
	default:
		ctrl_main_handle.voltage_reference_mV = 0;
		ctrl_main_handle.current_reference_mA = 0;
		ctrl_main_handle.voltage_iso_reference_V = 0;
		break;
	}

}

void ctrl_main_ctrl_voltage_buck(uint16_t voltage_meas_mV,
		int16_t voltage_meas_accurate_mV) {
uint8_t duty;
	// Startup Ramp
	if (ctrl_main_handle.ramp > 1.0F) {
		ctrl_pi_voltage_buck.ref = ctrl_main_handle.voltage_reference_mV / 1000.0F;
		ctrl_main_handle.ramp = 1.1F;
	}

	else {
		ctrl_main_handle.ramp += (10.0F / CTRL_FREQ);
		ctrl_pi_voltage_buck.ref = (ctrl_main_handle.voltage_reference_mV / 1000.0F)
				* ctrl_main_handle.ramp;
	}

	ctrl_PID_controller_execute(&ctrl_pi_voltage_buck, voltage_meas_mV / 1000.0F,
			voltage_meas_accurate_mV / 1000.0F, 0);

	// Apply Duty
	hrtim_set_duty(HRTIM_CHANNEL_PRIM, ctrl_pi_voltage_buck.action);
	duty = ctrl_pi_voltage_buck.action*100;
	//if(ctrl_pi_voltage.action > 0.24F || ctrl_pi_voltage.action<0.24F)
	//	printf("%d\r\n", duty);
}


void ctrl_main_ctrl_voltage_boost(uint16_t voltage_meas_mV,
		int16_t voltage_meas_accurate_mV) {
uint8_t duty;
	// Startup Ramp
	if (ctrl_main_handle.ramp > 1.0F) {
		ctrl_pi_voltage_boost.ref = ctrl_main_handle.voltage_reference_mV / 1000.0F;
		hrtim_set_duty(HRTIM_CHANNEL_PRIM, CTRL_PARAM_CONST_DUTY_HIGH);
		ctrl_main_handle.ramp = 1.1F;
	}

	else {
		ctrl_main_handle.ramp += (10.0F / CTRL_FREQ);
		hrtim_set_duty(HRTIM_CHANNEL_PRIM, CTRL_PARAM_CONST_DUTY_HIGH*ctrl_main_handle.ramp);
		ctrl_pi_voltage_boost.ref = (ctrl_main_handle.voltage_reference_mV / 1000.0F)
				* ctrl_main_handle.ramp;
	}

	ctrl_PID_controller_execute(&ctrl_pi_voltage_boost, voltage_meas_mV / 1000.0F,
			voltage_meas_accurate_mV / 1000.0F, 0);

	// Apply Duty
	hrtim_set_duty(HRTIM_CHANNEL_SEK, ctrl_pi_voltage_boost.action);
	duty = ctrl_pi_voltage_boost.action*100;
	//if(ctrl_pi_voltage.action > 0.24F || ctrl_pi_voltage.action<0.24F)
	//	printf("%d\r\n", duty);
}

void ctrl_main_ctrl_current(int16_t current_meas_mA,
		int16_t current_meas_accurate) {

	// Startup Ramp
	if (ctrl_main_handle.ramp > 1.0F) {
		ctrl_pi_current.ref = ctrl_main_handle.current_reference_mA / 1000.0F;
		ctrl_main_handle.ramp = 1.1F;
	}

	else {
		ctrl_main_handle.ramp += (10.0F / CTRL_FREQ);
		ctrl_pi_current.ref = (ctrl_main_handle.current_reference_mA / 1000.0F)
				* ctrl_main_handle.ramp;
	}

	ctrl_PID_controller_execute(&ctrl_pi_current, current_meas_mA / 1000.0F,
			current_meas_accurate / 1000.0F, 0);

	// Apply Duty
	hrtim_set_duty(HRTIM_CHANNEL_PRIM, ctrl_pi_current.action);

}
void ctrl_main_ctrl_hv(int32_t voltage_meas_mV, int16_t current_meas_mA) {

	// Startup Ramp
	if (ctrl_main_handle.ramp > 1.0F) {
		ctrl_pi_flyback_voltage.ref = ctrl_main_handle.voltage_iso_reference_V
				/ 1000.0F;
		ctrl_main_handle.ramp = 1.1F;
	}

	else {
		ctrl_main_handle.ramp += (10.0F / CTRL_FREQ);
		ctrl_pi_flyback_voltage.ref = (ctrl_main_handle.voltage_reference_mV / 1000.0F)
				* ctrl_main_handle.ramp;
	}

	ctrl_PID_controller_execute(&ctrl_pi_flyback_voltage, voltage_meas_mV / 1000.0F,
			voltage_meas_mV / 1000.0F, 0);

	ctrl_pi_current.ref = ctrl_pi_flyback_voltage.action;
	ctrl_PID_controller_execute(&ctrl_pi_current, current_meas_mA / 1000.0F,
			current_meas_mA / 1000.0F, 0);
	// Apply Duty
	//hrtim_set_duty_pri(ctrl_pi_current.action);
}

