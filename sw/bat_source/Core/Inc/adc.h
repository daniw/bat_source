/*
 * adc.h
 *
 *  Created on: May 27, 2025
 *      Author: ahorat
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_


#include "stm32f3xx_hal.h"

#define ADC_VIN_GAIN_MV    (3300* 210 /4096 /10)
#define ADC_VHV_GAIN_MV    (3300 * 960 /4096/1)
#define ADC_VOUT_GAIN_MV   30.61661342   //(3300 * 36400/4096/1300)
#define ADC_VTERM_GAIN_MV  (3300 * 14400/4096/15/1.0417)
								// Empirical Value	// Calculated Value 	// Calculation
#define ADC_EXT_VTERM_GAIN_MV 	0.144499515 		//0.1373291015625 		//(1200/8388608 * 14400/15) (ADC * Resistor divider) Max Value 1152 V
#define ADC_EXT_VSENS_GAIN_UV	0.7152557373046875  //(1200/8388608 * 1 * 5)(ADC * ADC GAIN * Resistor divider) Max Value 6V
#define ADC_EXT_IISO_GAIN_UA 	0.0014305114746093  //(1200/8388608 * 1 * 0.01) (ADC * ADC Gain * mA/mV)
#define ADC_EXT_IOUT_GAIN_mA 	0.001821494			//0.00178813934326171875 	//(1200/8388608 * 12.5) (ADC * ADC Gain * mA/mV)


#define ADC_IBAT_GAIN_MA  (3300 *200 / 10/4096)


typedef struct {
	uint16_t v_in_raw; 			// ADC1_IN1
	uint16_t i_bat_raw; 		// ADC1_IN3
	uint16_t v_out_raw; 		// ADC1_IN4
	uint16_t v_hv_raw; 			// ADC1_IN2
	uint16_t v_term_raw; 		// ADC1_IN13
	uint16_t temp_int_raw; 		// ADC1_INT

	uint16_t v_in_mV; 			// ADC1_IN1
	uint16_t v_hv_mV; 			// ADC1_IN2
	uint16_t i_bat_mA; 		// ADC1_IN3
	uint16_t v_out_mV; 		// ADC1_IN4
	uint16_t v_term_mV; 		// ADC1_IN13
	uint16_t temp_int_deg; 		// ADC1_INT

	uint16_t i_out_raw; 		// ADC2_IN5
	uint16_t temp_prim_raw; 	// ADC2_IN8
	uint16_t temp_current_raw; 	// ADC2_IN11
	uint16_t temp_sec_raw; 		// ADC2_IN12
	uint16_t i_iso_raw; 		// ADC2_IN13
	uint16_t temp_trafo_raw; 	// ADC2_IN15

	int16_t i_out_mA; 		// ADC2_IN5
	uint16_t temp_prim_deg; 	// ADC2_IN8
	uint16_t temp_current_deg; 	// ADC2_IN11
	uint16_t temp_sec_deg; 		// ADC2_IN12
	int16_t i_iso_mA; 		// ADC2_IN13
	uint16_t temp_trafo_deg; 	// ADC2_IN15

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
    int32_t v_sens_ext_uv;
    int32_t i_iso_ext_uA;
    int32_t i_out_ext_mA;

}ADC_MEAS_DATA;


void adc_init(int32_t* ext_adc_data);
void adc_data_convert(void);
uint16_t adc_encoder_read(void);
void adc_encoder_reset(uint8_t value);

#endif /* INC_ADC_H_ */
