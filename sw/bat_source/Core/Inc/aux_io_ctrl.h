/*
 * aux_io_ctrl.h
 *
 *  Created on: May 25, 2025
 *      Author: Andreas
 */

#ifndef SRC_AUX_IO_CTRL_H_
#define SRC_AUX_IO_CTRL_H_

#include "stdint.h"
#include "stm32f3xx_hal.h"
#include "stdio.h"
#include "driver_pca9554.h"

#define PCA9554_ADDRESS_HW_REV 0x40
#define PCA9554_ADDRESS_UI 0x42

void aux_io_ctrl_GPIO_Init(void);
/**
 * Prints all input pin states
 */
void aux_io_ctrl_print_inputs(void);

/**
 * Prints all output pin states
 */
void aux_io_ctrl_print_Output(void);

/**
 * Function used to manually set the GPIOs from CLI
 * @param pin pin number as defined above
 * @param value value to set the pin (0,1)
 */
void aux_io_ctrl_manual_set_io(uint8_t pin, uint8_t value);



#endif /* SRC_AUX_IO_CTRL_H_ */
