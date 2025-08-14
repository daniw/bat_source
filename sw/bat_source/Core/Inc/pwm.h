/*
 * pwm.h
 *
 *  Created on: Jul 30, 2025
 *      Author: ahorat
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#include "stm32f3xx_hal.h"

#define PWM_150KHZTOP 13653

void pwm_init(uint16_t top);
void pwm_setduty_prim(float value);
void pwm_enable(void);
void pwm_disable(void);

#endif /* INC_PWM_H_ */
