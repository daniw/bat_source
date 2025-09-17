/*
 * statemachine.h
 *
 *  Created on: Aug 9, 2025
 *      Author: ahorat
 */

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_

#include "stdint.h"

#define STATEMACHINE_STEP_PERIOD 100 //ms

typedef enum
{
	  STATEMACHINE_IDLE = 0,
	  STATEMACHINE_MODE_60V_OUT,
	  STATEMACHINE_MODE_10A_OUT,
	  STATEMACHINE_MODE_RESISTANCE_1A,
	  //STATEMACHINE_MODE_RESISTANCE_1mA,
	  STATEMACHINE_MODE_ISOMETER,
	  STATEMACHINE_MODE_VOLTMETER,
	  STATEMACHINE_MODE_SETTINGS,
	  STATEMACHINE_MODE_SHUTDOWN,
	  STATEMACHINE_MODE_RESERVED
} statemachine_modes_t;

typedef enum
{
	  STATEMACHINE_SETTINGS_MODE_MENU = 0,
	  STATEMACHINE_SETTINGS_MODE_BMS,
	  STATEMACHINE_SETTINGS_MODE_LENGTH
} statemachine_settings_modes_t;

typedef struct{
	statemachine_modes_t current_mode;
	uint8_t output_on;
	uint8_t current_menu_index;
	statemachine_settings_modes_t settings_mode;

}statemachine_t;


void statemachine_init(void);
void statemachine_step(void);

#endif /* INC_STATEMACHINE_H_ */
