/*
 * display.c
 *
 *  Created on: Aug 9, 2025
 *      Author: ahorat
 */

#include "display.h"
#include "ctrl_main.h"
#include "statemachine.h"
#include "bq76905.h"
#include "ds1309/driver_ssd1309.h"
#include "ds1309/driver_ssd1309_basic.h"
#include "display_icons.h"
#include "adc.h"

static ssd1309_handle_t gs_handle;
extern BQ76905_handle bms;
extern statemachine_t statemachine_handle;
extern ctrl_main_t ctrl_main_handle;
extern ADC_MEAS_DATA adc_data;

const char *const DISPLAY_MODE_STRINGS[6] = {
/*		0x0,
 VOLTAGE_MODE_STRING,
 CURRENT_MODE_STRING,
 RESISTANCE_1A_MODE_STRING,
 RESISTANCE_1MA_MODE_STRING,
 ISO_METER_MODE_STRING*/
"", "60V Source", "10A Source", "mOhm 4Wire",
//"Ohm  2Wire",
		"ISO Meter", "Voltmeter" };

uint8_t display_init(void) {
	return ssd1309_basic_init(&gs_handle, SSD1309_INTERFACE_IIC,
			SSD1309_ADDR_SA0_0);
}

void display_clear(void) {
	memset(gs_handle.gram, 0, 1024);
}

void display_debug_state(void) {
	char volt_str[20]; // = "Vbat    : ";
	sprintf(volt_str, "Vbat    : %d.%03dV",
			bms.VoltageRegisters.StackVoltage / 1000,
			bms.VoltageRegisters.StackVoltage % 1000);
	ssd1309_gram_write_string(&gs_handle, 10, 10, volt_str, 20, 1,
			SSD1309_FONT_08);

	char curr_str[20];
	sprintf(curr_str, "Ibat    : %dmA", bms.CurrentRegisters.CC1Current);
	ssd1309_gram_write_string(&gs_handle, 10, 22, curr_str, 20, 1,
			SSD1309_FONT_08);

	char ref_str[20];
	sprintf(ref_str, "Vout Ref: %*d.%dV", 2,
			(uint16_t) (ctrl_main_handle.poti_reference),
			(uint16_t) (((ctrl_main_handle.poti_reference * 10))) % 10);
	ssd1309_gram_write_string(&gs_handle, 10, 34, ref_str, 20, 1,
			SSD1309_FONT_08);
}

void display_draw(void) {
	if (ssd1309_gram_update(&gs_handle) != 0) {
		return 1;
	}
}

void display_draw_basic_layout(void) {
	display_clear();
	for (int i = 0; i < 128; i++)
		gs_handle.gram[i][1] |= 0x01;
	display_draw_battery(bms.Accumulator.charge_percentage);
	ssd1309_gram_write_string(&gs_handle, 3, 0,
			DISPLAY_MODE_STRINGS[statemachine_handle.current_mode], 10, 1,
			SSD1309_FONT_08);
	display_draw_battery(50);
}

void display_draw_battery(uint8_t percentage) {
	for (int i = 0; i < 21; i++)
		if (percentage / 5 > (i))
			gs_handle.gram[100 + i][0] = 0x3E;
		else
			gs_handle.gram[100 + i][0] = 0x22;

	gs_handle.gram[100][0] |= 0x3E;
	gs_handle.gram[121][0] |= 0x3E;
	gs_handle.gram[122][0] |= 0x1C;
}

void display_draw_measure_layout(void) {
	for (int i = 0; i < 127; i++)
		gs_handle.gram[i][5] = 0x10;
	gs_handle.gram[63][5] = 0xE0;
	gs_handle.gram[63][6] = 0xFF;
	gs_handle.gram[63][7] = 0xFF;

	switch (statemachine_handle.current_mode) {
	case STATEMACHINE_MODE_60V_OUT:
		ssd1309_gram_write_string(&gs_handle, 0, 49, "IOUT", 4, 1,
				SSD1309_FONT_12);
		ssd1309_gram_write_string(&gs_handle, 53, 47, "A", 1, 1,
				SSD1309_FONT_16);
		gs_handle.gram[39][7] = 0x18; // Dot
		gs_handle.gram[40][7] = 0x18;

		ssd1309_gram_write_string(&gs_handle, 65, 49, "SET", 4, 1,
				SSD1309_FONT_12);
		ssd1309_gram_write_string(&gs_handle, 118, 47, "V", 1, 1,
				SSD1309_FONT_16);
		gs_handle.gram[103][7] = 0x18; // Dot
		gs_handle.gram[104][7] = 0x18;

		ssd1309_gram_write_string(&gs_handle, 92, 15, "V", 1, 1,
				SSD1309_FONT_24);
		gs_handle.gram[59][4] = 0x0E; // Dot
		gs_handle.gram[60][4] = 0x0E;
		gs_handle.gram[61][4] = 0x0E;
		break;
	default:
		break;
	}
}

void display_draw_measure_data(void) {
	switch (statemachine_handle.current_mode) {
	case STATEMACHINE_MODE_60V_OUT:
		char ref_str[2];

		// I Out
		sprintf(ref_str, "%d", adc_data.i_out_ext_mA/1000);
		ssd1309_gram_write_string(&gs_handle, 31, 47, ref_str, 1, 1,
				SSD1309_FONT_16);

		sprintf(ref_str, "%d",  (adc_data.i_out_ext_mA%1000)/100);
		ssd1309_gram_write_string(&gs_handle, 42, 47, ref_str, 1, 1,
				SSD1309_FONT_16);

		// Reference Voltage
		sprintf(ref_str, "%*d", 2,
				ctrl_main_handle.poti_reference>>1);
		ssd1309_gram_write_string(&gs_handle, 87, 47, ref_str, 2, 1,
				SSD1309_FONT_16);

		sprintf(ref_str, "%d",
				(ctrl_main_handle.poti_reference&0x1) *5);
		ssd1309_gram_write_string(&gs_handle, 106, 47, ref_str, 2, 1,
				SSD1309_FONT_16);
		// Output Voltage
		sprintf(ref_str, "%*d", 2, adc_data.v_term_ext_mv/1000);
		ssd1309_gram_write_string(&gs_handle, 35, 15, ref_str, 2, 1,
				SSD1309_FONT_24);
		sprintf(ref_str, "%02d",  (adc_data.v_term_ext_mv%1000)/10);
		ssd1309_gram_write_string(&gs_handle, 63, 15, ref_str, 2, 1,
				SSD1309_FONT_24);

		break;
	default:
		break;
	}
}

void display_show_menu(void) {
	for (int row = 0; row < 4; row++) {
		for (int i = 0; i < 32; i++) {
			gs_handle.gram[8 + i][2 + row] =
					display_icons[(statemachine_handle.current_menu_index + 4)
							% 6][32 * row + i];
			gs_handle.gram[48 + i][2 + row] =
					display_icons[(statemachine_handle.current_menu_index-1) % 6][32
							* row + i];
			gs_handle.gram[88 + i][2 + row] =
					display_icons[(statemachine_handle.current_menu_index)
							% 6][32 * row + i];
		}
	}

	for (int i = 45; i < 83; i++) {
		gs_handle.gram[i][1] |= 0x60;
		gs_handle.gram[i][6] |= 0x06;
	}
	for (int i = 2; i < 6; i++) {

		gs_handle.gram[45][i] |= 0xFF;
		gs_handle.gram[46][i] |= 0xFF;
		gs_handle.gram[81][i] |= 0xFF;
		gs_handle.gram[82][i] |= 0xFF;
	}
	for (int i = 0; i < 2; i++) {
		gs_handle.gram[45+i][1] |= 0x80;
		gs_handle.gram[45+i][1] |= 0x80;
		gs_handle.gram[45+i][6] |= 0x01;
		gs_handle.gram[45+i][6] |= 0x01;
		gs_handle.gram[81+i][1] |= 0x80;
		gs_handle.gram[81+i][1] |= 0x80;
		gs_handle.gram[81+i][6] |= 0x01;
		gs_handle.gram[81+i][6] |= 0x01;
	}
}
