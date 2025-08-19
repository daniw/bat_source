/*
 * ctrl_main.h
 *
 *  Created on: Jul 30, 2025
 *      Author: ahorat
 */

#ifndef INC_CTRL_MAIN_H_
#define INC_CTRL_MAIN_H_

#include "ctrl_PID_control.h"

typedef struct{
	uint16_t poti_reference;

} ctrl_main_t;

void ctrl_main_init(void);
void ctrl_main_ctrl_voltage(uint16_t voltage_meas);
#endif /* INC_CTRL_MAIN_H_ */
