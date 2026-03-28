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

ctrl_main_t ctrl_main_handle;
PID_controller_t ctrl_pi_voltage;
PID_controller_t ctrl_pi_current;

const uint16_t ctrl_main_iso_values[4] = { 125, 250, 500, 1000 };

/******************* Function Prototypes **************************/
void ctrl_main_apply_reference(uint16_t reference_poti_count);


/**
 * Initializes the controllers
 */
void ctrl_main_init(void) {

	ctrl_PID_controller_init(&ctrl_pi_voltage, CTRL_PARAM_VOLTAGE_P,
	CTRL_PARAM_VOLTAGE_I, 0, CTRL_PARAM_VOTLAGE_CURRENT_SAT_HIGH,
	CTRL_PARAM_VOTLAGE_CURRENT_SAT_LOW);

	ctrl_PID_controller_init(&ctrl_pi_current, CTRL_PARAM_CURRENT_P,
	CTRL_PARAM_CURRENT_I, 0, CTRL_PARAM_CURRENT_DUTY_SAT_HIGH,
	CTRL_PARAM_CURRENT_DUTY_SAT_LOW);

	ctrl_main_handle.ramp = 0.0F;

}

/**
 * Main control task
 */
void ctrl_main_ctrl(ADC_MEAS_DATA *adc_data) {

	ctrl_main_apply_reference(adc_data->reference_poti);
	switch (ctrl_main_handle.mode) {

	case CTRL_MODE_60V:
	case CTRL_MODE_RESISTANCE_1A:
	case CTRL_MODE_RESISTANCE_1mA:
		ctrl_main_ctrl_60v(adc_data->v_out_mV, adc_data->v_term_ext_mv, adc_data->i_bat_mA);
		break;
	case CTRL_MODE_10A:
		// ToDo: Implement

		break;
	case CTRL_MODE_ISOMETER:
		// ToDo: Implement
		break;

	case CTRL_MODE_OFF:
	default:
		ctrl_main_handle.voltage_60v_reference_mV = 0;
		ctrl_main_handle.current_10A_reference_mA = 0;
		ctrl_main_handle.voltage_iso_reference_V = 0;
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
		ctrl_main_handle.voltage_60v_reference_mV = reference_poti_count * 500; // 500mV Auflösung, Max range 63V
		break;
	case CTRL_MODE_10A:
		ctrl_main_handle.current_10A_reference_mA = reference_poti_count * 100; // 100mA Auflösung, Max Range 10A
		break;
	case CTRL_MODE_RESISTANCE_1A:
		ctrl_main_handle.voltage_60v_reference_mV = CTRL_PARAM_IDLE_VOLTAGE_1A_mV;
		break;
	case CTRL_MODE_RESISTANCE_1mA:
		ctrl_main_handle.voltage_60v_reference_mV = CTRL_PARAM_IDLE_VOLTAGE_1mA_mV;
		break;
	case CTRL_MODE_ISOMETER:
		ctrl_main_handle.voltage_iso_reference_V = ctrl_main_iso_values[reference_poti_count & 0x3];
		break;

	case CTRL_MODE_OFF:
	default:
		ctrl_main_handle.voltage_60v_reference_mV = 0;
		ctrl_main_handle.current_10A_reference_mA = 0;
		ctrl_main_handle.voltage_iso_reference_V = 0;
		break;
	}

}

void ctrl_main_ctrl_60v(uint16_t voltage_meas_mV, uint16_t voltage_meas_accurate, float current_meas_mA) {

	// Startup Ramp
	if (ctrl_main_handle.ramp > 1.0F) {
		ctrl_pi_voltage.ref = ctrl_main_handle.voltage_60v_reference_mV / 1000.0F;
		ctrl_main_handle.ramp = 1.1F;
	}

	else {
		ctrl_main_handle.ramp += (10.0F / CTRL_FREQ);
		ctrl_pi_voltage.ref = (ctrl_main_handle.voltage_60v_reference_mV / 1000.0F) * ctrl_main_handle.ramp;
	}

	ctrl_PID_controller_execute(&ctrl_pi_voltage, voltage_meas_mV / 1000.0F, voltage_meas_mV / 1000.0F, 0);

	ctrl_pi_current.ref = ctrl_pi_voltage.action;
	ctrl_PID_controller_execute(&ctrl_pi_current, current_meas_mA / 1000.0F, current_meas_mA / 1000.0F, 0);
	// Apply Duty
	//hrtim_set_duty_pri(ctrl_pi_current.action);
}

