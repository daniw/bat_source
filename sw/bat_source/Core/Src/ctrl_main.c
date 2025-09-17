/*
 * ctrl_main.c
 *
 *  Created on: Jul 30, 2025
 *      Author: ahorat
 */

#include "ctrl_main.h"
#include "ctrl_param.h"
#include "ctrl_PID_control.h"
#include "pwm.h"

ctrl_main_t ctrl_main_handle;
PID_controller_t ctrl_pi_voltage;
PID_controller_t ctrl_pi_current;

void ctrl_main_init(void) {

	ctrl_PID_controller_init(&ctrl_pi_voltage, CTRL_PARAM_VOLTAGE_P,
	CTRL_PARAM_VOLTAGE_I, 0, CTRL_PARAM_VOTLAGE_CURRENT_SAT_HIGH,
	CTRL_PARAM_VOTLAGE_CURRENT_SAT_LOW);

	ctrl_PID_controller_init(&ctrl_pi_current, CTRL_PARAM_CURRENT_P,
	CTRL_PARAM_CURRENT_I, 0, CTRL_PARAM_CURRENT_DUTY_SAT_HIGH,
	CTRL_PARAM_CURRENT_DUTY_SAT_LOW);
	pwm_setduty_prim(0.0F);

	ctrl_main_handle.ramp = 0.0F;

}

void ctrl_main_ctrl_60v(uint16_t reference, uint16_t voltage_meas,
		uint16_t voltage_meas_accurate, float current_meas) {

	ctrl_main_handle.poti_reference = reference;

	if (ctrl_main_handle.ramp > 1.0F) {
		ctrl_pi_voltage.ref = ctrl_main_handle.poti_reference / 2.0F;
		ctrl_main_handle.ramp = 1.1F;
	}

	else {
		ctrl_main_handle.ramp += 1 / 2000.0F;
		ctrl_pi_voltage.ref = (ctrl_main_handle.poti_reference / 2.0F)
				* ctrl_main_handle.ramp;
	}

	ctrl_PID_controller_execute(&ctrl_pi_voltage, voltage_meas / 1000.0F,
			voltage_meas / 1000.0F, 0);

	ctrl_pi_current.ref = ctrl_pi_voltage.action;
	ctrl_PID_controller_execute(&ctrl_pi_current, current_meas / 1000.0F,
			current_meas / 1000.0F, 0);
	// Apply Duty
	pwm_setduty_prim(ctrl_pi_current.action);
}

