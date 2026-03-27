/*
 * event.c
 *
 *  Created on: 24.11.2016
 *      Author: User
 */

#include "event.h"
#include "queue.h"
#include "error.h"

#define EVENT_TIMER_LOAD ((1 << 29) - 1)

struct event_queue{
	EVENT_STRUCT queue[EVENT_QUEUE_SIZE];
	uint8_t head;
	uint8_t tail;
};
struct event_queue event_queue;

//XScuTimer event_timer;
uint32_t event_old, event_new, event_per[EVENT_EVENT + 1];
EVENTS event_last, event_prelast;
uint8_t event_overflow;

/**
 * Callback of the event timer interrupt.
 * @param timer
 */
void event_CallBack(void)
{
	//event_Add(EVENT_EVENT, (void*) 0);
}

/**
 * Initializes the event queue and the event timer.
 * @return the initialization error code and 0 when there was no error
 */
int event_Init(void)
{
	uint8_t i;
	event_last = EVENT_INIT;
	event_old = EVENT_TIMER_LOAD;
	event_new = 0;
	for (i = 0; i < EVENT_EVENT + 1; i++)
	{
		event_per[i] = 0;
	}
	queue_Init(event_queue);

	//XScuTimer_EnableAutoReload(&event_timer);
	//XScuTimer_LoadTimer(&event_timer, EVENT_TIMER_LOAD);


	event_overflow = 0;
	return 0;
}

/**
 * Starts the event timer.
 */
void event_TimerStart(void)
{
	//XScuTimer_Start(&event_timer);
}

/**
 * Stops the event timer.
 */
void event_TimerStop(void)
{
	//XScuTimer_Stop(&event_timer);
}

/**
 * Calculates the CPU load for the different event tasks.
 * @param e event task which load is returned.
 * @return load of a specific event.
 */
uint32_t event_Timer(EVENTS e)
{
	uint8_t i;
	uint32_t ret = 0;
	for (i = 0; i < EVENT_EVENT + 1; i++)
	{
		event_per[i] >>= 1;
		if (i == e)
		{
			ret = event_per[i];
		}
	}
	return ret;
}

/**
 * Adds an event to the event queue.
 * @param e event to be added.
 * @param data event data.
 */
void event_Add(EVENTS e, void* callback, void* argument)
{
	int newhead, oldhead;
	do
	{
		oldhead = event_queue.head;
		newhead = (oldhead + 1) % EVENT_QUEUE_SIZE ;
		if ((((oldhead + 1) % EVENT_QUEUE_SIZE) == event_queue.tail))
		{
			if (event_overflow == 0)
			{
				error_Add(ERROR_EVENT_OVF, (event_old << 16) | (e));
				event_overflow = 1;
			}
			return;
		}
		event_overflow = 0;
		newhead = __sync_val_compare_and_swap(&event_queue.head, oldhead, newhead);
	} while (newhead != oldhead);
	event_queue.queue[oldhead].event = e;
	event_queue.queue[oldhead].callback = callback;
	event_queue.queue[oldhead].argument = argument;
}

/**
 * Returns the next event in the event queue.
 * @return event and event data
 */
EVENT_STRUCT event_Get(void)
{
	EVENT_STRUCT e;
	e.event = EVENT_VOID;
	if (queue_NotEmpty(event_queue))
	{
		e.event = queue_Get(event_queue).event;
		e.callback = queue_Get(event_queue).callback;
		e.argument = queue_Get(event_queue).argument;
		queue_Pop(event_queue, EVENT_QUEUE_SIZE);
	}
	event_Take(e.event);
	return e;
}

/**
 * Takes the CPU time in a Interrupt.
 * @param e event task.
 */
void event_Take(EVENTS e)
{
	if (e != event_last)
	{
		//event_new = XScuTimer_GetCounterValue(&event_timer);
		event_per[event_last] += (event_old - event_new) & EVENT_TIMER_LOAD;
		event_prelast = event_last;
		event_last = e;
		event_old = event_new;
	}
}

/**
 * Gives back the CPU from an interrupt.
 */
void event_Give(void)
{
	event_Take(event_prelast);
}
