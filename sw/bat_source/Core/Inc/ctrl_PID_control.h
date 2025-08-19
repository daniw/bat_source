/*
 */

#ifndef SRC_CTRL_PID_CONTROL_H_
#define SRC_CTRL_PID_CONTROL_H_
#include <stdint.h>

#define ctrl_PID_controller_execute_lin(contr,data_in_PI,data_in_D) ctrl_PID_controller_execute(contr,data_in_PI,data_in_D, 0)
#define ctrl_PID_controller_execute_rot(contr,data_in_PI,data_in_D) ctrl_PID_controller_execute(contr,data_in_PI,data_in_D, 1)

#define TRAPEZIODAL_INTEGRATOR

typedef struct
{
	float ref;
	float err;
	double deriv;

	float P_gain;
	float I_gain;
	float D_gain;
	float D_filt;
	float D_sigma;

	float P_action;
	float I_action_out;
	double I_action;
	float D_action;
	float action;

	float prev_ref;
	double prev_I_action;

	float norm_in;
	float norm_out;

	float sat_limit_high;
	float sat_limit_low;
	int    sat_flag;
} PID_controller_t;

void ctrl_PID_controller_init(PID_controller_t *contr, float kp, float ki, float kd, float norm_in, float norm_out, float sat_high, float sat_low);
void ctrl_PID_controller_execute(PID_controller_t *contr, float data_in_PI, float data_in_D);




#endif /* SRC_CTRL_PID_CONTROL_H_ */
