/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

COMP_HandleTypeDef hcomp4;
COMP_HandleTypeDef hcomp6;

DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c1_tx;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

BQ76905_handle bms;
ADS131M04_handle ext_adc;

extern ctrl_main_t ctrl_main_handle;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_DMA_Init(void);
static void MX_COMP4_Init(void);
static void MX_COMP6_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	aux_io_ctrl_GPIO_Init();

	aux_io_ctrl_manual_set_io(AUX_IO_ON_REQ, 1);
	aux_io_ctrl_manual_set_io(AUX_IO_I_1A_DIS, 1);
	MX_DMA_Init();
	adc_init(ext_adc.channelData);
	MX_COMP4_Init();
	MX_COMP6_Init();
	dac_init();
	i2c_Init();
	timer_init();

	aux_io_ctrl_PCA9554_Init();
	aux_io_ctrl_manual_set_io(AUX_IO_EXPANDER, 0);

	MX_SPI1_Init();
	MX_TIM3_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */
	cli_init(&huart1);
	pwm_init(PWM_150KHZTOP);
	HAL_Delay(1000);
	BQ76905_init(&bms, BQ76905_I2C_ADDRESS);
	ADS131M04_init(&ext_adc, &hspi1, SPI_NSS_GPIO_Port, SPI_NSS_Pin);
	ctrl_main_init();
	display_init();
	statemachine_init();
	timer_start();
	/* USER CODE END 2 */

	/* Infinite loop */

	EVENT_STRUCT current_event;

	while (1) {

		cli_loop();
		current_event = event_Get();
		switch (current_event.event) {
		case EVENT_IIC_RX:
		case EVENT_IIC_TX:
			if (current_event.callback != 0)
				current_event.callback(current_event.argument);
			break;
		case EVENT_SM_STEP:
			//BQ76905_readAllValuesAsync(&bms);
			statemachine_step();
			break;
		case EVENT_BMS_TIMER:
			BQ76905_readAllValuesAsync(&bms);
		default:
			break;
		}

		ctrl_main_handle.poti_reference = adc_encoder_read();

	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_HRTIM1
			| RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1 | RCC_PERIPHCLK_TIM1
			| RCC_PERIPHCLK_ADC12;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
	PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
	PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_PLLCLK;
	PeriphClkInit.Hrtim1ClockSelection = RCC_HRTIM1CLK_PLLCLK;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief COMP4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_COMP4_Init(void) {

	/* USER CODE BEGIN COMP4_Init 0 */

	/* USER CODE END COMP4_Init 0 */

	/* USER CODE BEGIN COMP4_Init 1 */

	/* USER CODE END COMP4_Init 1 */
	hcomp4.Instance = COMP4;
	hcomp4.Init.InvertingInput = COMP_INVERTINGINPUT_DAC1_CH1;
	hcomp4.Init.NonInvertingInput = COMP_NONINVERTINGINPUT_IO1;
	hcomp4.Init.Output = COMP_OUTPUT_NONE;
	hcomp4.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
	hcomp4.Init.BlankingSrce = COMP_BLANKINGSRCE_NONE;
	hcomp4.Init.TriggerMode = COMP_TRIGGERMODE_NONE;
	if (HAL_COMP_Init(&hcomp4) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN COMP4_Init 2 */

	/* USER CODE END COMP4_Init 2 */

}

/**
 * @brief COMP6 Initialization Function
 * @param None
 * @retval None
 */
static void MX_COMP6_Init(void) {

	/* USER CODE BEGIN COMP6_Init 0 */

	/* USER CODE END COMP6_Init 0 */

	/* USER CODE BEGIN COMP6_Init 1 */

	/* USER CODE END COMP6_Init 1 */
	hcomp6.Instance = COMP6;
	hcomp6.Init.InvertingInput = COMP_INVERTINGINPUT_DAC1_CH1;
	hcomp6.Init.NonInvertingInput = COMP_NONINVERTINGINPUT_IO1;
	hcomp6.Init.Output = COMP_OUTPUT_NONE;
	hcomp6.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
	hcomp6.Init.BlankingSrce = COMP_BLANKINGSRCE_NONE;
	hcomp6.Init.TriggerMode = COMP_TRIGGERMODE_NONE;
	if (HAL_COMP_Init(&hcomp6) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN COMP6_Init 2 */

	/* USER CODE END COMP6_Init 2 */

}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {

	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		Error_Handler();
	}
}



/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 8;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 4;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	/* USER CODE END TIM3_Init 2 */
	HAL_TIM_MspPostInit(&htim3);

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 921600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	//HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	//HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	/* DMA1_Channel2_IRQn interrupt configuration */
	//HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
	//HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
	/* DMA1_Channel3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
	/* DMA1_Channel4_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
	/* DMA1_Channel5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
	/* DMA1_Channel6_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
	/* DMA1_Channel7_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

// --------------------------------- Interrupt Handler --------------------------------

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
	ADS131M04_handleSPIComplete(&ext_adc);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == ADC_DRDY_Pin) {
		ADS131M04_handleDRDYInterrupt(&ext_adc);
	} else {
		__NOP();
	}
}

namespace std {
#ifdef __cplusplus
extern "C" {
#endif
int _write(int fd, char *ptr, int len) {
	(void) fd;
	HAL_UART_Transmit(&huart1, (uint8_t*) ptr, len, 100);
	return len;
}

//size_t _read(int fd, char *ptr, size_t len){
//  (void)fd;
//  size_t i;
//  for(i=0;i<len;i++){
//    *ptr++ = uart_read();
//    uart_write(*ptr++);  //For Terminal Echo
//
//    HAL_UART_Receive(&_huart, ptr, len);
//  }
//  return i;
//}

#ifdef __cplusplus
}
#endif
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
