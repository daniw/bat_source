/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */



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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void SystemClock_Config(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DISPLAY_SCK_Pin GPIO_PIN_2
#define DISPLAY_SCK_GPIO_Port GPIOE
#define DISPLAY_WR_Pin GPIO_PIN_3
#define DISPLAY_WR_GPIO_Port GPIOE
#define DISPLAY_CS_Pin GPIO_PIN_4
#define DISPLAY_CS_GPIO_Port GPIOE
#define DISPLAY_MISO_Pin GPIO_PIN_5
#define DISPLAY_MISO_GPIO_Port GPIOE
#define DISPLAY_MOSI_Pin GPIO_PIN_6
#define DISPLAY_MOSI_GPIO_Port GPIOE
#define RESTRICT__No_toggle_Pin GPIO_PIN_13
#define RESTRICT__No_toggle_GPIO_Port GPIOC
#define BMS_CTRL_WAKEUP_Pin GPIO_PIN_9
#define BMS_CTRL_WAKEUP_GPIO_Port GPIOF
#define SHUNT_EN_Pin GPIO_PIN_0
#define SHUNT_EN_GPIO_Port GPIOC
#define OUT_SEL_HV_Pin GPIO_PIN_1
#define OUT_SEL_HV_GPIO_Port GPIOC
#define OVP_RESET_Pin GPIO_PIN_2
#define OVP_RESET_GPIO_Port GPIOC
#define OVP_N_Pin GPIO_PIN_3
#define OVP_N_GPIO_Port GPIOC
#define I_OUT_Pin GPIO_PIN_0
#define I_OUT_GPIO_Port GPIOA
#define V_TERM_Pin GPIO_PIN_1
#define V_TERM_GPIO_Port GPIOA
#define QSPI_CS__Pin GPIO_PIN_2
#define QSPI_CS__GPIO_Port GPIOA
#define QSPI_CLK_Pin GPIO_PIN_3
#define QSPI_CLK_GPIO_Port GPIOA
#define VREF_2_UC_Pin GPIO_PIN_4
#define VREF_2_UC_GPIO_Port GPIOA
#define I_1A_REF_Pin GPIO_PIN_5
#define I_1A_REF_GPIO_Port GPIOA
#define QSPI_IO3_Pin GPIO_PIN_6
#define QSPI_IO3_GPIO_Port GPIOA
#define QSPI_IO2_Pin GPIO_PIN_7
#define QSPI_IO2_GPIO_Port GPIOA
#define OCP_N_Pin GPIO_PIN_4
#define OCP_N_GPIO_Port GPIOC
#define DISCHARGE_N_Pin GPIO_PIN_5
#define DISCHARGE_N_GPIO_Port GPIOC
#define QSPI_IO1_Pin GPIO_PIN_0
#define QSPI_IO1_GPIO_Port GPIOB
#define QSPI_IO0_Pin GPIO_PIN_1
#define QSPI_IO0_GPIO_Port GPIOB
#define HV_CTRL_EN_Pin GPIO_PIN_2
#define HV_CTRL_EN_GPIO_Port GPIOB
#define CONV_CTRL_EN_Pin GPIO_PIN_7
#define CONV_CTRL_EN_GPIO_Port GPIOE
#define V_3V3_Pin GPIO_PIN_8
#define V_3V3_GPIO_Port GPIOE
#define ON_REQ_Pin GPIO_PIN_9
#define ON_REQ_GPIO_Port GPIOE
#define V_15V_Pin GPIO_PIN_10
#define V_15V_GPIO_Port GPIOE
#define V_VCC_Pin GPIO_PIN_11
#define V_VCC_GPIO_Port GPIOE
#define V_5V_Pin GPIO_PIN_12
#define V_5V_GPIO_Port GPIOE
#define V_IN_Pin GPIO_PIN_13
#define V_IN_GPIO_Port GPIOE
#define OUT_SEL_ISO_Pin GPIO_PIN_14
#define OUT_SEL_ISO_GPIO_Port GPIOE
#define V_OUT_Pin GPIO_PIN_15
#define V_OUT_GPIO_Port GPIOE
#define RESTRICT__No_toggleB10_Pin GPIO_PIN_10
#define RESTRICT__No_toggleB10_GPIO_Port GPIOB
#define RESTRICT__No_toggleB11_Pin GPIO_PIN_11
#define RESTRICT__No_toggleB11_GPIO_Port GPIOB
#define HV_CTRL_PRIM_Pin GPIO_PIN_12
#define HV_CTRL_PRIM_GPIO_Port GPIOB
#define RESTRICT__No_toggleB13_Pin GPIO_PIN_13
#define RESTRICT__No_toggleB13_GPIO_Port GPIOB
#define RESTRICT__No_toggleB14_Pin GPIO_PIN_14
#define RESTRICT__No_toggleB14_GPIO_Port GPIOB
#define V_HV_Pin GPIO_PIN_15
#define V_HV_GPIO_Port GPIOB
#define TEMP_CURRENT_Pin GPIO_PIN_8
#define TEMP_CURRENT_GPIO_Port GPIOD
#define CONV_CTRL_TEMP_PRIM_Pin GPIO_PIN_9
#define CONV_CTRL_TEMP_PRIM_GPIO_Port GPIOD
#define CONV_CTRL_TEMP_SEC_Pin GPIO_PIN_10
#define CONV_CTRL_TEMP_SEC_GPIO_Port GPIOD
#define V_3V3_A_Pin GPIO_PIN_11
#define V_3V3_A_GPIO_Port GPIOD
#define CONV_CTRL_TEMP_TRAFO_Pin GPIO_PIN_12
#define CONV_CTRL_TEMP_TRAFO_GPIO_Port GPIOD
#define SHUNT_ISO_EN_Pin GPIO_PIN_13
#define SHUNT_ISO_EN_GPIO_Port GPIOD
#define RESTRICT__No_toggleD14_Pin GPIO_PIN_14
#define RESTRICT__No_toggleD14_GPIO_Port GPIOD
#define I2C_ALERT_Pin GPIO_PIN_15
#define I2C_ALERT_GPIO_Port GPIOD
#define I2C_SCL_Pin GPIO_PIN_6
#define I2C_SCL_GPIO_Port GPIOC
#define I2C_SDA_Pin GPIO_PIN_7
#define I2C_SDA_GPIO_Port GPIOC
#define CONV_CTRL_SEC_L_Pin GPIO_PIN_8
#define CONV_CTRL_SEC_L_GPIO_Port GPIOC
#define CONV_CTRL_SEC_H_Pin GPIO_PIN_9
#define CONV_CTRL_SEC_H_GPIO_Port GPIOC
#define BMS_CTRL_I_BAT_Pin GPIO_PIN_8
#define BMS_CTRL_I_BAT_GPIO_Port GPIOA
#define I_ISO_Pin GPIO_PIN_9
#define I_ISO_GPIO_Port GPIOA
#define CONV_CTRL_PRIM_L_Pin GPIO_PIN_10
#define CONV_CTRL_PRIM_L_GPIO_Port GPIOA
#define CONV_CTRL_PRIM_H_Pin GPIO_PIN_11
#define CONV_CTRL_PRIM_H_GPIO_Port GPIOA
#define ADC_CLK_Pin GPIO_PIN_12
#define ADC_CLK_GPIO_Port GPIOA
#define SWD_SWDAT_Pin GPIO_PIN_13
#define SWD_SWDAT_GPIO_Port GPIOA
#define SWD_SWCLK_Pin GPIO_PIN_14
#define SWD_SWCLK_GPIO_Port GPIOA
#define SPI_CS_Pin GPIO_PIN_15
#define SPI_CS_GPIO_Port GPIOA
#define SPI_SCK_Pin GPIO_PIN_10
#define SPI_SCK_GPIO_Port GPIOC
#define SPI_MISO_Pin GPIO_PIN_11
#define SPI_MISO_GPIO_Port GPIOC
#define SPI_MOSI_Pin GPIO_PIN_12
#define SPI_MOSI_GPIO_Port GPIOC
#define ADC_SYNC_RESET_N_Pin GPIO_PIN_0
#define ADC_SYNC_RESET_N_GPIO_Port GPIOD
#define ADC_DRDY_N_Pin GPIO_PIN_2
#define ADC_DRDY_N_GPIO_Port GPIOD
#define ADC_DRDY_N_EXTI_IRQn EXTI2_IRQn
#define BUTTON_ESC_Pin GPIO_PIN_4
#define BUTTON_ESC_GPIO_Port GPIOD
#define UART_TX_Pin GPIO_PIN_5
#define UART_TX_GPIO_Port GPIOD
#define UART_RX_Pin GPIO_PIN_6
#define UART_RX_GPIO_Port GPIOD
#define SWD_SWO_Pin GPIO_PIN_3
#define SWD_SWO_GPIO_Port GPIOB
#define UI_PRESENT_Pin GPIO_PIN_5
#define UI_PRESENT_GPIO_Port GPIOB
#define ENCODER_B_Pin GPIO_PIN_6
#define ENCODER_B_GPIO_Port GPIOB
#define ENCODER_A_Pin GPIO_PIN_7
#define ENCODER_A_GPIO_Port GPIOB
#define BOOT0_Pin GPIO_PIN_8
#define BOOT0_GPIO_Port GPIOB
#define BUTTON_OUT_Pin GPIO_PIN_9
#define BUTTON_OUT_GPIO_Port GPIOB
#define BUTTON_OK_Pin GPIO_PIN_1
#define BUTTON_OK_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

#define PCA9554_ADDRESS_HW_REV 0x70
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
