/*
 * calibration.c
 *
 * See calibration.h.
 */

#include "calibration.h"
#include "adc.h"
#include "config_store.h"
#include "statemachine.h"
#include "stm32g4xx_hal.h"

extern ADC_MEAS_DATA adc_data;

#define CALIBRATION_SAMPLE_COUNT 8
#define CALIBRATION_SAMPLE_DELAY_MS 1

static const char *const CAL_CHANNEL_NAMES[CAL_CH_COUNT] = {
		[CAL_CH_V_TERM] = "V_TERM",
		[CAL_CH_V_SENS] = "V_SENS",
		[CAL_CH_V_OUT] = "V_OUT",
		[CAL_CH_V_HV] = "V_HV",
		[CAL_CH_I_OUT] = "I_OUT",
		[CAL_CH_I_ISO] = "I_ISO", };

static const char *const CAL_CHANNEL_UNITS[CAL_CH_COUNT] = {
		[CAL_CH_V_TERM] = "mV",
		[CAL_CH_V_SENS] = "uV",
		[CAL_CH_V_OUT] = "mV",
		[CAL_CH_V_HV] = "mV",
		[CAL_CH_I_OUT] = "mA",
		[CAL_CH_I_ISO] = "uA", };

const char* calibration_channel_name(calibration_channel_t ch) {
	if (ch >= CAL_CH_COUNT)
		return "?";
	return CAL_CHANNEL_NAMES[ch];
}

const char* calibration_channel_unit(calibration_channel_t ch) {
	if (ch >= CAL_CH_COUNT)
		return "?";
	return CAL_CHANNEL_UNITS[ch];
}

/* V_OUT/V_HV (and I_ISO's injected channel) are only sampled by ADC4/5
 * while adc_configure_mode() has configured them for a specific
 * statemachine mode (see adc.c) - the Settings/Calibration screen
 * itself never does, since it never drives the output. Point the
 * relevant ADC(s) at the right channel before sampling; harmless to
 * call repeatedly (aux IO / output-enable are untouched by this call). */
static void calibration_ensure_adc_mode(calibration_channel_t ch) {
	switch (ch) {
	case CAL_CH_V_OUT:
		adc_configure_mode(STATEMACHINE_MODE_60V_OUT);
		break;
	case CAL_CH_V_HV:
	case CAL_CH_I_ISO:
		adc_configure_mode(STATEMACHINE_MODE_ISOMETER);
		break;
	default:
		adc_configure_mode(STATEMACHINE_MODE_SETTINGS);
		break;
	}
}

static int32_t calibration_sample_raw_once(calibration_channel_t ch) {
	switch (ch) {
	case CAL_CH_V_TERM:
		return (int32_t) adc_data.raw.v_term;
	case CAL_CH_V_SENS:
		return adc_data.ext_adc_data[2];
	case CAL_CH_V_OUT:
		return (int32_t) adc_data.raw.v_out;
	case CAL_CH_V_HV:
		return (int32_t) adc_data.raw.v_hv;
	case CAL_CH_I_OUT:
		return (int32_t) adc_data.raw.i_out;
	case CAL_CH_I_ISO:
		return (int32_t) adc_data.raw.i_iso;
	default:
		return 0;
	}
}

int32_t calibration_read_raw(calibration_channel_t ch) {
	int64_t sum = 0;

	calibration_ensure_adc_mode(ch);
	for (uint8_t i = 0; i < CALIBRATION_SAMPLE_COUNT; i++) {
		sum += calibration_sample_raw_once(ch);
		HAL_Delay(CALIBRATION_SAMPLE_DELAY_MS);
	}
	return (int32_t) (sum / CALIBRATION_SAMPLE_COUNT);
}

int32_t calibration_read_converted(calibration_channel_t ch) {
	calibration_ensure_adc_mode(ch);
	switch (ch) {
	case CAL_CH_V_TERM:
		return adc_data.converted.v_term;
	case CAL_CH_V_SENS:
		return adc_data.converted.v_sens_ext_uv;
	case CAL_CH_V_OUT:
		return (int32_t) adc_data.converted.v_out;
	case CAL_CH_V_HV:
		return (int32_t) adc_data.converted.v_hv;
	case CAL_CH_I_OUT:
		return adc_data.converted.i_out;
	case CAL_CH_I_ISO:
		return adc_data.converted.i_iso;
	default:
		return 0;
	}
}

static int32_t calibration_get_offset(calibration_channel_t ch) {
	switch (ch) {
	case CAL_CH_V_TERM:
		return adc_data.v_term_offset;
	case CAL_CH_V_SENS:
		return adc_data.v_sens_offset;
	case CAL_CH_V_OUT:
		return adc_data.v_out_offset;
	case CAL_CH_V_HV:
		return adc_data.v_hv_offset;
	case CAL_CH_I_OUT:
		return adc_data.i_out_offset;
	case CAL_CH_I_ISO:
		return adc_data.i_iso_offset;
	default:
		return 0;
	}
}

void calibration_zero(calibration_channel_t ch) {
	int32_t raw = calibration_read_raw(ch);

	switch (ch) {
	case CAL_CH_V_TERM:
		adc_data.v_term_offset = (uint16_t) raw;
		config_store.calibration.v_term_offset_mv = (uint16_t) raw;
		break;
	case CAL_CH_V_SENS:
		adc_data.v_sens_offset = raw;
		config_store.calibration.v_sens_offset = raw;
		break;
	case CAL_CH_V_OUT:
		adc_data.v_out_offset = (uint16_t) raw;
		config_store.calibration.v_out_offset = (uint16_t) raw;
		break;
	case CAL_CH_V_HV:
		adc_data.v_hv_offset = (uint16_t) raw;
		config_store.calibration.v_hv_offset = (uint16_t) raw;
		break;
	case CAL_CH_I_OUT:
		adc_data.i_out_offset = (uint16_t) raw;
		config_store.calibration.i_out_offset_ma = (uint16_t) raw;
		break;
	case CAL_CH_I_ISO:
		adc_data.i_iso_offset = (uint16_t) raw;
		config_store.calibration.i_iso_offset_ua = (uint16_t) raw;
		break;
	default:
		return;
	}
	config_store_store();
}

void calibration_set_gain(calibration_channel_t ch, float reference_value) {
	int32_t raw = calibration_read_raw(ch);
	int32_t offset = calibration_get_offset(ch);
	float gain = 0.0f;

	if (raw != offset)
		gain = reference_value / (float) (raw - offset);

	switch (ch) {
	case CAL_CH_V_TERM:
		adc_data.v_term_gain = gain;
		config_store.calibration.v_term_gain = gain;
		break;
	case CAL_CH_V_SENS:
		adc_data.v_sens_gain = gain;
		config_store.calibration.v_sens_gain = gain;
		break;
	case CAL_CH_V_OUT:
		adc_data.v_out_gain = gain;
		config_store.calibration.v_out_gain = gain;
		break;
	case CAL_CH_V_HV:
		adc_data.v_hv_gain = gain;
		config_store.calibration.v_hv_gain = gain;
		break;
	case CAL_CH_I_OUT:
		adc_data.i_out_gain = gain;
		config_store.calibration.i_out_gain = gain;
		break;
	case CAL_CH_I_ISO:
		adc_data.i_iso_gain = gain;
		config_store.calibration.i_iso_gain = gain;
		break;
	default:
		return;
	}
	config_store_store();
}
