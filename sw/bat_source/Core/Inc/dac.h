/*
 * dac.h
 *
 *  Created on: May 25, 2025
 *      Author: Andreas
 */

#ifndef INC_DAC_H_
#define INC_DAC_H_

#include "stm32f3xx_hal.h"

void dac_init(void);
void dac_setValueRef(uint16_t value);
void dac_setValue1ARef(uint16_t value);



#endif /* INC_DAC_H_ */
