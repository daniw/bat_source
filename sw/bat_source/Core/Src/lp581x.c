/*
 * LP581X.h
 *
 *  Created on: October 10, 2025
 *      Author: ahorat
 */

#include "lp581x.h"

const uint8_t default_config[] = {
LP581X_REG_CHIP_EN,
LP581X_FLAG_CHIP_EN,
LP581X_FLAG_DEV_CONFIG0_25MA5,
LP581X_FLAG_DEV_CONFIG1_OUT_EN_ALL,
LP581X_FLAG_DEV_CONFIG2_FADE_EN_ALL | LP581X_LED_FADE_TIME_MIN,
LP581X_FLAG_DEV_CONFIG3_OUT_EXP_EN_ALL };

/**
 * Set direction for all pins at once
 * @param hlp581x lp581x handle to the device. 
 * @param address i2c device address
 */
uint8_t lp581x_init(lp581x_handle *hlp581x, uint8_t address) {
	hlp581x->address = address;
	i2c_WriteBlocking(hlp581x->address, default_config, sizeof(default_config));
	return lp581x_updateConfig(hlp581x);
}

/**
 * Makes the modified config values take effect.
 * @param hlp581x lp581x handle to the device. 
 */
uint8_t lp581x_updateConfig(lp581x_handle *hlp581x) {
	uint8_t buf[] = { LP581X_REG_CMD_UPDATE, LP581X_VAL_CMD_UPDATE };
	return i2c_WriteBlocking(hlp581x->address, buf, 2);
}

/**
 * Reset the LP581X Device
 * @param hlp581x lp581x handle to the device. 
 */
uint8_t lp581x_reset(lp581x_handle *hlp581x) {
	uint8_t buf[] = { LP581X_REG_CMD_RESET, LP581X_VAL_CMD_RESET };
	return i2c_WriteBlocking(hlp581x->address, buf, 2);
}

/**
 * Switches the LP581X device to shutdown mode.
 * @param hlp581x lp581x handle to the device. 
 */
uint8_t lp581x_shutdown(lp581x_handle *hlp581x) {
	uint8_t buf[] = { LP581X_REG_CMD_SHUTDOWN, LP581X_VAL_CMD_SHUTDOWN };
	return i2c_WriteBlocking(hlp581x->address, buf, 2);
}

/**
 * Enables specific channel
 * @param hlp581x lp581x handle to the device.
 * @param channel channel number
 */
uint8_t lp581x_enableChannel(lp581x_handle *hlp581x, uint8_t channel) {
	hlp581x->config1 |= (1 << channel);
	uint8_t buf[] = { LP581X_REG_DEV_CONFIG1, hlp581x->config1 };
	i2c_WriteBlocking(hlp581x->address, buf, 2);
	return lp581x_updateConfig(hlp581x);
}

/**
 * Disables specific channel
 * @param hlp581x lp581x handle to the device.
 * @param channel channel number
 */
uint8_t lp581xdisableChannel(lp581x_handle *hlp581x, uint8_t channel) {
	hlp581x->config1 &= ~(1 << channel);
	uint8_t buf[] = { LP581X_REG_DEV_CONFIG1, hlp581x->config1 };
	i2c_WriteBlocking(hlp581x->address, buf, 2);
	return lp581x_updateConfig(hlp581x);
}

/**
 * Set the analog Current value
 * @param hlp581x lp581x handle to the device. 
 * @param values pointer to 8bit 4 element array containing all Analog Values
 */
uint8_t lp581x_setAnalogDimming(lp581x_handle *hlp581x, uint8_t* value) {
	uint8_t buf[] = { LP581X_REG_OUT0_DC, value[0], value[1], value[2], value[3] };
	return i2c_WriteBlocking(hlp581x->address, buf, 5);
}


/**
 * Set the analog Current value
 * @param hlp581x lp581x handle to the device.
 * @param channel channel number
 * @param value 8-bit analog dimming value
 */
uint8_t lp581x_setAnalogDimmingChannel(lp581x_handle *hlp581x, uint8_t channel, uint8_t value) {
	uint8_t buf[] = { LP581X_REG_OUT0_DC + channel, value };
	return i2c_WriteBlocking(hlp581x->address, buf, 2);
}

/**
 * Set the digital PWM value
 * @param hlp581x lp581x handle to the device. 
 * @param channel channel number 
 * @param value 8-bit pwm dimming value
 */
uint8_t lp581x_setPWMDimmingChannel(lp581x_handle *hlp581x, uint8_t channel, uint8_t value) {
	uint8_t buf[] = { LP581X_REG_OUT0_PWM + channel, value };
	return i2c_WriteBlocking(hlp581x->address, buf, 2);
}

/**
 * Set the digital PWM value
 * @param hlp581x lp581x handle to the device.
 * @param values pointer to 8bit 4 element array containing all PWM Values
 */
uint8_t lp581x_setPWMDimming(lp581x_handle *hlp581x, uint8_t* value) {
	uint8_t buf[] = { LP581X_REG_OUT0_PWM, value[0], value[1], value[2], value[3] };
	return i2c_WriteBlocking(hlp581x->address, buf, 5);
}

/**
 * Configure fading time and enable for all channels at once. 
 * @param hlp581x lp581x handle to the device. 
 * @param fade_time_ms fade time in ms 
 * @param mask fading enable channel mask 
 */
uint8_t lp581x_configureFading(lp581x_handle *hlp581x, uint16_t fade_time_ms, uint8_t mask) {
	uint8_t buf[] = { LP581X_REG_DEV_CONFIG2, mask | LP581X_LED_FADE_TIME_MS_TO_REG(fade_time_ms) };
	i2c_WriteBlocking(hlp581x->address, buf, 2);
	return lp581x_updateConfig(hlp581x);
}

/**
 * Configure eye-friendly exponential dimming curve for all channels. 
 * @param hlp581x lp581x handle to the device. 
 * @param mask exponential dimming channel mask 
 */
uint8_t lp581x_configureExpDimming(lp581x_handle *hlp581x, uint8_t mask) {
	uint8_t buf[] = { LP581X_REG_DEV_CONFIG3, mask };
	i2c_WriteBlocking(hlp581x->address, buf, 2);
	return lp581x_updateConfig(hlp581x);
}

