/*
 */

#include "ctrl_PID_control.h"
#include "math.h"
#include "ctrl_param.h"

//#####################################################################################################################################################
// General purpose PID control with external derivative

/**
 * Initialises Control structure
 */
void ctrl_PID_controller_init(PID_controller_t *contr, float kp, float ki, float kd, float norm_in, float norm_out, float sat_high,
		float sat_low) {
	contr->ref = 0;
	contr->err = 0;
	contr->deriv = 0;

	contr->P_gain = kp;
	contr->I_gain = ki;
	contr->D_gain = kd;
	contr->D_filt = 0;
	contr->D_sigma = 1;

	contr->P_action = 0;
	contr->I_action = 0;
	contr->D_action = 0;
	contr->action = 0;

	contr->prev_ref = 0;
	contr->prev_I_action = 0;

	contr->norm_in = norm_in;
	contr->norm_out = norm_out;

	contr->sat_limit_high = sat_high;
	contr->sat_limit_low = sat_low;
	contr->sat_flag = 0;
}

/**
 * Executes the PID Controller
 * @param data_in_PI position used for PI part
 * @param data_in_D Speed used for calculating the D Part
 */
void ctrl_PID_controller_execute(PID_controller_t *contr, float data_in_PI, float data_in_D) {
	double derrivative_integrator;
	float d_factor1;
	float d_factor2;
	float sign = 0;

	contr->err = (contr->ref - data_in_PI) * contr->norm_in;

#ifdef FORWARD_EULER
	contr->D_action = (contr->err * contr->D_gain - contr->deriv ) * contr->D_filt;
	contr->deriv += contr->D_action / HW_CFG_POS_CTRL_FREQ;
#endif
#ifdef TRAPEZIODAL_INTEGRATOR
	d_factor1 = 1-0.5*contr->D_filt/HW_CFG_POS_CTRL_FREQ;
	d_factor2 = 1.0/(1.0+0.5*contr->D_filt/HW_CFG_POS_CTRL_FREQ);

	derrivative_integrator = contr->D_gain * contr->err * contr->D_filt - d_factor1*d_factor2 * contr->deriv;
	contr->D_action = d_factor2 * (derrivative_integrator-contr->deriv);
	contr->deriv = derrivative_integrator;
#endif
	contr->P_action = contr->P_gain * contr->err;
	if (contr->sat_flag == 0) {
		contr->I_action = (double) (contr->I_gain * contr->err) + contr->prev_I_action;
	}

	contr->action = contr->P_action + contr->I_action + contr->D_action;
	contr->action = contr->action * contr->norm_out;

	if (contr->action > contr->sat_limit_high) {
		contr->action = contr->sat_limit_high;
		contr->sat_flag = 1;
	} else if (contr->action < contr->sat_limit_low) {
		contr->action = contr->sat_limit_low;
		contr->sat_flag = 1;
	} else {
		contr->sat_flag = 0;
	}
	contr->prev_I_action = contr->I_action;
	contr->prev_ref = contr->ref;
	contr->I_action_out = contr->I_action;
}
