/*
 * ctrl_main.h
 *
 *  Created on: Jul 30, 2025
 *      Author: ahorat
 */

#ifndef INC_CTRL_MAIN_H_
#define INC_CTRL_MAIN_H_

#include "ctrl_PID_control.h"
#include "adc.h"
#include "statemachine.h"

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
	uint32_t voltage_reference_mV;
	uint32_t current_reference_mA;
	float ramp;
	ctrl_mode_t mode;

} ctrl_main_t;

void ctrl_main_init(void);
void ctrl_main_start_ctrl(ctrl_mode_t mode);
void ctrl_main_ctrl(ADC_MEAS_DATA *adc_data);
void ctrl_main_stop_control(void);
ctrl_mode_t statemachine_mode_to_ctrl_mode(statemachine_modes_t mode);


#endif /* INC_CTRL_MAIN_H_ */
