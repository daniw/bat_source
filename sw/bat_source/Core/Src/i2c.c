/*
 * i2c.h
 *
 *  Created on: May 25, 2025
 *      Author: Andreas
 */


#include "i2c.h"

#define I2C_QUEUE_SIZE 64

/**
 * Enum to define the I2C communication direction.
 */
typedef enum
{
	I2CM_READ, //!< I2CM_READ
	I2CM_WRITE //!< I2CM_WRITE
} I2C_METHODE;

/**
 * Enum that represents the state in the work task.
 */
typedef enum
{
	WORK_I2C_IDLE,//!< WORK_I2C_IDLE
	WORK_I2C_RUN  //!< WORK_I2C_RUN
} WORK_I2C;

/**
 * Struct to serialize an I2C message.
 */
typedef struct
{
	I2C_METHODE methode;
	uint8_t address;
	uint32_t numbytes;
	uint8_t* buf;
	void* ret;
} I2C_STRUCT;


struct i2c_queue{
	I2C_STRUCT queue[I2C_QUEUE_SIZE];
	uint8_t head;
	uint8_t tail;
};
struct i2c_queue i2c_queue;

uint8_t i2c_head, i2c_tail;
WORK_I2C i2c_work;
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef *i2c_handle = &hi2c1;
volatile uint8_t i2c_exclusiveAccess;

/**
 * Works through the I2C queue and sends an I2C message.
 */
void i2c_Get(void)
{
	if (!i2c_exclusiveAccess && queue_NotEmpty(i2c_queue))
	{
		i2c_work = WORK_I2C_RUN;
		switch (queue_Get(i2c_queue).methode)
		{
			case I2CM_READ:
			HAL_I2C_Master_Receive_DMA(i2c_handle, queue_Get(i2c_queue).address,
					queue_Get(i2c_queue).buf, queue_Get(i2c_queue).numbytes);
			//printf("i2c.c: Read i2C Command\n\r");
			break;
			case I2CM_WRITE:
			HAL_I2C_Master_Transmit(i2c_handle, queue_Get(i2c_queue).address,
					queue_Get(i2c_queue).buf, queue_Get(i2c_queue).numbytes, 100);
			queue_Pop(i2c_queue, I2C_QUEUE_SIZE);
			i2c_work = WORK_I2C_IDLE;
			//printf("i2c.c: Write i2C Command\n\r");
			break;
		}
	}
	else
	{
		i2c_work = WORK_I2C_IDLE;
	}
}

/**
 * Adds an I2C message to the queue.
 * @param methode communication direction.
 * @param address I2C address of the target.
 * @param numbytes number of bytes to send or receive.
 * @param buf pointer to the data.
 * @param ret pointer that is returned to the callback to identify the caller.
 * @param wait can be set to one, if the add should be blocking.
 */
void i2c_Add(I2C_METHODE methode, uint8_t address, uint32_t numbytes,
		uint8_t *buf, void *ret, uint8_t wait) {
	if (queue_Full(i2c_queue, I2C_QUEUE_SIZE)) {
		if (wait) {
			while (queue_Full(i2c_queue, I2C_QUEUE_SIZE))
				HAL_Delay(1);

		} else {
			error_Add(ERROR_IIC_OVF, address);
			return;
		}
	}

	queue_Set(i2c_queue).methode = methode;
	queue_Set(i2c_queue).address = address;
	queue_Set(i2c_queue).numbytes = numbytes;
	queue_Set(i2c_queue).buf = buf;
	queue_Set(i2c_queue).ret = ret;
	queue_Push(i2c_queue, I2C_QUEUE_SIZE);

	if (i2c_work == WORK_I2C_IDLE) {
		i2c_Get();
	}
}

/**
 * Handles the interrupt, if read was successful
 * @param hi2c Handle to I2C interface
 */
void HAL_I2C_MasterRxCpltCallback (I2C_HandleTypeDef * hi2c){
	event_Add(EVENT_IIC_RX, queue_Get(i2c_queue).ret);
	queue_Pop(i2c_queue, I2C_QUEUE_SIZE);
	i2c_Get();
}

/**
 * Handles the interrupt, if write was successful
 * @param hi2c Handle to I2C interface
 */
void HAL_I2C_MasterTxCpltCallback (I2C_HandleTypeDef * hi2c){
	event_Add(EVENT_IIC_TX, queue_Get(i2c_queue).ret);
	queue_Pop(i2c_queue, I2C_QUEUE_SIZE);
	i2c_Get();
}

void HAL_I2C_ErrorCallback (I2C_HandleTypeDef * hi2c){
	event_Add(EVENT_IIC_ERROR, queue_Get(i2c_queue).ret);
	queue_Pop(i2c_queue, I2C_QUEUE_SIZE);
	i2c_Get();
}

/**
 * Initializes the I2C interface.
 * @param speed I2C clock frequency in Hz.
 * @return the initialization error code and 0 when there was no error
 */
int i2c_Init(void)
{
	queue_Init(i2c_queue);
	i2c_work = WORK_I2C_IDLE;
	i2c_exclusiveAccess = 0;
	  i2c_handle->Instance = I2C1;
	  i2c_handle->Init.Timing = 0x0010020A;
	  i2c_handle->Init.OwnAddress1 = 0;
	  i2c_handle->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	  i2c_handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	  i2c_handle->Init.OwnAddress2 = 0;
	  i2c_handle->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	  i2c_handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	  i2c_handle->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	  if (HAL_I2C_Init(i2c_handle) != HAL_OK)
	  {
	    error_Add(ERROR_IIC_INIT, 1);
	    return 1;
	  }

	  /** Configure Analogue filter
	  */
	  if (HAL_I2CEx_ConfigAnalogFilter(i2c_handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	  {
		    error_Add(ERROR_IIC_INIT, 2);
		    return 2;
	  }

	  /** Configure Digital filter
	  */
	  if (HAL_I2CEx_ConfigDigitalFilter(i2c_handle, 0) != HAL_OK)
	  {
		    error_Add(ERROR_IIC_INIT, 3);
		    return 3;
	  }
	  return 0;

}

/**
 * Enables the I2C interface.
 */
void i2c_Start(void)
{
	// Enable interrupt?
}

/**
 * Adds a read message to the queue.
 * @param address address I2C address of the target.
 * @param buf numbytes number of bytes to send or receive.
 * @param numbytes buf pointer to the data.
 * @param ret ret pointer that is returned to the callback to identify the caller.
 */
void i2c_Read(uint8_t address, uint8_t* buf, uint32_t numbytes, void* ret)
{
	i2c_Add(I2CM_READ, address, numbytes, buf, ret, 0);
}

/**
 * Adds a write message to the queue.
 * @param address address I2C address of the target.
 * @param buf numbytes number of bytes to send or receive.
 * @param numbytes buf pointer to the data.
 * @param ret ret pointer that is returned to the callback to identify the caller.
 */
void i2c_Write(uint8_t address, uint8_t* buf, uint32_t numbytes, void* ret)
{
	i2c_Add(I2CM_WRITE, address, numbytes, buf, ret, 0);
}

/**
 * Performs a blocking write to the target. Waits until current operation is finished.
 * @param address address I2C address of the target.
 * @param buf numbytes number of bytes to send or receive.
 * @param numbytes buf pointer to the data.
 * @param ret ret pointer that is returned to the callback to identify the caller.
 */
uint8_t i2c_WriteBlocking(uint8_t address, uint8_t* buf, uint32_t numbytes){
	uint8_t retval;

	i2c_exclusiveAccess = 1;
	while (i2c_work!=WORK_I2C_IDLE)
		HAL_Delay(1);
	retval = HAL_I2C_Master_Transmit(i2c_handle, address, buf, numbytes, 100);
	i2c_exclusiveAccess = 0;
	return retval;
}

/**
 * Performs a blocking read to the target. Waits until current operation is finished.
 * @param address address I2C address of the target.
 * @param buf numbytes number of bytes to send or receive.
 * @param numbytes buf pointer to the data.
 * @param ret ret pointer that is returned to the callback to identify the caller.
 */
uint8_t i2c_ReadBlocking(uint8_t address, uint8_t* buf, uint32_t numbytes){
	uint8_t retval;

	i2c_exclusiveAccess = 1;
	while (i2c_work!=WORK_I2C_IDLE)
		HAL_Delay(1);
	retval = HAL_I2C_Master_Receive(i2c_handle, address, buf, numbytes, 100);
	i2c_exclusiveAccess = 0;
	return retval;
}

