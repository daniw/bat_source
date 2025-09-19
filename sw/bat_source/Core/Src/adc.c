/*
 * adc.c
 *
 *  Created on: May 27, 2025
 *      Author: ahorat
 */
#include "adc.h"

ADC_MEAS_DATA adc_data;

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;

TIM_HandleTypeDef htim1;

static void adc_Init1(void);
static void adc_Init2(void);
void adc_init_encoder(void);


void adc_init(int32_t* ext_adc_data)
{
	adc_data.ext_adc_data = ext_adc_data;
	adc_Init1();
	adc_Init2();
	adc_init_encoder();

	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);

}


void adc_start(void){

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_data.v_hv_raw, 3);
	HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&adc_data.i_out_raw, 6);
	HAL_ADCEx_InjectedStart_IT(&hadc1);

}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void adc_Init1(void)
{

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};
  ADC_InjectionConfTypeDef sConfigInjected = {0};


  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.LowPowerAutoWait = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Injected Channel
  */
  sConfigInjected.InjectedSingleDiff = ADC_SINGLE_ENDED;
  sConfigInjected.InjectedNbrOfConversion = 3;
  sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONV_EDGE_RISING;
  sConfigInjected.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJECCONV_HRTIM_TRG2;
  sConfigInjected.AutoInjectedConv = DISABLE;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.QueueInjectedContext = DISABLE;
  sConfigInjected.InjectedOffset = 0;
  sConfigInjected.InjectedOffsetNumber = ADC_OFFSET_NONE;

  sConfigInjected.InjectedChannel = ADC_CHANNEL_1;
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_1;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Injected Channel
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_3;
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_2;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Injected Channel
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_4;
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_3;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }

  adc_data.v_in_offset   = ADC_VIN_OFFSET_MV  ;
  adc_data.v_out_offset  = ADC_VOUT_OFFSET_MV ;
  adc_data.v_term_offset = ADC_VTERM_OFFSET_MV;
  adc_data.v_hv_offset   = ADC_VHV_OFFSET_MV  ;
  adc_data.i_bat_offset  = ADC_IBAT_OFFSET_MA ;


}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void adc_Init2(void)
{


  ADC_ChannelConfTypeDef sConfig = {0};


  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 6;
  hadc2.Init.DMAContinuousRequests = ENABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}


void adc_convert_data(void){
	adc_data.v_in_mV   = (adc_data.v_in_raw   - adc_data.v_in_offset  )* ADC_VIN_GAIN_MV;
	adc_data.v_out_mV  = (adc_data.v_out_raw  - adc_data.v_out_offset )* ADC_VOUT_GAIN_MV;
	adc_data.v_term_mV = (adc_data.v_term_raw - adc_data.v_term_offset)* ADC_VTERM_GAIN_MV;
	adc_data.v_hv_mV   = (adc_data.v_hv_raw   - adc_data.v_hv_offset  )* ADC_VHV_GAIN_MV;
	adc_data.i_bat_mA  = (adc_data.i_bat_raw  - adc_data.i_bat_offset )* ADC_IBAT_GAIN_MA;

	adc_data.v_sens_ext_uv = adc_data.ext_adc_data[1] * ADC_EXT_VSENS_GAIN_UV;
	adc_data.i_iso_ext_uA  = adc_data.ext_adc_data[2] * ADC_EXT_IISO_GAIN_UA;

	//HAL_ADC_Stop_DMA(&hadc1);
	//HAL_ADC_Stop_DMA(&hadc2);
    //
	//HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	//HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
	//adc_start();
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{

	//adc_data.v_in_raw = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
	adc_data.i_bat_raw = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_2);
	adc_data.v_out_raw = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_3);
	//adc_data.v_in_mV   = (adc_data.v_in_raw   - adc_data.v_in_offset  ) * ADC_VIN_GAIN_MV;
	adc_data.i_bat_mA  = (adc_data.i_bat_raw  - adc_data.i_bat_offset ) * ADC_IBAT_GAIN_MA;
	adc_data.v_out_mV  = (adc_data.v_out_raw  - adc_data.v_out_offset ) * ADC_VOUT_GAIN_MV;
	adc_data.v_term_ext_mv = adc_data.ext_adc_data[0] * ADC_EXT_VTERM_GAIN_MV;
	//adc_data.v_term_ext_mv_filt = 0.9f*adc_data.v_term_ext_mv_filt + 0.1f*adc_data.v_term_ext_mv;
	adc_data.i_out_ext_mA  = adc_data.ext_adc_data[3] * ADC_EXT_IOUT_GAIN_mA;
	ctrl_main_ctrl_60v(adc_encoder_read(), adc_data.v_out_mV, adc_data.v_term_ext_mv, adc_data.i_bat_mA);
}


/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
void adc_init_encoder(void) {
	TIM_Encoder_InitTypeDef sConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };


	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 127;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}

	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
}

inline uint16_t adc_encoder_read(void){
	 return(htim1.Instance->CNT & 0xffff);
}

void adc_encoder_reset(uint8_t value) {
	htim1.Instance->CNT =value;
}
