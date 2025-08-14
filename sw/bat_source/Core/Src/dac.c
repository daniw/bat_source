/*
 * dac.c
 *
 *  Created on: May 25, 2025
 *      Author: Andreas
 */

#include "dac.h"

DAC_HandleTypeDef hdacRef;

TIM_HandleTypeDef htim1ARef;

/**
 * Initialise the DAC
 */
void dac_init(void) {

	DAC_ChannelConfTypeDef sConfig = { 0 };

	/** DAC Initialization
	 */
	hdacRef.Instance = DAC1;
	if (HAL_DAC_Init(&hdacRef) != HAL_OK) {
		Error_Handler();
	}

	/** DAC channel OUT1 config
	 */
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	if (HAL_DAC_ConfigChannel(&hdacRef, &sConfig, DAC_CHANNEL_1) != HAL_OK) {
		Error_Handler();
	}
	HAL_DAC_Start(&hdacRef, DAC_CHANNEL_1);

	  dac_setValueRef(2048);

	/*************** TIM15 *****************/

	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};
	  TIM_OC_InitTypeDef sConfigOC = {0};
	  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

	  htim1ARef.Instance = TIM15;
	  htim1ARef.Init.Prescaler = 0;
	  htim1ARef.Init.CounterMode = TIM_COUNTERMODE_UP;
	  htim1ARef.Init.Period = 4096;
	  htim1ARef.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htim1ARef.Init.RepetitionCounter = 0;
	  htim1ARef.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	  if (HAL_TIM_Base_Init(&htim1ARef) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(&htim1ARef, &sClockSourceConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  if (HAL_TIM_PWM_Init(&htim1ARef) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(&htim1ARef, &sMasterConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sConfigOC.OCMode = TIM_OCMODE_PWM1;
	  sConfigOC.Pulse = 0;
	  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	  if (HAL_TIM_PWM_ConfigChannel(&htim1ARef, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	  sBreakDeadTimeConfig.DeadTime = 0;
	  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	  sBreakDeadTimeConfig.BreakFilter = 0;
	  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1ARef, &sBreakDeadTimeConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  HAL_TIM_Base_Start_IT(&htim1ARef);
	  HAL_TIM_PWM_Start(&htim1ARef, TIM_CHANNEL_1);
	  dac_setValue1ARef(2048);
	  HAL_TIM_MspPostInit(&htim1ARef);

}

/**
 * Sets the DAC Value of the DAC1-1 used as reference
 * @param value 12 bit value for the DAC Output
 */
void dac_setValueRef(uint16_t value) {
	HAL_DAC_SetValue(&hdacRef, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
}

/**
 * Sets the DAC Value of the TIM15-1 PWM used as reference for the 1A
 * @param value 12 bit value for the DAC Output
 */
void dac_setValue1ARef(uint16_t value){
	  __HAL_TIM_SET_COMPARE(&htim1ARef, TIM_CHANNEL_1, (uint32_t)(value));
}
