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
#include "aux_io_ctrl.h"

static ssd1309_handle_t gs_handle;
extern BQ76905_handle bms;
extern statemachine_t statemachine_handle;
extern ctrl_main_t ctrl_main_handle;
extern ADC_MEAS_DATA adc_data;
char string[32];

const char *const DISPLAY_MODE_STRINGS[7] = {
/*		0x0,
 VOLTAGE_MODE_STRING,
 CURRENT_MODE_STRING,
 RESISTANCE_1A_MODE_STRING,
 RESISTANCE_1MA_MODE_STRING,
 ISO_METER_MODE_STRING*/
"", "60V Source", "10A Source", "mOhm 4Wire",
//"Ohm  2Wire",
		"ISO Meter", "Voltmeter", "Settings" };

void display_draw_battery(uint8_t percentage);
void display_draw_crowbar_icon(uint8_t position);
void display_show_settings_bms(void);

uint8_t display_init(void) {
	uint8_t result;

	result = ssd1309_basic_init(&gs_handle, SSD1309_INTERFACE_IIC,
			SSD1309_ADDR_SA0_0);
	//ssd1309_clear(&gs_handle);
	return result;
}

void display_clear(void) {
	memset(gs_handle.gram, 0, 1024);
}

void display_debug_state(void) {
	sprintf(string, "Vbat    : %d.%03dV",
			bms.VoltageRegisters.StackVoltage / 1000,
			bms.VoltageRegisters.StackVoltage % 1000);
	ssd1309_gram_write_string(&gs_handle, 10, 10, string, 20, 1,
			SSD1309_FONT_08);

	sprintf(string, "Ibat    : %dmA", bms.CurrentRegisters.CC1Current);
	ssd1309_gram_write_string(&gs_handle, 10, 22, string, 20, 1,
			SSD1309_FONT_08);

	sprintf(string, "Vout Ref: %*d.%dV", 2,
			(uint16_t) (ctrl_main_handle.poti_reference),
			(uint16_t) (((ctrl_main_handle.poti_reference * 10))) % 10);
	ssd1309_gram_write_string(&gs_handle, 10, 34, string, 20, 1,
			SSD1309_FONT_08);
}

void display_draw(void) {
	ssd1309_gram_update(&gs_handle);
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
	display_draw_crowbar_icon(91);
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

void display_draw_crowbar_icon(uint8_t position) {
	if (AUX_IO_READ_CROWBAR)
		return;
	gs_handle.gram[position + 0][0] = 0x18;
	gs_handle.gram[position + 1][0] = 0x18;
	gs_handle.gram[position + 2][0] = 0x1E;
	gs_handle.gram[position + 3][0] = 0xC7;
	gs_handle.gram[position + 4][0] = 0xE3;
	gs_handle.gram[position + 5][0] = 0x70;
	gs_handle.gram[position + 6][0] = 0x18;
	gs_handle.gram[position + 7][0] = 0x18;
}

void display_draw_measure_layout(void) {
	for (int i = 0; i < 127; i++)
		gs_handle.gram[i][5] = 0x10;

	switch (statemachine_handle.current_mode) {
	case STATEMACHINE_MODE_60V_OUT:
		gs_handle.gram[63][5] = 0xE0;
		gs_handle.gram[63][6] = 0xFF;
		gs_handle.gram[63][7] = 0xFF;
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

	case STATEMACHINE_MODE_10A_OUT:
		gs_handle.gram[63][5] = 0xE0;
		gs_handle.gram[63][6] = 0xFF;
		gs_handle.gram[63][7] = 0xFF;
		ssd1309_gram_write_string(&gs_handle, 0, 49, "VOUT", 4, 1,
				SSD1309_FONT_12);
		ssd1309_gram_write_string(&gs_handle, 53, 47, "V", 1, 1,
				SSD1309_FONT_16);
		gs_handle.gram[39][7] = 0x18; // Dot
		gs_handle.gram[40][7] = 0x18;

		ssd1309_gram_write_string(&gs_handle, 65, 49, "SET", 4, 1,
				SSD1309_FONT_12);
		ssd1309_gram_write_string(&gs_handle, 118, 47, "A", 1, 1,
				SSD1309_FONT_16);
		gs_handle.gram[103][7] = 0x18; // Dot
		gs_handle.gram[104][7] = 0x18;

		ssd1309_gram_write_string(&gs_handle, 92, 15, "A", 1, 1,
				SSD1309_FONT_24);
		gs_handle.gram[59][4] = 0x0E; // Dot
		gs_handle.gram[60][4] = 0x0E;
		gs_handle.gram[61][4] = 0x0E;
		break;

	case STATEMACHINE_MODE_RESISTANCE_1A:

		ssd1309_gram_write_string(&gs_handle, 92, 15, "mOhm", 1, 1,
				SSD1309_FONT_24);
		gs_handle.gram[71][4] = 0x0E; // Dot
		gs_handle.gram[72][4] = 0x0E;
		gs_handle.gram[73][4] = 0x0E;
		break;

	case STATEMACHINE_MODE_ISOMETER:
		ssd1309_gram_write_string(&gs_handle, 92, 15, "V", 1, 1,
				SSD1309_FONT_24);
		gs_handle.gram[47][4] = 0x0E; // Dot
		gs_handle.gram[48][4] = 0x0E;
		gs_handle.gram[49][4] = 0x0E;
		break;

	case STATEMACHINE_MODE_VOLTMETER:
		ssd1309_gram_write_string(&gs_handle, 92, 15, "V", 1, 1,
				SSD1309_FONT_24);
		gs_handle.gram[71][4] = 0x0E; // Dot
		gs_handle.gram[72][4] = 0x0E;
		gs_handle.gram[73][4] = 0x0E;
		break;
	default:
		break;
	}
}

void display_draw_measure_data(void) {
	switch (statemachine_handle.current_mode) {
	case STATEMACHINE_MODE_60V_OUT:

		// I Out
		sprintf(string, "%d", adc_data.i_out_ext_mA / 1000);
		ssd1309_gram_write_string(&gs_handle, 31, 47, string, 1, 1,
				SSD1309_FONT_16);

		sprintf(string, "%d", (adc_data.i_out_ext_mA % 1000) / 100);
		ssd1309_gram_write_string(&gs_handle, 42, 47, string, 1, 1,
				SSD1309_FONT_16);

		// Reference Voltage
		sprintf(string, "%*d", 2, ctrl_main_handle.poti_reference >> 1);
		ssd1309_gram_write_string(&gs_handle, 87, 47, string, 2, 1,
				SSD1309_FONT_16);

		sprintf(string, "%d", (ctrl_main_handle.poti_reference & 0x1) * 5);
		ssd1309_gram_write_string(&gs_handle, 106, 47, string, 2, 1,
				SSD1309_FONT_16);
		// Output Voltage
		sprintf(string, "%*d", 2, adc_data.v_term_ext_mv_filt / 1000);
		ssd1309_gram_write_string(&gs_handle, 35, 15, string, 2, 1,
				SSD1309_FONT_24);
		sprintf(string, "%02d", (adc_data.v_term_ext_mv_filt % 1000) / 10);
		ssd1309_gram_write_string(&gs_handle, 63, 15, string, 2, 1,
				SSD1309_FONT_24);

		break;

	case STATEMACHINE_MODE_10A_OUT:

		// I Out
		sprintf(string, "%d", adc_data.v_term_ext_mv_filt / 1000);
		ssd1309_gram_write_string(&gs_handle, 31, 47, string, 1, 1,
				SSD1309_FONT_16);

		sprintf(string, "%d", (adc_data.v_term_ext_mv_filt % 1000) / 100);
		ssd1309_gram_write_string(&gs_handle, 42, 47, string, 1, 1,
				SSD1309_FONT_16);

		// Reference Voltage
		sprintf(string, "%d", ctrl_main_handle.poti_reference >> 4);
		ssd1309_gram_write_string(&gs_handle, 87, 47, string, 1, 1,
				SSD1309_FONT_16);

		sprintf(string, "%d", (ctrl_main_handle.poti_reference & 0xf) * 6);
		ssd1309_gram_write_string(&gs_handle, 106, 47, string, 2, 1,
				SSD1309_FONT_16);
		// Output Voltage
		sprintf(string, "%*d", 2, adc_data.i_out_ext_mA / 1000);
		ssd1309_gram_write_string(&gs_handle, 35, 15, string, 2, 1,
				SSD1309_FONT_24);
		sprintf(string, "%02d", (adc_data.i_out_ext_mA % 1000) / 10);
		ssd1309_gram_write_string(&gs_handle, 63, 15, string, 2, 1,
				SSD1309_FONT_24);

		break;
	case STATEMACHINE_MODE_RESISTANCE_1A:

		// Output Resistance
		sprintf(string, "%*d", 3, 99);
		ssd1309_gram_write_string(&gs_handle, 35, 15, string, 3, 1,
				SSD1309_FONT_24);
		sprintf(string, "%02d", 2);
		ssd1309_gram_write_string(&gs_handle, 75, 15, string, 2, 1,
				SSD1309_FONT_24);

		break;
	case STATEMACHINE_MODE_ISOMETER:

		// Output Voltage
		sprintf(string, "%*d", 2, 1);
		ssd1309_gram_write_string(&gs_handle, 35, 15, string, 1, 1,
				SSD1309_FONT_24);
		sprintf(string, "%01d", 210);
		ssd1309_gram_write_string(&gs_handle, 51, 15, string, 3, 1,
				SSD1309_FONT_24);

		break;
	case STATEMACHINE_MODE_VOLTMETER:

		// Output Voltage
		sprintf(string, "%*d", 3, 580);
		ssd1309_gram_write_string(&gs_handle, 35, 15, string, 3, 1,
				SSD1309_FONT_24);
		sprintf(string, "%01d", 2);
		ssd1309_gram_write_string(&gs_handle, 75, 15, string, 1, 1,
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
					display_icons[(statemachine_handle.current_menu_index - 1)
							% 6][32 * row + i];
			gs_handle.gram[88 + i][2 + row] =
					display_icons[(statemachine_handle.current_menu_index) % 6][32
							* row + i];
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
		gs_handle.gram[45 + i][1] |= 0x80;
		gs_handle.gram[45 + i][1] |= 0x80;
		gs_handle.gram[45 + i][6] |= 0x01;
		gs_handle.gram[45 + i][6] |= 0x01;
		gs_handle.gram[81 + i][1] |= 0x80;
		gs_handle.gram[81 + i][1] |= 0x80;
		gs_handle.gram[81 + i][6] |= 0x01;
		gs_handle.gram[81 + i][6] |= 0x01;
	}
}

void display_show_settings(uint8_t menu_id) {
	switch (statemachine_handle.settings_mode) {
	case STATEMACHINE_SETTINGS_MODE_MENU:
		ssd1309_gram_write_string(&gs_handle, 8, 10, "BMS", 25, 1,
				SSD1309_FONT_08);
		if (menu_id > 0)
			ssd1309_gram_write_string(&gs_handle, 3, menu_id * 8 + 2, ">", 25,
					1, SSD1309_FONT_08);
		break;
	case STATEMACHINE_SETTINGS_MODE_BMS:
		//display_draw_basic_layout();
		display_show_settings_bms();
		break;
	default:
		break;
	}
}

void display_show_settings_bms(void) {
	sprintf(string, "Safety Alert  AB = 0x%04X",
			(bms.SafetyRegisters.safetyAlertA << 8)
					| bms.SafetyRegisters.safetyAlertB);
	ssd1309_gram_write_string(&gs_handle, 8, 10, string, 25, 1,
			SSD1309_FONT_08);

	sprintf(string, "Safety Status AB = 0x%04X",
			(bms.SafetyRegisters.safetyStatusA << 8)
					| bms.SafetyRegisters.safetyStatusB);
	ssd1309_gram_write_string(&gs_handle, 8, 18, string, 25, 1,
			SSD1309_FONT_08);

	sprintf(string, "Cell = %u %u %u %u mV",
			bms.CellVoltageRegisters.CellVoltages[0],
			bms.CellVoltageRegisters.CellVoltages[1],
			bms.CellVoltageRegisters.CellVoltages[2],
			bms.CellVoltageRegisters.CellVoltages[3]);
	ssd1309_gram_write_string(&gs_handle, 8, 26, string, 29, 1,
			SSD1309_FONT_08);

	sprintf(string, "Current = %d mA", bms.CurrentRegisters.CC2Current);
	ssd1309_gram_write_string(&gs_handle, 8, 34, string, 25, 1,
			SSD1309_FONT_08);
	sprintf(string, "Passed Q = %ld mAs",
			(int32_t) (bms.Accumulator.accumulatedCharge & 0xFFFFFFFF));
	ssd1309_gram_write_string(&gs_handle, 8, 42, string, 25, 1,
			SSD1309_FONT_08);
	sprintf(string, "Passed T = %ld s", bms.Accumulator.passedTime / 4);
	ssd1309_gram_write_string(&gs_handle, 8, 50, string, 25, 1,
			SSD1309_FONT_08);
}

