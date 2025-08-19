/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ds1309/driver_ssd1309_basic.h"
#include "cli.h"
#include <stdio.h>
#include "stdint.h"
#include "i2c.h"
#include "driver_pca9554.h"
#include "aux_io_ctrl.h"
#include "dac.h"
#include "bq76905.h"
#include "bq76905_config.h"
#include "adc.h"
#include "ADS131M04.h"
#include "pwm.h"
#include "ctrl_main.h"
#include "timer.h"
#include "statemachine.h"
#include "display.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_HRTIM_MspPostInit(HRTIM_HandleTypeDef *hhrtim);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BTN_OK_Pin GPIO_PIN_13
#define BTN_OK_GPIO_Port GPIOC
#define ENCODER_A_Pin GPIO_PIN_0
#define ENCODER_A_GPIO_Port GPIOC
#define ENCODER_B_Pin GPIO_PIN_1
#define ENCODER_B_GPIO_Port GPIOC
#define TEMP_PRIM_Pin GPIO_PIN_2
#define TEMP_PRIM_GPIO_Port GPIOC
#define BTN_ESC_Pin GPIO_PIN_3
#define BTN_ESC_GPIO_Port GPIOC
#define V_IN_Pin GPIO_PIN_0
#define V_IN_GPIO_Port GPIOA
#define V_HV_Pin GPIO_PIN_1
#define V_HV_GPIO_Port GPIOA
#define I_BAT_Pin GPIO_PIN_2
#define I_BAT_GPIO_Port GPIOA
#define V_OUT_Pin GPIO_PIN_3
#define V_OUT_GPIO_Port GPIOA
#define VREF_UC_Pin GPIO_PIN_4
#define VREF_UC_GPIO_Port GPIOA
#define I_OUT_Pin GPIO_PIN_4
#define I_OUT_GPIO_Port GPIOC
#define TEMP_CURRENT_Pin GPIO_PIN_5
#define TEMP_CURRENT_GPIO_Port GPIOC
#define TEMP_SEC_Pin GPIO_PIN_2
#define TEMP_SEC_GPIO_Port GPIOB
#define ADC_DRDY_Pin GPIO_PIN_10
#define ADC_DRDY_GPIO_Port GPIOB
#define ADC_DRDY_EXTI_IRQn EXTI15_10_IRQn
#define I_ISO_Pin GPIO_PIN_12
#define I_ISO_GPIO_Port GPIOB
#define V_TERM_Pin GPIO_PIN_13
#define V_TERM_GPIO_Port GPIOB
#define I_1A_REF_Pin GPIO_PIN_14
#define I_1A_REF_GPIO_Port GPIOB
#define TEMP_TRAFO_Pin GPIO_PIN_15
#define TEMP_TRAFO_GPIO_Port GPIOB
#define I_OUT_GAIN_Pin GPIO_PIN_6
#define I_OUT_GAIN_GPIO_Port GPIOC
#define CONF_CURR_Pin GPIO_PIN_7
#define CONF_CURR_GPIO_Port GPIOC
#define DISCHARGE_Pin GPIO_PIN_8
#define DISCHARGE_GPIO_Port GPIOC
#define CONF_ISO_Pin GPIO_PIN_9
#define CONF_ISO_GPIO_Port GPIOC
#define SHUNT_ISO_Pin GPIO_PIN_8
#define SHUNT_ISO_GPIO_Port GPIOA
#define ADC_SYNC_RESET_Pin GPIO_PIN_9
#define ADC_SYNC_RESET_GPIO_Port GPIOA
#define I_1A_DIS_Pin GPIO_PIN_12
#define I_1A_DIS_GPIO_Port GPIOA
#define SPI_NSS_Pin GPIO_PIN_15
#define SPI_NSS_GPIO_Port GPIOA
#define ON_REQ_Pin GPIO_PIN_10
#define ON_REQ_GPIO_Port GPIOC
#define UI_PRESENT_Pin GPIO_PIN_11
#define UI_PRESENT_GPIO_Port GPIOC
#define CROWBAR_LATCH_Pin GPIO_PIN_12
#define CROWBAR_LATCH_GPIO_Port GPIOC
#define CTRL_EN_Pin GPIO_PIN_2
#define CTRL_EN_GPIO_Port GPIOD
#define BTN_OUT_Pin GPIO_PIN_4
#define BTN_OUT_GPIO_Port GPIOB
#define I2C_ALERT_Pin GPIO_PIN_5
#define I2C_ALERT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
