/*
 * i2c.h
 *
 *  Created on: May 25, 2025
 *      Author: Andreas
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "stm32f3xx_hal.h"
#include "queue.h"
#include "error.h"
#include "event.h"
/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
int i2c_Init(void);
void i2c_Start(void);
void i2c_Read(uint8_t address, uint8_t* buf, uint32_t numbytes,  void* callback, void* argument);
void i2c_Write(uint8_t address, uint8_t* buf, uint32_t numbytes, void* callback, void* argument);
uint8_t i2c_WriteBlocking(uint8_t address, uint8_t* buf, uint32_t numbytes);
uint8_t i2c_ReadBlocking(uint8_t address, uint8_t* buf, uint32_t numbytes);

#endif /* INC_I2C_H_ */
