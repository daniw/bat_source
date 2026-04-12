/*
 * ui_ctrl.c
 *
 *  Created on: 11.04.2026
 *      Author: Andreas
 */

#include "ui_ctrl.h"
#include "opt3004.h"
#include "lp581x.h"

opt3004_handle hamb;
lp581x_handle hled;
lp581x_handle hbacklight;

void ui_ctrl_init(void) {
	/*
	 * LED test setup
	 */
	uint8_t led_pwms[] = { 0xFF, 0xFF, 0xFF, 0xFF };
	uint8_t led_current[] = { 0x00, 0x00, 0x00, 0x00 };
	lp581x_init(&hled, LP5817_ADDRESS);
	lp581x_setPWMDimming(&hled, led_pwms);
	lp581x_setAnalogDimming(&hled, led_current);

	uint8_t backlight_current[] = { 0xFF, 0xFF, 0xFF, 0xFF };
	lp581x_init(&hbacklight, LP5816_ADDRESS);
	lp581x_setPWMDimming(&hbacklight, led_pwms);
	lp581x_setAnalogDimming(&hbacklight, backlight_current);


	/*
	 * Ambient light sensor test setup
	 */
	opt3004_init(&hamb, OPT3004_DEVICE_ADDRESS_GND);
}

void ui_ctrl_Dim(void){
	uint32_t brightness;
	brightness = opt3004_readLux(&hamb);
	// ToDo: Implement dimming here
	uint8_t led_current[] = { 0x10, 0x10, 0x10, 0x10 };
	lp581x_setAnalogDimming(&hled, led_current);

}

