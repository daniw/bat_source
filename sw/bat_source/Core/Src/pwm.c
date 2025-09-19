/*
 * pwm.c
 *
 *  Created on: Jul 30, 2025
 *      Author: ahorat
 */

#include "pwm.h"

typedef struct {
	uint16_t pwm_top;
	HRTIM_HandleTypeDef hrtim1;
} PWM_t;

PWM_t pwm_unit_1;

void pwm_init(uint16_t pwm_top) {

	pwm_unit_1.pwm_top = pwm_top;
	HRTIM_ADCTriggerCfgTypeDef pADCTriggerCfg = { 0 };
	HRTIM_TimeBaseCfgTypeDef pTimeBaseCfg = { 0 };
	HRTIM_TimerCfgTypeDef pTimerCfg = { 0 };
	HRTIM_CompareCfgTypeDef pCompareCfg = { 0 };
	HRTIM_OutputCfgTypeDef pOutputCfg = { 0 };

	/* USER CODE BEGIN HRTIM1_Init 1 */

	/* USER CODE END HRTIM1_Init 1 */
	pwm_unit_1.hrtim1.Instance = HRTIM1;
	pwm_unit_1.hrtim1.Init.HRTIMInterruptResquests = HRTIM_IT_NONE;
	pwm_unit_1.hrtim1.Init.SyncOptions = HRTIM_SYNCOPTION_NONE;
	if (HAL_HRTIM_Init(&pwm_unit_1.hrtim1) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_HRTIM_DLLCalibrationStart(&pwm_unit_1.hrtim1,
			HRTIM_CALIBRATIONRATE_14) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_HRTIM_PollForDLLCalibration(&pwm_unit_1.hrtim1, 10) != HAL_OK) {
		Error_Handler();
	}
	pADCTriggerCfg.UpdateSource = HRTIM_ADCTRIGGERUPDATE_TIMER_B;
	pADCTriggerCfg.Trigger = HRTIM_ADCTRIGGEREVENT24_TIMERB_CMP2;
	if (HAL_HRTIM_ADCTriggerConfig(&pwm_unit_1.hrtim1, HRTIM_ADCTRIGGER_2,
			&pADCTriggerCfg) != HAL_OK) {
		Error_Handler();
	}
	pTimeBaseCfg.Period = pwm_unit_1.pwm_top;
	pTimeBaseCfg.RepetitionCounter = 5 - 1;
	pTimeBaseCfg.PrescalerRatio = HRTIM_PRESCALERRATIO_MUL16;
	pTimeBaseCfg.Mode = HRTIM_MODE_CONTINUOUS; // HRTIM_MODE_CONTINUOUS;HRTIM_MODE_SINGLESHOT
	if (HAL_HRTIM_TimeBaseConfig(&pwm_unit_1.hrtim1, HRTIM_TIMERINDEX_MASTER,
			&pTimeBaseCfg) != HAL_OK) {
		Error_Handler();
	}
	pTimerCfg.InterruptRequests = HRTIM_MASTER_IT_NONE;
	pTimerCfg.DMARequests = HRTIM_MASTER_DMA_NONE;
	pTimerCfg.DMASrcAddress = 0x0000;
	pTimerCfg.DMADstAddress = 0x0000;
	pTimerCfg.DMASize = 0x1;
	pTimerCfg.HalfModeEnable = HRTIM_HALFMODE_DISABLED;
	pTimerCfg.StartOnSync = HRTIM_SYNCSTART_DISABLED;
	pTimerCfg.ResetOnSync = HRTIM_SYNCRESET_DISABLED;
	pTimerCfg.DACSynchro = HRTIM_DACSYNC_NONE;
	pTimerCfg.PreloadEnable = HRTIM_PRELOAD_DISABLED;
	pTimerCfg.UpdateGating = HRTIM_UPDATEGATING_INDEPENDENT;
	pTimerCfg.BurstMode = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
	pTimerCfg.RepetitionUpdate = HRTIM_UPDATEONREPETITION_DISABLED;
	if (HAL_HRTIM_WaveformTimerConfig(&pwm_unit_1.hrtim1,
			HRTIM_TIMERINDEX_MASTER, &pTimerCfg) != HAL_OK) {
		Error_Handler();
	}
	pTimeBaseCfg.Mode = HRTIM_MODE_CONTINUOUS;
	if (HAL_HRTIM_TimeBaseConfig(&pwm_unit_1.hrtim1, HRTIM_TIMERINDEX_TIMER_B,
			&pTimeBaseCfg) != HAL_OK) {
		Error_Handler();
	}
	pTimerCfg.InterruptRequests = HRTIM_TIM_IT_NONE;
	pTimerCfg.DMARequests = HRTIM_TIM_DMA_NONE;
	pTimerCfg.PushPull = HRTIM_TIMPUSHPULLMODE_DISABLED;
	pTimerCfg.FaultEnable = HRTIM_TIMFAULTENABLE_NONE;
	pTimerCfg.FaultLock = HRTIM_TIMFAULTLOCK_READWRITE;
	pTimerCfg.DeadTimeInsertion = HRTIM_TIMDEADTIMEINSERTION_DISABLED;
	pTimerCfg.DelayedProtectionMode =
	HRTIM_TIMER_A_B_C_DELAYEDPROTECTION_DISABLED;
	pTimerCfg.UpdateTrigger = HRTIM_TIMUPDATETRIGGER_NONE;
	pTimerCfg.ResetTrigger = HRTIM_TIMRESETTRIGGER_NONE;
	pTimerCfg.ResetUpdate = HRTIM_TIMUPDATEONRESET_DISABLED;
	if (HAL_HRTIM_WaveformTimerConfig(&pwm_unit_1.hrtim1,
			HRTIM_TIMERINDEX_TIMER_B, &pTimerCfg) != HAL_OK) {
		Error_Handler();
	}
	pCompareCfg.CompareValue = 1000;
	if (HAL_HRTIM_WaveformCompareConfig(&pwm_unit_1.hrtim1,
			HRTIM_TIMERINDEX_TIMER_B,
			HRTIM_COMPAREUNIT_2, &pCompareCfg) != HAL_OK) {
		Error_Handler();
	}
	pCompareCfg.AutoDelayedMode = HRTIM_AUTODELAYEDMODE_REGULAR;
	pCompareCfg.AutoDelayedTimeout = 0x0000;

	if (HAL_HRTIM_WaveformCompareConfig(&pwm_unit_1.hrtim1,
			HRTIM_TIMERINDEX_TIMER_B,
			HRTIM_COMPAREUNIT_1, &pCompareCfg) != HAL_OK) {
		Error_Handler();
	}
	pOutputCfg.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;
	//pOutputCfg.SetSource = HRTIM_OUTPUTSET_RESYNC; // Use for single mode
	pOutputCfg.SetSource = HRTIM_OUTPUTSET_TIMPER;
	pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_TIMCMP1;
	pOutputCfg.IdleMode = HRTIM_OUTPUTIDLEMODE_NONE;
	pOutputCfg.IdleLevel = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
	pOutputCfg.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_NONE;
	pOutputCfg.ChopperModeEnable = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
	pOutputCfg.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;
	if (HAL_HRTIM_WaveformOutputConfig(&pwm_unit_1.hrtim1,
			HRTIM_TIMERINDEX_TIMER_B,
			HRTIM_OUTPUT_TB2, &pOutputCfg) != HAL_OK) {
		Error_Handler();
	}
	pOutputCfg.SetSource = HRTIM_OUTPUTSET_NONE;
	pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_TIMCMP2;
	if (HAL_HRTIM_WaveformOutputConfig(&pwm_unit_1.hrtim1,
			HRTIM_TIMERINDEX_TIMER_B,
			HRTIM_OUTPUT_TB1, &pOutputCfg) != HAL_OK) {
		Error_Handler();
	}

	HAL_HRTIM_MspPostInit(&pwm_unit_1.hrtim1);
	HAL_HRTIM_WaveformCountStart(&pwm_unit_1.hrtim1, HRTIM_TIMERID_TIMER_B); // Start the counter of the Timer A operating in waveform mode

}
void pwm_setduty_prim(float value) {
	HRTIM_CompareCfgTypeDef pCompareCfg = { 0 };

	if (value > 0.8f)
		value = 0.8f;
	else if (value < 0)
		value = 0;

	pCompareCfg.CompareValue = (uint16_t) (value * pwm_unit_1.pwm_top);

	if (pCompareCfg.CompareValue < 48)
		pCompareCfg.CompareValue = 48;

	if (pwm_unit_1.hrtim1.State == HAL_HRTIM_STATE_READY) {
		if (HAL_HRTIM_WaveformCompareConfig(&pwm_unit_1.hrtim1,
				HRTIM_TIMERINDEX_TIMER_B,
				HRTIM_COMPAREUNIT_1, &pCompareCfg) != HAL_OK) {
		}
	}

}
void pwm_enable(void) {
	//HAL_HRTIM_WaveformCountStop(&pwm_unit_1.hrtim1, HRTIM_TIMERID_TIMER_B);
	//HAL_HRTIM_WaveformCountStart(&pwm_unit_1.hrtim1, HRTIM_TIMERID_TIMER_B); // Start the counter of the Timer A operating in waveform mode
	HAL_HRTIM_WaveformOutputStart(&pwm_unit_1.hrtim1, HRTIM_OUTPUT_TB2); // Enable the generation of the waveform signal on the designated output
	//HRTIM1_COMMON->CR2 |= (0x0F << 8); // Use for single pulse

}

void pwm_disable(void) {
	HAL_HRTIM_WaveformOutputStop(&pwm_unit_1.hrtim1, HRTIM_OUTPUT_TB2); // Enable the generation of the waveform signal on the designated output

}
