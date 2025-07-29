/*
 * event.h
 *
 *  Created on: 24.11.2016
 *      Author: User
 */

#ifndef SRC_EVENT_H_
#define SRC_EVENT_H_

#define EVENT_QUEUE_SIZE 64

#include "stdint.h"

/**
 * List of the possible events
 */
typedef enum
{
	EVENT_VOID,             //!< EVENT_VOID
	EVENT_INIT,             //!< EVENT_INIT
	EVENT_IIC_RX,           //!< EVENT_I2C_RX
	EVENT_IIC_TX,           //!< EVENT_I2C_TX
	EVENT_IIC_ERROR,        //!< EVENT_I2C_ERROR
	EVENT_EEPROM_READ,      //!< EVENT_EEPROM_READ
	EVENT_EEPROM_TIMEOUT,	//!< EVENT_EEPROM_TIMEOUT
	EVENT_EVENT             //!< EVENT_EVENT
} EVENTS;

/**
 * Struct that represents an event with the data.
 */
typedef struct
{
	EVENTS event;
	void(* callback)(void*);
	void* argument;
} EVENT_STRUCT;

int event_Init(void);
void event_TimerStart(void);
void event_TimerStop(void);
uint32_t event_Timer(EVENTS e);
void event_Add(EVENTS e, void* callback, void* argument);
EVENT_STRUCT event_Get(void);
void event_Take(EVENTS e);
void event_Give(void);

#endif /* SRC_EVENT_H_ */
