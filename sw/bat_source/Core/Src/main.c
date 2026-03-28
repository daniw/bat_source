/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "hrtim.h"
#include "i2c.h"
#include "quadspi.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

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

/* USER CODE BEGIN PV */
BQ76905_handle bms;
opt3004_handle hamb;
lp581x_handle hled;
lp581x_handle hbacklight;
ADC_MEAS_DATA adc_data;
ADS131M04_handle ext_adc;
PCA9554_handle hpca_hw_rev;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

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
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_HRTIM1_Init();
  MX_QUADSPI1_Init();
  MX_SPI4_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_ADC3_Init();
  MX_ADC4_Init();
  MX_ADC5_Init();
  MX_DAC1_Init();
  MX_SPI3_Init();
  MX_I2C4_Init();
  MX_TIM4_Init();
  MX_TIM16_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  /*
   * DAC test setup
   */
  uint16_t dac_value = 0;
  dac_setValueRef2(2048);
  dac_setValue1ARef(dac_value);

  /*
   * CLI Setup
   */
  cli_init(&huart2);

  /*
   * LED test setup
   */

  lp581x_init(&hled, LP5817_ADDRESS);
  uint8_t led_pwms[] = {0xFF, 0xFF, 0xFF, 0xFF};
  lp581x_setPWMDimming(&hled, led_pwms);

  lp581x_init(&hbacklight, LP5816_ADDRESS);

  /*
   * Ambient light sensor test setup
   */

  opt3004_init(&hamb, OPT3004_DEVICE_ADDRESS_GND);
  pca9554_init(&hpca_hw_rev,PCA9554_ADDRESS_HW_REV, 0xFF);

  uint8_t amb_address = 0x44<<1;
  uint8_t amb_brightness_arr[] = {0, 0, 0, 0};
  uint16_t amb_brightness = 0;
  uint8_t amb_setup[] = {0x01, 0xC4, 0x10};
  i2c_WriteBlocking(amb_address, amb_setup, sizeof(amb_setup));
  uint8_t amb_dummy[] = {0x00};
  i2c_WriteBlocking(amb_address, amb_dummy, sizeof(amb_dummy));

  /*
   * Wakeup BMS and configure if necessary
   */
  GPIO_SET_BMS_CTRL_WAKEUP(1);
  HAL_Delay(100);
  GPIO_SET_BMS_CTRL_WAKEUP(0);
  BQ76905_init(&bms, BQ76905_I2C_ADDRESS);


  /*
   * PWM Init
   */

  hrtim_set_freq_prim(50000);
  hrtim_set_freq_sek(150000);
  hrtim_set_duty_pri(0.985);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	  /*
	   * DAC Test
	   */
	  if (dac_value + 256 >= 4096)
	  {
		  dac_value = 0;
	  }
	  else
	  {
		  dac_value += 256;
	  }
	  dac_setValue1ARef(dac_value);
	  /*
	   * LED test
	   */
	  /*if (led_current < 192)
	  {
		  led_current *= 2;
	  }
	  else
	  {
		  led_current = 3;
	  }
	  uint8_t led_currents[] = {0x14, led_current, led_current, led_current};
	  i2c_WriteBlocking(led_address, led_currents, sizeof(led_currents));
	  */

	  cli_loop();


	  /*
	   * Ambient light sensor test
	   */


	  /*amb_brightness = opt3004_readLux(&hamb);

	  led_current = amb_brightness/1000;

	  if (led_current > 200)
	  {
		  led_current = 200;
	  }
	  else if (amb_brightness < 1000)
	  {
		  led_current = 2;
	  }
	  uint8_t led_currents[] = {0x14, led_current, led_current/2, led_current/2};
	  lp581x_setAnalogDimming(&hled, 0, led_current);
	  lp581x_setAnalogDimming(&hled, 0, led_current/2);
	  lp581x_setAnalogDimming&(hled, 0, led_current/2);

	  i2c_WriteBlocking(led_address, led_currents, sizeof(led_currents));*/

	  /*
	   * Loop delay
	   */
	  HAL_Delay(1);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
