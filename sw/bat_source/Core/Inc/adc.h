/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
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
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

//#include "ctrl_main.h"

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;

extern ADC_HandleTypeDef hadc2;

extern ADC_HandleTypeDef hadc3;

extern ADC_HandleTypeDef hadc4;

extern ADC_HandleTypeDef hadc5;

/* USER CODE BEGIN Private defines */

#define ADC_VIN_GAIN_MV    (2500* 21 /4096)
#define ADC_VHV_GAIN_MV    (2500 * 441 /4096)
#define ADC_VOUT_GAIN_MV   (2500 * 2731/4096/91)
#define ADC_VTERM_GAIN_MV  (2500 * 1001/4096/1.0417f)

#define ADC_IOUT_GAIN_MA   (2500*1000/43.33/4096) // ToDo: Calculate
#define ADC_IISO_GAIN_MA   0 // ToDo: Calculate
								// Empirical Value	// Calculated Value 	// Calculation
#define ADC_EXT_VTERM_GAIN_MV 	0.144499515f 		//0.1373291015625 		//(1200/8388608 * 14400/15) (ADC * Resistor divider) Max Value 1152 V
#define ADC_EXT_VSENS_GAIN_UV	0.7152557373046875f  //(1200/8388608 * 1 * 5)(ADC * ADC GAIN * Resistor divider) Max Value 6V
#define ADC_EXT_IISO_GAIN_UA 	0.0014305114746093f  //(1200/8388608 * 1 * 0.01) (ADC * ADC Gain * mA/mV)
#define ADC_EXT_IOUT_GAIN_mA 	0.001821494f			//0.00178813934326171875 	//(1200/8388608 * 12.5) (ADC * ADC Gain * mA/mV)

#define ADC_GAIN_V_3V3         2500*53/10/65536
#define ADC_GAIN_V_3V3A        2500*53/10/65536
#define ADC_GAIN_V_15V         2500*25/65536
#define ADC_GAIN_V_VCC         2500*19/65536
#define ADC_GAIN_V_5V          2500*78/10/65536
#define ADC_GAIN_V_BAT         (2500*3/4096)

#define ADC_IBAT_GAIN_MA  0.004761905f //(3300 *200 / 10/4096)

#define ADC_VIN_OFFSET_MV   0
#define ADC_VOUT_OFFSET_MV  0
#define ADC_VTERM_OFFSET_MV 2006
#define ADC_VHV_OFFSET_MV   0
#define ADC_IBAT_OFFSET_MA  1926
#define ADC_IOUT_OFFSET  2006


#define ADC_POTI_MAX 127

/* USER CODE END Private defines */

void MX_ADC1_Init(void);
void MX_ADC2_Init(void);
void MX_ADC3_Init(void);
void MX_ADC4_Init(void);
void MX_ADC5_Init(void);

/* USER CODE BEGIN Prototypes */

typedef struct {
	struct  {
		uint16_t v_in;		        // ADC3_IN3
		uint16_t v_hv;              // ADC4_IN5
		uint16_t v_term;            // ADC2_IN2
		uint16_t i_out;             // ADC1_IN1
		uint16_t i_iso;             // ADC5_IN2
		uint16_t v_out;             // ADC4_IN2
		uint16_t i_bat;             // ADC5_IN1

		uint16_t v_3v3; 		    // ADC5_IN6
		uint16_t temp_sec;      	// ADC5_IN7
		uint16_t v_3v3a;           	// ADC5_IN8
		uint16_t temp_trafo;   		// ADC5_IN9
		uint16_t temp_current;      // ADC5_IN12
		uint16_t temp_prim;         // ADC5_IN13
		uint16_t v_15v;             // ADC5_IN14
		uint16_t v_vcc;             // ADC5_IN15
		uint16_t v_5v;              // ADC5_IN16
		uint16_t int_temp;          // ADC5
		uint16_t v_bat;             // ADC5
		uint16_t v_ref_int;         // ADC5
	} raw;
	struct {
		uint16_t v_in;		        // ADC3_IN3
		uint32_t v_hv;              // ADC4_IN5
		int32_t v_term;            // ADC2_IN2
		uint16_t i_out;             // ADC1_IN1
		uint16_t i_iso;             // ADC5_IN2
		uint32_t v_out;             // ADC4_IN2
		uint16_t i_bat;             // ADC5_IN1

		uint16_t v_3v3; 		    // ADC5_IN6
		uint16_t temp_sec;      	// ADC5_IN7
		uint16_t v_3v3a;           	// ADC5_IN8
		uint16_t temp_trafo;   		// ADC5_IN9
		uint16_t temp_current;      // ADC5_IN12
		uint16_t temp_prim;         // ADC5_IN13
		uint16_t v_15v;             // ADC5_IN14
		uint16_t v_vcc;             // ADC5_IN15
		uint16_t v_5v;              // ADC5_IN16
		int16_t int_temp;          // ADC5
		uint16_t v_bat;             // ADC5
		uint16_t v_ref_int;         // ADC5
	} converted;


	uint16_t v_in_offset;
    uint16_t v_hv_offset;
    uint16_t i_bat_offset;
    uint16_t v_out_offset;
    uint16_t v_term_offset;
    uint16_t temp_int_offset;

    uint16_t i_out_offset;
    uint16_t temp_prim_offset;
    uint16_t temp_current_offset;
    uint16_t temp_sec_offset;
    uint16_t i_iso_offset;
    uint16_t temp_trafo_offset;

    int32_t* ext_adc_data;
    int32_t v_term_ext_mv;
    int32_t v_term_ext_mv_filt;
    int32_t v_sens_ext_uv;
    int32_t i_iso_ext_uA;
    int32_t i_out_ext_mA;

    uint16_t reference_poti;

    uint16_t r_mOhmx10;

    uint16_t vref_mV;

}ADC_MEAS_DATA;


void adc_init(int32_t* ext_adc_data);
void adc_start(void);
void adc_convert_data(void);
void adc_configure_mode(statemachine_modes_t mode);
uint16_t adc_encoder_read(void);
void adc_encoder_reset(uint8_t value);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

