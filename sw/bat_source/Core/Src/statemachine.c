/*
 * statemachine.c
 *
 *  Created on: Aug 9, 2025
 *      Author: ahorat
 */

#include "statemachine.h"
#include "aux_io_ctrl.h"
#include "display.h"
#include "timer.h"
#include "ctrl_main.h"
#include "stm32f3xx_hal.h"
#include "adc.h"

statemachine_t statemachine_handle;
uint16_t ok_button_pressed;
uint16_t esc_button_pressed;
extern ctrl_main_t ctrl_main_handle;

void statemachine_switchfromIdle(void);
void statemachine_switchtoIdle(void);

void statemachine_init(void) {
	statemachine_handle.output_on = 0;
	statemachine_handle.current_mode = STATEMACHINE_IDLE;
	ok_button_pressed = 0;
	esc_button_pressed = 0;
	timer_add(STATEMACHINE_STEP_PERIOD, TIMER_TYPE_TICK, EVENT_SM_STEP, 0);
}

void statemachine_step(void) {
	uint8_t temp;
	if (AUX_IO_READ_OK)
		ok_button_pressed += 1;
	else if (AUX_IO_READ_ESC)
		esc_button_pressed += 1;
	else {
		ok_button_pressed = 0;
		esc_button_pressed = 0;
	}

	switch (statemachine_handle.current_mode) {
	case STATEMACHINE_IDLE:
		temp = ( adc_encoder_read()/2) % 6 + 1;
		if(temp != statemachine_handle.current_menu_index)
		{
			statemachine_handle.current_menu_index =temp;;
			display_show_menu();

		}
		if (ok_button_pressed == 1) {
			statemachine_switchfromIdle();
		}

		if (esc_button_pressed == 1)
			aux_io_ctrl_manual_set_io(AUX_IO_ON_REQ, 0);
		break;
	case STATEMACHINE_MODE_60V_OUT:
		//pwm_setduty_prim((ctrl_main_handle.poti_reference)*0.8/60.0);
		display_draw_measure_data();
		if(AUX_IO_READ_OUT)
		{
			aux_io_ctrl_manual_set_io(AUX_IO_EXPANDER, AUX_IO_UI_LED_OUTN_ORANGE| AUX_IO_UI_LED_OUTP_ORANGE);
		}else
			aux_io_ctrl_manual_set_io(AUX_IO_EXPANDER, AUX_IO_UI_LED_OUTN_GREEN| AUX_IO_UI_LED_OUTP_GREEN);


		if (esc_button_pressed == 1) {
			statemachine_switchtoIdle();
		}
		break;

	case STATEMACHINE_MODE_SETTINGS:
		break;

	default:
		break;
	}

	display_draw();
}

void statemachine_switchfromIdle(void) {
	switch (statemachine_handle.current_menu_index) {
	case STATEMACHINE_MODE_60V_OUT:
		aux_io_ctrl_manual_set_io(AUX_IO_EXPANDER, AUX_IO_UI_LED_OUTN_GREEN| AUX_IO_UI_LED_OUTP_GREEN);

		statemachine_handle.current_mode =
				statemachine_handle.current_menu_index;

		display_draw_basic_layout();
		display_draw_measure_layout();
		display_draw_measure_data();
		break;
	case 0:
		//statemachine_handle.current_mode =
		//		statemachine_handle.current_menu_index;
		break;
	default:
		break;
	}

	adc_encoder_reset(0);
}

void statemachine_switchtoIdle(void){
	adc_encoder_reset(62);
	statemachine_handle.current_mode = STATEMACHINE_IDLE;
	aux_io_ctrl_manual_set_io(AUX_IO_EXPANDER, 0x00);
	display_draw_basic_layout();

}
