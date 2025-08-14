/*
 * ctrl_main.c
 *
 *  Created on: Jul 30, 2025
 *      Author: ahorat
 */

#include "ctrl_main.h"
#include "ctrl_param.h"
#include "ctrl_PID_control.h"

ctrl_main_t ctrl_main_handle;
PID_controller_t ctrl_pi_voltage;

void ctrl_main_init(void) {

	ctrl_PID_controller_init(&ctrl_pi_voltage, CTRL_PARAM_VOLTAGE_P,
			CTRL_PARAM_VOLTAGE_I, 0, 1.0, 1.0, CTRL_PARAM_DUTY_SAT_HIGH, CTRL_PARAM_DUTY_SAT_LOW);

}


void ctrl_main_ctrl_voltage(uint16_t voltage_meas)
{
	ctrl_pi_voltage.ref = ctrl_main_handle.poti_reference/2.0;
	ctrl_PID_controller_execute(&ctrl_pi_voltage, voltage_meas, voltage_meas);

	// Apply Duty
	//pwm_setduty_prim(ctrl_pi_voltage.action);
}

