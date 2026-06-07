/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
     PE2   ------> SPI4_SCK
     PE4   ------> SPI4_NSS
     PE5   ------> SPI4_MISO
     PE6   ------> SPI4_MOSI
     PA0   ------> ADC1_IN1
     PA1   ------> ADC2_IN2
     PA4   ------> COMP_DAC11_group
     PA5   ------> COMP_DAC12_group
     PE8   ------> ADC5_IN6
     PE10   ------> ADC5_IN14
     PE11   ------> ADC5_IN15
     PE12   ------> ADC5_IN16
     PE13   ------> ADC3_IN3
     PE15   ------> ADC4_IN2
     PB12   ------> HRTIM1_CHC1
     PD8   ------> ADC5_IN12
     PD9   ------> ADC5_IN13
     PD10   ------> ADC5_IN7
     PD11   ------> ADC5_IN8
     PD12   ------> ADC5_IN9
     PC8   ------> HRTIM1_CHE1
     PC9   ------> HRTIM1_CHE2
     PA8   ------> ADC5_IN1
     PA9   ------> ADC5_IN2
     PA10   ------> HRTIM1_CHB1
     PA11   ------> HRTIM1_CHB2
     PA12   ------> S_TIM16_CH1
     PC10   ------> SPI3_SCK
     PC11   ------> SPI3_MISO
     PC12   ------> SPI3_MOSI
     PB6   ------> S_TIM4_CH1
     PB7   ------> S_TIM4_CH2
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, DISPLAY_WR_Pin|CONV_CTRL_EN_Pin|OUT_SEL_ISO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BMS_CTRL_WAKEUP_GPIO_Port, BMS_CTRL_WAKEUP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SHUNT_EN_Pin|DISCHARGE_N_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, OUT_SEL_HV_Pin|OVP_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(HV_CTRL_EN_GPIO_Port, HV_CTRL_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ON_REQ_GPIO_Port, ON_REQ_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, SHUNT_ISO_EN_Pin|ADC_SYNC_RESET_N_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DISPLAY_SCK_Pin DISPLAY_CS_Pin DISPLAY_MISO_Pin DISPLAY_MOSI_Pin */
  GPIO_InitStruct.Pin = DISPLAY_SCK_Pin|DISPLAY_CS_Pin|DISPLAY_MISO_Pin|DISPLAY_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : DISPLAY_WR_Pin */
  GPIO_InitStruct.Pin = DISPLAY_WR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(DISPLAY_WR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RESTRICT__No_toggle_Pin */
  GPIO_InitStruct.Pin = RESTRICT__No_toggle_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RESTRICT__No_toggle_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BMS_CTRL_WAKEUP_Pin */
  GPIO_InitStruct.Pin = BMS_CTRL_WAKEUP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BMS_CTRL_WAKEUP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SHUNT_EN_Pin OUT_SEL_HV_Pin OVP_RESET_Pin DISCHARGE_N_Pin */
  GPIO_InitStruct.Pin = SHUNT_EN_Pin|OUT_SEL_HV_Pin|OVP_RESET_Pin|DISCHARGE_N_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : OVP_N_Pin OCP_N_Pin */
  GPIO_InitStruct.Pin = OVP_N_Pin|OCP_N_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : I_OUT_Pin V_TERM_Pin VREF_2_UC_Pin I_1A_REF_Pin
                           BMS_CTRL_I_BAT_Pin I_ISO_Pin */
  GPIO_InitStruct.Pin = I_OUT_Pin|V_TERM_Pin|VREF_2_UC_Pin|I_1A_REF_Pin
                          |BMS_CTRL_I_BAT_Pin|I_ISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : HV_CTRL_EN_Pin */
  GPIO_InitStruct.Pin = HV_CTRL_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(HV_CTRL_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CONV_CTRL_EN_Pin ON_REQ_Pin OUT_SEL_ISO_Pin */
  GPIO_InitStruct.Pin = CONV_CTRL_EN_Pin|ON_REQ_Pin|OUT_SEL_ISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : V_3V3_Pin V_15V_Pin V_VCC_Pin V_5V_Pin
                           V_IN_Pin V_OUT_Pin */
  GPIO_InitStruct.Pin = V_3V3_Pin|V_15V_Pin|V_VCC_Pin|V_5V_Pin
                          |V_IN_Pin|V_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : RESTRICT__No_toggleB10_Pin RESTRICT__No_toggleB11_Pin RESTRICT__No_toggleB13_Pin RESTRICT__No_toggleB14_Pin */
  GPIO_InitStruct.Pin = RESTRICT__No_toggleB10_Pin|RESTRICT__No_toggleB11_Pin|RESTRICT__No_toggleB13_Pin|RESTRICT__No_toggleB14_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : HV_CTRL_PRIM_Pin */
  GPIO_InitStruct.Pin = HV_CTRL_PRIM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_HRTIM1;
  HAL_GPIO_Init(HV_CTRL_PRIM_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : TEMP_CURRENT_Pin CONV_CTRL_TEMP_PRIM_Pin CONV_CTRL_TEMP_SEC_Pin V_3V3_A_Pin
                           CONV_CTRL_TEMP_TRAFO_Pin RESTRICT__No_toggleD14_Pin */
  GPIO_InitStruct.Pin = TEMP_CURRENT_Pin|CONV_CTRL_TEMP_PRIM_Pin|CONV_CTRL_TEMP_SEC_Pin|V_3V3_A_Pin
                          |CONV_CTRL_TEMP_TRAFO_Pin|RESTRICT__No_toggleD14_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : SHUNT_ISO_EN_Pin */
  GPIO_InitStruct.Pin = SHUNT_ISO_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SHUNT_ISO_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : I2C_ALERT_Pin BUTTON_ESC_Pin */
  GPIO_InitStruct.Pin = I2C_ALERT_Pin|BUTTON_ESC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : CONV_CTRL_SEC_L_Pin CONV_CTRL_SEC_H_Pin */
  GPIO_InitStruct.Pin = CONV_CTRL_SEC_L_Pin|CONV_CTRL_SEC_H_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF3_HRTIM1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : CONV_CTRL_PRIM_L_Pin CONV_CTRL_PRIM_H_Pin */
  GPIO_InitStruct.Pin = CONV_CTRL_PRIM_L_Pin|CONV_CTRL_PRIM_H_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_HRTIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ADC_CLK_Pin */
  GPIO_InitStruct.Pin = ADC_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM16;
  HAL_GPIO_Init(ADC_CLK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_CS_Pin */
  GPIO_InitStruct.Pin = SPI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SPI_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI_SCK_Pin SPI_MISO_Pin SPI_MOSI_Pin */
  GPIO_InitStruct.Pin = SPI_SCK_Pin|SPI_MISO_Pin|SPI_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : ADC_SYNC_RESET_N_Pin */
  GPIO_InitStruct.Pin = ADC_SYNC_RESET_N_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ADC_SYNC_RESET_N_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ADC_DRDY_N_Pin */
  GPIO_InitStruct.Pin = ADC_DRDY_N_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ADC_DRDY_N_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : UI_PRESENT_Pin BOOT0_Pin BUTTON_OUT_Pin */
  GPIO_InitStruct.Pin = UI_PRESENT_Pin|BOOT0_Pin|BUTTON_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ENCODER_B_Pin ENCODER_A_Pin */
  GPIO_InitStruct.Pin = ENCODER_B_Pin|ENCODER_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_OK_Pin */
  GPIO_InitStruct.Pin = BUTTON_OK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON_OK_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);

}

/* USER CODE BEGIN 2 */

/*
 * Sends on-request signal to on/off controller
 */
void gpio_turnOn(void){
	HAL_GPIO_WritePin(ON_REQ_GPIO_Port, ON_REQ_Pin,1);
}

/*
 * Sends turn-off signal to on/off controller
 */
void gpio_turnOff(void){
	HAL_GPIO_WritePin(ON_REQ_GPIO_Port, ON_REQ_Pin,0);
}
/* USER CODE END 2 */
