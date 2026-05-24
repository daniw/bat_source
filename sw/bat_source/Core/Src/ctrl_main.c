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

ctrl_main_t ctrl_main_handle;
PID_controller_t ctrl_pi_voltage;
PID_controller_t ctrl_pi_current;

PID_controller_t ctrl_pi_flyback_voltage;
PID_controller_t ctrl_pi_flyback_current;

const uint16_t ctrl_main_iso_values[4] = { 125, 250, 500, 1000 };

/******************* Function Prototypes **************************/
void ctrl_main_apply_reference(uint16_t reference_poti_count);
void ctrl_main_ctrl_voltage(uint16_t voltage_meas_mV,
		int16_t voltage_meas_accurate);
void ctrl_main_ctrl_current(int16_t current_meas_mA,
		int16_t current_meas_accurate);

/**
 * Initializes the controllers
 */
void ctrl_main_init(void) {

	ctrl_PID_controller_init(&ctrl_pi_voltage, CTRL_PARAM_VOLTAGE_P,
	CTRL_PARAM_VOLTAGE_I, 0, CTRL_PARAM_VOLTAGE_DUTY_SAT_HIGH,
	CTRL_PARAM_VOLTAGE_DUTY_SAT_LOW);

	ctrl_PID_controller_init(&ctrl_pi_current, CTRL_PARAM_CURRENT_P,
	CTRL_PARAM_CURRENT_I, 0, CTRL_PARAM_CURRENT_DUTY_SAT_HIGH,
	CTRL_PARAM_CURRENT_DUTY_SAT_LOW);

	//ctrl_PID_controller_init(&ctrl_pi_flyback_voltage, CTRL_PARAM_HV_KP, CTRL_PARAM_BH_KI, 0, 0.5, 0);
	//ctrl_PID_controller_init(&ctrl_pi_flyback_current, kp, ki, kd, sat_high, sat_low)


}

void ctrl_main_start_ctrl(ctrl_mode_t mode) {
	switch (mode) {

	case CTRL_MODE_60V:
		hrtim_set_freq(HRTIM_CHANNEL_PRIM, CTRL_PARAM_SW_FREQ_LOW);
		hrtim_set_freq(HRTIM_CHANNEL_SEK, CTRL_PARAM_SW_FREQ_HIGH);
		hrtim_set_duty(HRTIM_CHANNEL_PRIM, CTRL_PARAM_CONST_DUTY_HIGH);
		hrtim_set_duty(HRTIM_CHANNEL_SEK, CTRL_PARAM_CONST_DUTY_HIGH);
		hrtim_enable(HRTIM_CHANNEL_PRIM);
		hrtim_enable(HRTIM_CHANNEL_SEK);
		break;
	case CTRL_MODE_RESISTANCE_1A:
	case CTRL_MODE_RESISTANCE_1mA:
	case CTRL_MODE_10A:
		ctrl_pi_voltage.prev_I_action = 0.0;
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
	case CTRL_MODE_RESISTANCE_1A:
	case CTRL_MODE_RESISTANCE_1mA:
		ctrl_main_ctrl_voltage(adc_data->converted.v_out,
				adc_data->converted.v_term_ext_mv);
		break;
	case CTRL_MODE_10A:
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

void ctrl_main_ctrl_voltage(uint16_t voltage_meas_mV,
		int16_t voltage_meas_accurate_mV) {
uint8_t duty;
	// Startup Ramp
	if (ctrl_main_handle.ramp > 1.0F) {
		ctrl_pi_voltage.ref = ctrl_main_handle.voltage_reference_mV / 1000.0F;
		ctrl_main_handle.ramp = 1.1F;
	}

	else {
		ctrl_main_handle.ramp += (10.0F / CTRL_FREQ);
		ctrl_pi_voltage.ref = (ctrl_main_handle.voltage_reference_mV / 1000.0F)
				* ctrl_main_handle.ramp;
	}

	ctrl_PID_controller_execute(&ctrl_pi_voltage, voltage_meas_mV / 1000.0F,
			voltage_meas_accurate_mV / 1000.0F, 0);

	// Apply Duty
	hrtim_set_duty(HRTIM_CHANNEL_PRIM, ctrl_pi_voltage.action);
	duty = ctrl_pi_voltage.action*100;
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
		ctrl_pi_voltage.ref = (ctrl_main_handle.voltage_reference_mV / 1000.0F)
				* ctrl_main_handle.ramp;
	}

	ctrl_PID_controller_execute(&ctrl_pi_voltage, voltage_meas_mV / 1000.0F,
			voltage_meas_mV / 1000.0F, 0);

	ctrl_pi_current.ref = ctrl_pi_voltage.action;
	ctrl_PID_controller_execute(&ctrl_pi_current, current_meas_mA / 1000.0F,
			current_meas_mA / 1000.0F, 0);
	// Apply Duty
	//hrtim_set_duty_pri(ctrl_pi_current.action);
}

