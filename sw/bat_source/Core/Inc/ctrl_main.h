/*
 * ctrl_main.h
 *
 *  Created on: Jul 30, 2025
 *      Author: ahorat
 */

#ifndef INC_CTRL_MAIN_H_
#define INC_CTRL_MAIN_H_

#include "ctrl_PID_control.h"

typedef enum {
	CTRL_MODE_OFF,
	CTRL_MODE_60V,
	CTRL_MODE_10A,
	CTRL_MODE_RESISTANCE_1A,
	CTRL_MODE_RESISTANCE_1mA,
	CTRL_MODE_ISOMETER,
	CTRL_MODE_CHARGE
} ctrl_mode_t;


typedef struct{
	uint16_t poti_reference;
	uint16_t voltage_iso_reference_V;
	uint32_t voltage_60v_reference_mV;
	uint32_t current_10A_reference_mA;
	float ramp;
	ctrl_mode_t mode;

} ctrl_main_t;

void ctrl_main_init(void);
void ctrl_main_ctrl_60v(uint16_t voltage_meas, uint16_t voltage_meas_accurate, float current_meas);
#endif /* INC_CTRL_MAIN_H_ */
