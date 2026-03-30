/*
 * aux_io_ctrl.c
 *
 *  Created on: May 25, 2025
 *      Author: Andreas
 */


#include "aux_io_ctrl.h"
#include "main.h"



void aux_io_ctrl_GPIO_Init(void)
{

}

/**
 * Prints all input pin states
 */
void aux_io_ctrl_print_inputs(void){

}


/**
 * Function used to manually set the GPIOs from CLI
 * @param pin pin number as defined above
 * @param value value to set the pin (0,1)
 */
void aux_io_ctrl_manual_set_io(uint8_t pin, uint8_t value)
{
	switch (pin) {
	case GPIO_DISPLAY_WR:
		HAL_GPIO_WritePin(DISPLAY_WR_GPIO_Port, DISPLAY_WR_Pin, value);
		return;
	case GPIO_CONV_CTRL_EN:
		HAL_GPIO_WritePin(CONV_CTRL_EN_GPIO_Port, CONV_CTRL_EN_Pin, value);
		return;
	case GPIO_ON_REQ:
		HAL_GPIO_WritePin(ON_REQ_GPIO_Port, ON_REQ_Pin, value);
		return;
	case GPIO_OUT_SEL_ISO:
		HAL_GPIO_WritePin(OUT_SEL_ISO_GPIO_Port, OUT_SEL_ISO_Pin, value);
		return;
	case GPIO_BMS_CTRL_WAKEUP:
		HAL_GPIO_WritePin(BMS_CTRL_WAKEUP_GPIO_Port, BMS_CTRL_WAKEUP_Pin,value);
		return;
	case GPIO_SHUNT_EN:
		HAL_GPIO_WritePin(SHUNT_EN_GPIO_Port, SHUNT_EN_Pin, value);
		return;
	case GPIO_OUT_SEL_HV:
		HAL_GPIO_WritePin(OUT_SEL_HV_GPIO_Port, OUT_SEL_HV_Pin, value);
		return;
	case GPIO_OVP_RESET:
		HAL_GPIO_WritePin(OVP_RESET_GPIO_Port, OVP_RESET_Pin, value);
		return;
	case GPIO_DISCHARGE:
		HAL_GPIO_WritePin(DISCHARGE_N_GPIO_Port, DISCHARGE_N_Pin, value);
		return;
	case GPIO_HV_CTRL_EN:
		HAL_GPIO_WritePin(HV_CTRL_EN_GPIO_Port, HV_CTRL_EN_Pin, value);
		return;
	case GPIO_SHUNT_ISO_EN:
		HAL_GPIO_WritePin(SHUNT_ISO_EN_GPIO_Port, SHUNT_ISO_EN_Pin, value);
		return;
	case GPIO_ADC_SYNC_RESET:
		HAL_GPIO_WritePin(ADC_SYNC_RESET_N_GPIO_Port, ADC_SYNC_RESET_N_Pin, value);
		return;
	default:
			return;
	}
}



