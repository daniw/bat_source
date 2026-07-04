/*
 * display.c
 *
 * See display.h. Static "enter" layout vs. per-tick "update" data is kept
 * deliberately separate: LCD_HANDLE is a bit-banged SPI (no local
 * framebuffer, see lcd.h LCD_LOCAL_FB), so a full-screen redraw every 100ms
 * tick would be slow and would flicker. Static chrome (labels, units,
 * footer) is drawn once on entry; only the numeric fields are redrawn each
 * tick, using fixed-width sprintf formatting so a shorter new value fully
 * overwrites a longer old one.
 */

#include "display.h"
#include "menu.h"
#include "lcd.h"
#include "ugui_fonts.h"
#include "ctrl_main.h"
#include "ctrl_param.h"
#include "adc.h"
#include "bq76905.h"
#include "aux_io_ctrl.h"
#include "ui_ctrl.h"
#include <stdio.h>
#include <string.h>

extern ctrl_main_t ctrl_main_handle;
extern ADC_MEAS_DATA adc_data;
extern BQ76905_handle bms;

static char text[48];

/* ---------------------------------------------------------------------- */
/* Layout                                                                  */
/* ---------------------------------------------------------------------- */

#define STATUS_Y0     0
#define STATUS_H      26
#define BIG_Y         40
#define SECOND_Y      130
#define FOOTER_H      24
#define FOOTER_Y      (LCD_HEIGHT - FOOTER_H)

#define FONT_BIG      FONT_32X53
#define FONT_MED      FONT_16X26
#define FONT_SMALL    FONT_12X16
#define FONT_TINY     FONT_8X12

/* ---------------------------------------------------------------------- */
/* Shared chrome helpers                                                   */
/* ---------------------------------------------------------------------- */

static void draw_battery_icon(int16_t x, int16_t y, uint8_t pct) {
	int16_t w = 28, h = 14;
	UG_COLOR fill = (pct < 20) ? C_RED : (pct < 50 ? C_ORANGE : C_GREEN);
	int16_t fill_w = (int16_t) ((w - 4) * pct / 100);

	UG_FillFrame(x, y, x + w + 3, y + h, C_BLACK);
	UG_DrawFrame(x, y, x + w, y + h, C_WHITE);
	UG_FillFrame(x + w + 1, y + h / 2 - 2, x + w + 3, y + h / 2 + 2, C_WHITE);
	UG_FillFrame(x + 2, y + 2, x + 2 + fill_w, y + h - 2, fill);
}

static void draw_status_bar(const char *title, UG_COLOR accent) {
	UG_FillFrame(0, STATUS_Y0, LCD_WIDTH - 1, STATUS_H - 1, C_BLACK);
	LCD_PutStr(4, 4, (char*) title, FONT_SMALL, C_WHITE, C_BLACK);
	draw_battery_icon(LCD_WIDTH - 40, 6, bms.Accumulator.charge_percentage);
	UG_FillFrame(0, STATUS_H - 3, LCD_WIDTH - 1, STATUS_H - 1, accent);
}

static void update_status_bar(void) {
	draw_battery_icon(LCD_WIDTH - 40, 6, bms.Accumulator.charge_percentage);
}

static void draw_footer(const char *left, const char *right) {
	UG_FillFrame(0, FOOTER_Y, LCD_WIDTH - 1, LCD_HEIGHT - 1, C_BLACK);
	LCD_PutStr(4, FOOTER_Y + 4, (char*) left, FONT_TINY, C_WHITE_63, C_BLACK);
	if (right != 0) {
		int16_t w = (int16_t) (strlen(right) * UG_GetFontWidth(FONT_TINY));
		LCD_PutStr(LCD_WIDTH - 4 - w, FOOTER_Y + 4, (char*) right, FONT_TINY,
				C_WHITE_63, C_BLACK);
	}
}

static void draw_output_border(UG_COLOR accent, uint8_t active) {
	UG_COLOR c = active ? accent : C_DIM_GRAY;
	UG_DrawFrame(0, STATUS_H, LCD_WIDTH - 1, FOOTER_Y - 1, c);
	UG_DrawFrame(1, STATUS_H + 1, LCD_WIDTH - 2, FOOTER_Y - 2, c);
}

/* ---------------------------------------------------------------------- */
/* Home / carousel                                                        */
/* ---------------------------------------------------------------------- */

static uint8_t wrap_index(int idx) {
	int m = idx % MENU_ORDER_LENGTH;
	if (m < 0)
		m += MENU_ORDER_LENGTH;
	return (uint8_t) m;
}

void display_show_idle(uint8_t menu_index) {
	const menu_entry_t *prev = menu_entry_at(wrap_index((int) menu_index - 1));
	const menu_entry_t *cur = menu_entry_at(menu_index);
	const menu_entry_t *next = menu_entry_at(wrap_index((int) menu_index + 1));
	int16_t cy = STATUS_H + (FOOTER_Y - STATUS_H) / 2 - 10;

	UG_FillFrame(0, STATUS_H, LCD_WIDTH - 1, FOOTER_Y - 1, C_BLACK);
	draw_status_bar("BatSource", C_SILVER);
	draw_footer("OK: Select", "ESC: Shutdown");

	prev->draw_icon(LCD_WIDTH / 2 - 100, cy, 16, C_DIM_GRAY);
	next->draw_icon(LCD_WIDTH / 2 + 100, cy, 16, C_DIM_GRAY);
	cur->draw_icon(LCD_WIDTH / 2, cy, 28, cur->accent);

	strncpy(text, menu_name_for_mode(cur->mode), sizeof(text) - 1);
	text[sizeof(text) - 1] = 0;
	{
		int16_t w = (int16_t) (strlen(text) * UG_GetFontWidth(FONT_SMALL));
		UG_FillFrame(0, cy + 40, LCD_WIDTH - 1, cy + 40 + UG_GetFontHeight(FONT_SMALL),
				C_BLACK);
		LCD_PutStr(LCD_WIDTH / 2 - w / 2, cy + 40, text, FONT_SMALL, C_WHITE,
				C_BLACK);
	}
}

/* ---------------------------------------------------------------------- */
/* Active-output template (60V / 10A): big measured value, secondary row,  */
/* setpoint row, hold-to-enable border.                                    */
/* ---------------------------------------------------------------------- */

static void enter_active_output(statemachine_modes_t mode, const char *big_unit,
		const char *secondary_label, const char *secondary_unit,
		const char *setpoint_unit) {
	const menu_entry_t *entry = menu_entry_for_mode(mode);

	draw_status_bar(menu_name_for_mode(mode), entry->accent);
	draw_output_border(entry->accent, 0);
	draw_footer("Hold OUT to enable", "ESC: Back");

	LCD_PutStr(LCD_WIDTH / 2 + 70, BIG_Y + 10, (char*) big_unit, FONT_SMALL,
			C_WHITE, C_BLACK);
	LCD_PutStr(16, SECOND_Y, (char*) secondary_label, FONT_TINY, C_WHITE_63,
			C_BLACK);
	LCD_PutStr(16, SECOND_Y + 18, (char*) secondary_unit, FONT_TINY, C_WHITE_63,
			C_BLACK);
	LCD_PutStr(LCD_WIDTH - 100, SECOND_Y, "SET", FONT_TINY, C_WHITE_63,
			C_BLACK);
	LCD_PutStr(LCD_WIDTH - 100, SECOND_Y + 18, (char*) setpoint_unit,
			FONT_TINY, C_WHITE_63, C_BLACK);
}

static void update_active_output(statemachine_modes_t mode, uint8_t output_active,
		int32_t big_value_x100, int32_t secondary_value_x1000,
		uint32_t setpoint_value_x1000) {
	const menu_entry_t *entry = menu_entry_for_mode(mode);

	draw_output_border(entry->accent, output_active);
	update_status_bar();

	sprintf(text, "%3d.%02d", (int) (big_value_x100 / 100),
			(int) ((big_value_x100 < 0 ? -big_value_x100 : big_value_x100) % 100));
	LCD_PutStr(16, BIG_Y, text, FONT_BIG, C_WHITE, C_BLACK);

	sprintf(text, "%3d.%03d", (int) (secondary_value_x1000 / 1000),
			(int) ((secondary_value_x1000 < 0 ? -secondary_value_x1000 : secondary_value_x1000)
					% 1000));
	LCD_PutStr(16, SECOND_Y + 36, text, FONT_SMALL, C_WHITE, C_BLACK);

	sprintf(text, "%3u.%03u", (unsigned) (setpoint_value_x1000 / 1000),
			(unsigned) (setpoint_value_x1000 % 1000));
	LCD_PutStr(LCD_WIDTH - 100, SECOND_Y + 36, text, FONT_SMALL, C_WHITE,
			C_BLACK);
}

/* ---------------------------------------------------------------------- */
/* Passive-readout template (Voltmeter / Ampmeter): big value only.        */
/* ---------------------------------------------------------------------- */

static void enter_passive_readout(statemachine_modes_t mode, const char *unit) {
	const menu_entry_t *entry = menu_entry_for_mode(mode);

	draw_status_bar(menu_name_for_mode(mode), entry->accent);
	draw_output_border(entry->accent, 1);
	draw_footer("ESC: Back", 0);
	LCD_PutStr(LCD_WIDTH / 2 + 70, BIG_Y + 10, (char*) unit, FONT_SMALL,
			C_WHITE, C_BLACK);
}

static void update_passive_readout(int32_t value_x100) {
	update_status_bar();
	sprintf(text, "%3d.%02d", (int) (value_x100 / 100),
			(int) ((value_x100 < 0 ? -value_x100 : value_x100) % 100));
	LCD_PutStr(16, BIG_Y, text, FONT_BIG, C_WHITE, C_BLACK);
}

/* ---------------------------------------------------------------------- */
/* Resistance 1A / 1mA                                                     */
/* ---------------------------------------------------------------------- */

static void enter_resistance(statemachine_modes_t mode) {
	const menu_entry_t *entry = menu_entry_for_mode(mode);
	const char *excitation =
			(mode == STATEMACHINE_MODE_RESISTANCE_1A) ? "I = 1.000 A" : "I = 1.000 mA";

	draw_status_bar(menu_name_for_mode(mode), entry->accent);
	draw_output_border(entry->accent, 1);
	draw_footer("ESC: Back", 0);
	LCD_PutStr(16, SECOND_Y, (char*) excitation, FONT_TINY, C_WHITE_63,
			C_BLACK);
	LCD_PutStr(LCD_WIDTH / 2 + 100, BIG_Y + 10, "mOhm", FONT_SMALL, C_WHITE,
			C_BLACK);
}

static void update_resistance(void) {
	update_status_bar();
	if (adc_data.r_mOhmx10 == UINT16_MAX) {
		sprintf(text, "OVER ");
	} else {
		sprintf(text, "%4u.%01u", adc_data.r_mOhmx10 / 10,
				adc_data.r_mOhmx10 % 10);
	}
	LCD_PutStr(16, BIG_Y, text, FONT_BIG, C_WHITE, C_BLACK);

	sprintf(text, "V_sense = %d uV     ", (int) adc_data.converted.v_sens_ext_uv);
	LCD_PutStr(16, SECOND_Y + 18, text, FONT_TINY, C_WHITE_63, C_BLACK);
}

/* ---------------------------------------------------------------------- */
/* Isolation test                                                          */
/* ---------------------------------------------------------------------- */

static void enter_isometer(void) {
	const menu_entry_t *entry = menu_entry_for_mode(STATEMACHINE_MODE_ISOMETER);

	draw_status_bar("Isolation Test", entry->accent);
	draw_output_border(entry->accent, 1);
	draw_footer("Encoder: test V", "ESC: Back");
	LCD_PutStr(LCD_WIDTH / 2 + 100, BIG_Y + 10, "Mohm", FONT_SMALL, C_WHITE,
			C_BLACK);
}

static void update_isometer(void) {
	update_status_bar();

	sprintf(text, "Test Voltage: %4u V   ", ctrl_main_handle.voltage_iso_reference_V);
	LCD_PutStr(16, SECOND_Y, text, FONT_TINY, C_WHITE_63, C_BLACK);

	if (adc_data.converted.i_iso_ext_uA > 0) {
		/* R[Mohm] = V / I[uA] (since Mohm = V/uA algebraically); keep one
		 * decimal digit of precision via a x10 fixed-point intermediate. */
		int32_t r_megaohm_x10 = (int32_t) ctrl_main_handle.voltage_iso_reference_V
				* 10 / adc_data.converted.i_iso_ext_uA;
		sprintf(text, "%3d.%01d", (int) (r_megaohm_x10 / 10), (int) (r_megaohm_x10 % 10));
	} else {
		sprintf(text, " OVER");
	}
	LCD_PutStr(16, BIG_Y, text, FONT_BIG, C_WHITE, C_BLACK);

	sprintf(text, "Ileak = %d uA     ", (int) adc_data.converted.i_iso_ext_uA);
	LCD_PutStr(16, SECOND_Y + 18, text, FONT_TINY, C_WHITE_63, C_BLACK);
}

/* ---------------------------------------------------------------------- */
/* Charge                                                                  */
/* ---------------------------------------------------------------------- */

static void enter_charge(void) {
	const menu_entry_t *entry = menu_entry_for_mode(STATEMACHINE_MODE_CHARGE);

	draw_status_bar("Charge", entry->accent);
	draw_output_border(entry->accent, 0);
	draw_footer("OK: Start/Stop", "ESC: Back");
	LCD_PutStr(LCD_WIDTH / 2 + 100, BIG_Y + 10, "V", FONT_SMALL, C_WHITE,
			C_BLACK);
}

static void update_charge(uint8_t output_active) {
	const menu_entry_t *entry = menu_entry_for_mode(STATEMACHINE_MODE_CHARGE);
	uint32_t elapsed_s = bms.Accumulator.passedTime / 4;

	draw_output_border(entry->accent, output_active);
	update_status_bar();

	sprintf(text, "%2u.%03u", bms.VoltageRegisters.StackVoltage / 1000,
			bms.VoltageRegisters.StackVoltage % 1000);
	LCD_PutStr(16, BIG_Y, text, FONT_BIG, C_WHITE, C_BLACK);

	sprintf(text, "I %4d mA   %3u%%   %02u:%02u:%02u    ",
			bms.CurrentRegisters.CC1Current, bms.Accumulator.charge_percentage,
			(unsigned) (elapsed_s / 3600), (unsigned) ((elapsed_s / 60) % 60),
			(unsigned) (elapsed_s % 60));
	LCD_PutStr(16, SECOND_Y + 18, text, FONT_TINY, C_WHITE_63, C_BLACK);
}

/* ---------------------------------------------------------------------- */
/* Public API: mode dispatch                                              */
/* ---------------------------------------------------------------------- */

void display_init(void) {
	UG_FillScreen(C_BLACK);
}

void display_enter_mode(statemachine_modes_t mode) {
	UG_FillFrame(0, STATUS_H, LCD_WIDTH - 1, FOOTER_Y - 1, C_BLACK);

	switch (mode) {
	case STATEMACHINE_MODE_60V_OUT:
		enter_active_output(mode, "V", "IOUT", "A", "V");
		break;
	case STATEMACHINE_MODE_10A_OUT:
		enter_active_output(mode, "A", "VOUT", "V", "A");
		break;
	case STATEMACHINE_MODE_RESISTANCE_1A:
	case STATEMACHINE_MODE_RESISTANCE_1mA:
		enter_resistance(mode);
		break;
	case STATEMACHINE_MODE_ISOMETER:
		enter_isometer();
		break;
	case STATEMACHINE_MODE_VOLTMETER:
		enter_passive_readout(mode, "V");
		break;
	case STATEMACHINE_MODE_AMPMETER:
		enter_passive_readout(mode, "A");
		break;
	case STATEMACHINE_MODE_CHARGE:
		enter_charge();
		break;
	default:
		break;
	}
}

void display_update_mode(statemachine_modes_t mode, uint8_t output_active) {
	switch (mode) {
	case STATEMACHINE_MODE_60V_OUT:
		update_active_output(mode, output_active,
				adc_data.converted.v_term_ext_mv_filt / 10,
				adc_data.converted.i_out_ext_mA,
				ctrl_main_handle.voltage_reference_mV);
		break;
	case STATEMACHINE_MODE_10A_OUT:
		update_active_output(mode, output_active,
				adc_data.converted.i_out_ext_mA / 10,
				adc_data.converted.v_term_ext_mv_filt,
				ctrl_main_handle.current_reference_mA);
		break;
	case STATEMACHINE_MODE_RESISTANCE_1A:
	case STATEMACHINE_MODE_RESISTANCE_1mA:
		update_resistance();
		break;
	case STATEMACHINE_MODE_ISOMETER:
		update_isometer();
		break;
	case STATEMACHINE_MODE_VOLTMETER:
		update_passive_readout(adc_data.converted.v_term_ext_mv_filt / 10);
		break;
	case STATEMACHINE_MODE_AMPMETER:
		update_passive_readout(adc_data.converted.i_out_ext_mA / 10);
		break;
	case STATEMACHINE_MODE_CHARGE:
		update_charge(output_active);
		break;
	default:
		break;
	}
}

/* ---------------------------------------------------------------------- */
/* Settings                                                                */
/* ---------------------------------------------------------------------- */

static const char *const SETTINGS_ITEMS[STATEMACHINE_SETTINGS_MODE_LENGTH] = {
	[STATEMACHINE_SETTINGS_MODE_MENU] = "(list)",
	[STATEMACHINE_SETTINGS_MODE_BMS] = "BMS Diagnostics",
	[STATEMACHINE_SETTINGS_MODE_DISPLAY] = "Display Brightness",
	[STATEMACHINE_SETTINGS_MODE_ABOUT] = "About",
};

void display_show_settings_list(uint8_t submenu_index) {
	UG_FillFrame(0, STATUS_H, LCD_WIDTH - 1, FOOTER_Y - 1, C_BLACK);
	draw_status_bar("Settings", C_SILVER);
	draw_footer("OK: Open", "ESC: Back");

	for (int i = STATEMACHINE_SETTINGS_MODE_BMS; i < STATEMACHINE_SETTINGS_MODE_LENGTH; i++) {
		int16_t y = STATUS_H + 16 + (i - 1) * 24;
		uint8_t selected = (i == submenu_index);

		LCD_PutStr(selected ? 26 : 16, y, (char*) SETTINGS_ITEMS[i], FONT_SMALL,
				C_WHITE, C_BLACK);
		if (selected)
			LCD_PutStr(6, y, ">", FONT_SMALL, C_WHITE, C_BLACK);
	}
}

void display_enter_settings_detail(uint8_t submenu_index) {
	UG_FillFrame(0, STATUS_H, LCD_WIDTH - 1, FOOTER_Y - 1, C_BLACK);
	draw_status_bar(SETTINGS_ITEMS[submenu_index], C_SILVER);
	draw_footer("ESC: Back", 0);

	if (submenu_index == STATEMACHINE_SETTINGS_MODE_ABOUT) {
		sprintf(text, "BatSource Firmware");
		LCD_PutStr(16, STATUS_H + 12, text, FONT_SMALL, C_WHITE, C_BLACK);
		sprintf(text, "Built %s %s", __DATE__, __TIME__);
		LCD_PutStr(16, STATUS_H + 34, text, FONT_TINY, C_WHITE_63, C_BLACK);
		sprintf(text, "HW revision: %u", aux_io_ctrl_readHW_Revision());
		LCD_PutStr(16, STATUS_H + 52, text, FONT_TINY, C_WHITE_63, C_BLACK);
		sprintf(text, "MCU: STM32G474VET6");
		LCD_PutStr(16, STATUS_H + 70, text, FONT_TINY, C_WHITE_63, C_BLACK);
	}
}

static void update_settings_bms(void) {
	sprintf(text, "Safety Alert  AB = 0x%04X",
			(bms.SafetyRegisters.safetyAlertA << 8) | bms.SafetyRegisters.safetyAlertB);
	LCD_PutStr(16, STATUS_H + 8, text, FONT_TINY, C_WHITE, C_BLACK);

	sprintf(text, "Safety Status AB = 0x%04X",
			(bms.SafetyRegisters.safetyStatusA << 8) | bms.SafetyRegisters.safetyStatusB);
	LCD_PutStr(16, STATUS_H + 24, text, FONT_TINY, C_WHITE, C_BLACK);

	sprintf(text, "Cell = %u %u %u %u mV", bms.CellVoltageRegisters.CellVoltages[0],
			bms.CellVoltageRegisters.CellVoltages[1],
			bms.CellVoltageRegisters.CellVoltages[2],
			bms.CellVoltageRegisters.CellVoltages[3]);
	LCD_PutStr(16, STATUS_H + 40, text, FONT_TINY, C_WHITE, C_BLACK);

	sprintf(text, "Current = %d mA    ", bms.CurrentRegisters.CC2Current);
	LCD_PutStr(16, STATUS_H + 56, text, FONT_TINY, C_WHITE, C_BLACK);

	sprintf(text, "Passed Q = %d mAs    ",
			(int) (bms.Accumulator.accumulatedCharge & 0xFFFFFFFF));
	LCD_PutStr(16, STATUS_H + 72, text, FONT_TINY, C_WHITE, C_BLACK);

	sprintf(text, "Passed T = %u s    ", (unsigned) (bms.Accumulator.passedTime / 4));
	LCD_PutStr(16, STATUS_H + 88, text, FONT_TINY, C_WHITE, C_BLACK);
}

static void update_settings_display(void) {
	sprintf(text, "Ambient light: %u lux    ", (unsigned) ui_ctrl_readBrightness());
	LCD_PutStr(16, STATUS_H + 12, text, FONT_SMALL, C_WHITE, C_BLACK);
	sprintf(text, "(auto-dimming not yet implemented)");
	LCD_PutStr(16, STATUS_H + 40, text, FONT_TINY, C_WHITE_63, C_BLACK);
}

void display_update_settings_detail(uint8_t submenu_index) {
	switch (submenu_index) {
	case STATEMACHINE_SETTINGS_MODE_BMS:
		update_settings_bms();
		break;
	case STATEMACHINE_SETTINGS_MODE_DISPLAY:
		update_settings_display();
		break;
	default:
		break;
	}
}
