/*
 * timer.c
 *
 *  Created on: 09.08.2025
 *      Author: ahorat
 */

#include "timer.h"
#include "error.h"
#include "stm32f3xx_hal.h"
#include "list.h"

/**
 * Struct that represents one timer event.
 */
typedef struct
{
	TIMER_TYPE type;
	uint8_t load;
	uint8_t count;
	EVENTS event;
	void* ret;
} TIMER_SUB;

/**
 * Struct the represent a timer instance and the event list.
 */
typedef struct
{
	TIM_HandleTypeDef* instance;
	uint32_t frq;
	TIMER_SUB* list[TIMER_SUB_COUNT];
	TIMER_SUB subs[TIMER_SUB_COUNT];
	LIST_STR liststr;
} timer_handle_t;

timer_handle_t timer_handle;
TIM_HandleTypeDef htim2;

/**
 * Callback from the timer interrupt. Rises the timer events.
 * @param timer pointer to the timer instance.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim != timer_handle.instance)
		return;
	uint8_t i;
	TIMER_SUB* sub;
	//event_Take(EVENT_TTC);
	for (i = 0; i < timer_handle.liststr.count; i++)
	{
		sub = list_Get(&(timer_handle.liststr), i);
		if (!list_CheckRemove(sub))
		{
			sub->count--;
			if (sub->count == 0)
			{
				event_Add(sub->event, 0,  sub->ret);
				switch (sub->type)
				{
					case TIMER_TYPE_TICK:
						sub->count = sub->load;
						break;
					case TIMER_TYPE_TIMEOUT:
						list_Remove(&timer_handle.liststr, i);
						break;
				}
			}
		}
	}
	//event_Give();
}

/**
 * Initializes a timer instance.
 * @param timer pointer to the timer instance.
 * @param id id of the timer.
 * @param frq frequency of the timer in MHz.
 * @return the initialization error code and 0 when there was no error
 */
void timer_init_hw(timer_handle_t* timer, uint32_t frq)
{
	  list_Init(&(timer->liststr), (void**) timer->list, timer->subs, sizeof(TIMER_SUB), TIMER_SUB_COUNT);
	  timer->frq=frq;
	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};
	  timer->instance->Instance = TIM2;
	  timer->instance->Init.Prescaler = 16000;
	  timer->instance->Init.CounterMode = TIM_COUNTERMODE_UP;
	  timer->instance->Init.Period = 4000/frq;
	  timer->instance->Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
	  timer->instance->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	  if (HAL_TIM_Base_Init(timer->instance) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(timer->instance, &sClockSourceConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(timer->instance, &sMasterConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

/**
 * Initializes all three timers.
 * @return the initialization error code and 0 when there was no error
 */
void timer_init(void)
{
	timer_handle.instance = &htim2;
	timer_init_hw(&timer_handle,TIMER_FREQ);
}

/**
 * Starts all three timers.
 */
void timer_start(void)
{
	HAL_TIM_Base_Start_IT(timer_handle.instance);
}

/**
 * Adds a timer event to a timer instance.
 * @param timer pointer to the timer instance.
 * @param load value for the divider.
 * @param type of the timer.
 * @param event the event that is risen when the timer elapsed.
 * @param ret return value that is added to the event.
 */
void timer_addTimer(timer_handle_t* timer, uint8_t load, TIMER_TYPE type, EVENTS event, void* ret)
{
	TIMER_SUB* new;
	new = list_GetFree(&(timer->liststr));
	if (!list_CheckRemove(new))
	{
		new->type = type;
		new->load = load;
		new->count = load;
		new->event = event;
		new->ret = ret;
		list_Add(&(timer->liststr), new);
	}
	else
	{
		error_Add(ERROR_TTC_OVF, event);
	}
}

/**
 * Finds the timer instance that fist the best to the timer event time and adds the event.
 * @param time of the timer event in ms.
 * @param type of the timer.
 * @param event the event that is risen when the timer elapsed.
 * @param ret return value that is added to the event.
 */
void timer_add(uint32_t time, TIMER_TYPE type, EVENTS event, void* ret)
{
	uint32_t load;
	load = time * timer_handle.frq / 1000;
	if (load > 0)
	{
		timer_addTimer(&(timer_handle), (uint8_t) load, type, event, ret);
		return;
	}
}

/**
 * Removes a timer event from the timer instance.
 * @param event timer event that is removed.
 */
void timer_remove(EVENTS event)
{
	uint8_t index;
	TIMER_SUB* sub;
		for (index = 0; index < timer_handle.liststr.count; index++)
		{
			sub = list_Get(&(timer_handle.liststr), index);
			if (!list_CheckRemove(sub))
			{
				if (sub->event == event)
				{
					list_Remove(&(timer_handle.liststr), index);
					return;
				}
			}
		}
}

/**
 * Removes a timer event from the timer instance.
 * @param event timer event that is removed.
 * @param ret return data that has to match too
 */
/*void timer_removeSelective(EVENTS event, void* ret)
{
	uint8_t i, index;
	TIMER_SUB* sub;
		for (index = 0; index < timer_handle.liststr.count; index++)
		{
			sub = list_Get(&(timer_handle.liststr), index);
			if (!list_CheckRemove(sub))
			{
				if (sub->event == event && sub->ret == ret)
				{
					list_Remove(&(timer_handle.liststr), index);
					return;
				}
			}
		}

}*/
