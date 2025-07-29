/*******************************************************************************/
/*
 * File: cli.c
 * Author: Andreas Horat
 */
/*******************************************************************************/
/*
 * This file contains a very simple command line interface.
 */
/*******************************************************************************/

#include "cli.h"

#ifdef CLI_ENABLED

char line_buffer[LINE_BUF_SIZE];
char *arg_locs[MAX_NUM_ARGS];
uint8_t number_of_args;
uint8_t line_buffer_index;

// Uart handling
UART_HandleTypeDef *_huart;
extern BQ76905_handle bms;
extern ADC_MEAS_DATA adc_data;
extern HRTIM_HandleTypeDef hhrtim1;
extern ADS131M04_handle ext_adc;

// Function declarations
void cmd_help(void);
void cmd_saveEEPROM(void);
void cmd_readEEPROM(void);
void cmd_turnOff(void);
void cmd_printGPIO(void);
void cmd_setGPIO(void);
void cmd_setDAC(void);
void cmd_printBMS(void);
void cmd_printADC(void);
void cmd_pulse(void);

// List of functions pointers corresponding to each command
void (*cmd_func[])(void) = {
	cmd_help,
	cmd_saveEEPROM,
	cmd_readEEPROM,
	cmd_turnOff,
	cmd_printGPIO,
	cmd_setGPIO,
	cmd_setDAC,
	cmd_printBMS,
	cmd_printADC,
	cmd_pulse
};

// List of command names
const char *cmd_str[] = {
		"help",
		"saveEEPROM",
		"readEEPROM",
		"turnOff" ,
		"pGPIO",
		"setGPIO",
		"setDAC",
		"pBMS",
		"pADC",
		"pulse"
};

int num_commands = sizeof(cmd_str) / sizeof(char*);
#endif

/**
 * Initialises the CLI
 */
void cli_init(UART_HandleTypeDef *huart1) {
	_huart = huart1;
	setbuf(stdin, NULL); //TO HANDLE INPUT BUFFER WHEN USING SCANF/COUT

#ifdef CLI_ENABLED
	line_buffer_index = 0;
	number_of_args = 0;
	memset(line_buffer, 0, LINE_BUF_SIZE);
	printf("Serial Interface enabled.\r\n");
	printf(">>\r\n");
#endif
}

#ifdef CLI_ENABLED

/**
 * Read the line from the buffer
 *
 * @return Returns true if line is available in buffer.
 */
bool readline(void) {
	while (HAL_UART_Receive(_huart, (uint8_t*) &line_buffer[line_buffer_index],
			1, 1) == 0) {
		HAL_UART_Transmit(_huart, (uint8_t*) &line_buffer[line_buffer_index], 1,
				1);

		if (line_buffer[line_buffer_index] == '\n') {
			line_buffer[line_buffer_index] = 0;
			return line_buffer_index > 0;
		}

		if (line_buffer[line_buffer_index] == '\b')
			line_buffer_index--;

		if (++line_buffer_index == LINE_BUF_SIZE) {
			printf("Input String too long. Deleted.\r\n>");
			line_buffer_index = 0;
			memset(line_buffer, 0, LINE_BUF_SIZE);
		}
	}
	return false;
}

/**
 * Parse the line into the arguments
 *
 * @return Returns the number of available arguments
 */
int parseline(void) {
	number_of_args = 0;

	arg_locs[number_of_args] = strtok(line_buffer, " ");
	while (arg_locs[number_of_args] != NULL) {
		if (++number_of_args >= MAX_NUM_ARGS) {
			printf("Input String contains too much arguments!\r\n>");
			return 0;
		}
		arg_locs[number_of_args] = strtok(NULL, " ");
	}

	return number_of_args;
}

/**
 * Executes the Command according to first argument
 */
void execute(void) {
	for (int i = 0; i < num_commands; i++) {
		if (strcmp(arg_locs[0], cmd_str[i]) == 0) {
			(*cmd_func[i])();
			return;
		}
	}

	printf("Invalid Command.\r\n");
}
#endif

/**
 * CLI Worker function that executes the command if correct
 */
void cli_loop(void) {
#ifdef CLI_ENABLED
	if (!readline())
		return;
	if (!parseline())
		return;
	execute();
	line_buffer_index = 0;
	memset(line_buffer, 0, LINE_BUF_SIZE);
#endif
}

#ifdef CLI_ENABLED
/**
 * Print Help Message
 */
void cmd_help(void) {
	printf("Simple Interface for configuration only.\r\n");
	for (int i = 0; i < num_commands; i++)
		printf("\t%s\n", cmd_str[i]);
}

/*********************** Functions ************************************/

/**
 * Saves the EEPROM including Hardware Part
 */
void cmd_saveEEPROM(void) {
	printf("Not Implemented!\n");
	/*printf("Saving Hardware Area: %d\r\n", config_store_storeHW());
	 printf("Saving User Area: %d\r\n", config_store_store());
	 */
}

/**
 * Reads the value of the EEPROM and prints it to the Serial Port
 */
void cmd_readEEPROM(void) {
	printf("Not Implemented!\n");
	/*printf("Serial Number: %06d\r\n", config_store.hardware_data.serial_number);
	 printf("Hardware Revision: %d\r\n", config_store.hardware_data.hardware_revision);
	 printf("Hardware Data CRC: %d\r\n", config_store.hardware_data_crc);
	 printf("Cone ID: %d\r\n", config_store.user_settings.cone_id);
	 printf("Fallback Color: %d\r\n", config_store.user_settings.fallback_color);
	 printf("Fallback Mode: %d\r\n", config_store.user_settings.fallback_lightmode);
	 printf("Fallback Repetition Time: %d\r\n", config_store.user_settings.fallback_repetition_time);
	 printf("Fallback Phase: %d\r\n", config_store.user_settings.fallback_phase);
	 printf("User Settings CRC: %d\r\n", config_store.user_settings_crc);

	 printf("Hex View\r\n");
	 for (uint8_t i = 0; i < sizeof(config_store_t); i += 16)
	 {
	 printf("0x%02x ", i);
	 for (uint8_t j = 0; j < 16; j += 4)
	 {
	 printf("%02x%02x%02x%02x\t", *(((uint8_t *)&config_store) + i + j),
	 *(((uint8_t *)&config_store) + i + j + 1),
	 *(((uint8_t *)&config_store) + i + j + 2),
	 *(((uint8_t *)&config_store) + i + j + 3));
	 if (j == 12)
	 printf(" \r\n");
	 }
	 }*/
}

void cmd_turnOff(void) {
	printf("Not implemented!\r\n");
	//hw_ctrl_turn_off();
}

void cmd_printGPIO(void) {
	aux_io_ctrl_print_inputs();
	aux_io_ctrl_print_Output();
}
void cmd_setGPIO(void) {
	uint8_t pin, value;
	if (number_of_args != 3)
		return;

	char *end;
	pin = strtol(arg_locs[1], &end, 10);
	value = strtol(arg_locs[2], &end, 10);

	aux_io_ctrl_manual_set_io(pin, value);
}


void cmd_setDAC(void) {
	uint8_t  pin;
	uint16_t value;
	if (number_of_args != 3)
		return;

	char *end;
	pin = strtol(arg_locs[1], &end, 10);
	value = strtol(arg_locs[2], &end, 10);
	if (pin == 0)
		dac_setValueRef(value);
	else
		dac_setValue1ARef(value);
}


void cmd_printBMS(void){
	  printf("SafetyRegisters.safetyAlertA         = 0x%04X\n", bms.SafetyRegisters.safetyAlertA);
	  printf("SafetyRegisters.safetyStatusA        = 0x%04X\n", bms.SafetyRegisters.safetyStatusA);
	  printf("SafetyRegisters.safetyAlertB         = 0x%04X\n", bms.SafetyRegisters.safetyAlertB);
	  printf("SafetyRegisters.safetyStatusB        = 0x%04X\n", bms.SafetyRegisters.safetyStatusB);
	  printf("\n");
	  printf("CellVoltageRegisters.BatteryStatus   = 0x%04X\n", bms.CellVoltageRegisters.BatteryStatus);
	  for (int i = 0; i < 5; ++i) {
	  	printf("CellVoltageRegisters.CellVoltages[%d] = %u mV\n", i, bms.CellVoltageRegisters.CellVoltages[i]);
	  }
	  printf("\n");
	  printf("VoltageRegisters.Reg18Voltage        = %u\n", bms.VoltageRegisters.Reg18Voltage);
	  printf("VoltageRegisters.VssVoltage          = %u\n", bms.VoltageRegisters.VssVoltage);
	  printf("VoltageRegisters.StackVoltage        = %u mV\n", bms.VoltageRegisters.StackVoltage);
	  printf("VoltageRegisters.IntTemp             = %u\n", bms.VoltageRegisters.IntTemp);
	  printf("VoltageRegisters.TSTemp              = %u\n", bms.VoltageRegisters.TSTemp);
	  printf("\n");
	  printf("CurrentRegisters.RawCurrent          = 0x%04X%04X\n", (uint16_t)(bms.CurrentRegisters.RawCurrent>>16),(uint16_t)(bms.CurrentRegisters.RawCurrent));
	  printf("CurrentRegisters.CC2Current          = %d mA\n", bms.CurrentRegisters.CC2Current);
	  printf("CurrentRegisters.CC1Current          = %d mA\n", bms.CurrentRegisters.CC1Current);
	  printf("Passed Charge                        = %lld mAs\n",  bms.Accumulator.accumulatedCharge);
	  printf("Passed Charge                        = %ld mAs\n",  (int32_t)(bms.Accumulator.accumulatedCharge&0xFFFFFFFF));
	  printf("Passed Charge                        = %ld mAs\n",  (int32_t)(bms.Accumulator.accumulatedCharge>>32));
	  printf("Passed Time                          = %ld s\n",    bms.Accumulator.passedTime/4);
	  printf("\n");
	  printf("SystemCtrl.AlarmStatus               = 0x%04X\n", bms.SystemCtrl.AlarmStatus);
	  printf("SystemCtrl.AlarmRawStatus            = 0x%04X\n", bms.SystemCtrl.AlarmRawStatus);
	  printf("SystemCtrl.AlarmEnable               = 0x%04X\n", bms.SystemCtrl.AlarmEnable);
	  printf("SystemCtrl.FETCtrl                   = 0x%04X\n", bms.SystemCtrl.FETCtrl);
	  printf("SystemCtrl.RegoutCtrl                = 0x%04X\n", bms.SystemCtrl.RegoutCtrl);
	  printf("SystemCtrl.DSGFetPWM                 = 0x%04X\n", bms.SystemCtrl.DSGFetPWM);
	  printf("SystemCtrl.CHGFetPWM                 = 0x%04X\n", bms.SystemCtrl.CHGFetPWM);
}


void cmd_printADC() {
	for(int i=0; i<10; i++){
	adc_convert_data();
    printf("ADC Measurements:\n");
    printf("  v_in         (ADC1_IN1)  : %u \t: %u mV\n",adc_data.v_in_raw, adc_data.v_in_mV);
    printf("  v_hv         (ADC1_IN2)  : %u \t: %u mV\n",adc_data.v_hv_raw, adc_data.v_hv_mV);
    printf("  i_bat        (ADC1_IN3)  : %u \t: %u mA\n",adc_data.i_bat_raw, adc_data.i_bat_mA);
    printf("  v_out        (ADC1_IN4)  : %u \t: %u mV\n",adc_data.v_out_raw, adc_data.v_out_mV);
    printf("  v_term       (ADC1_IN13) : %u \t: %u mV\n",adc_data.v_term_raw, adc_data.v_term_mV);
    printf("  temp_int     (ADC1_INT)  : %u \t: %u °C\n",adc_data.temp_int_raw, adc_data.temp_int_deg);
    printf("  i_out        (ADC2_IN5)  : %u \t: %u mA\n",adc_data.i_out_raw, adc_data.i_out_mA);
    printf("  temp_prim    (ADC2_IN8)  : %u \t: %u °C\n",adc_data.temp_prim_raw, adc_data.temp_prim_deg);
    printf("  temp_current (ADC2_IN11) : %u \t: %u °C\n",adc_data.temp_current_raw, adc_data.temp_current_deg);
    printf("  temp_sec     (ADC2_IN12) : %u \t: %u °C\n",adc_data.temp_sec_raw, adc_data.temp_sec_deg);
    printf("  i_iso        (ADC2_IN13) : %u \t: %u mA\n",adc_data.i_iso_raw, adc_data.i_iso_mA);
    printf("  temp_trafo   (ADC2_IN15) : %u \t: %u °C\n",adc_data.temp_trafo_raw, adc_data.temp_trafo_deg);

    printf("  Ext ADC 1 : %i : %i\n", ext_adc.channelData[0], adc_data.v_term_ext_mv);
    printf("  Ext ADC 2 : %i : %i\n", ext_adc.channelData[1], adc_data.v_meas_ext_mv);
    printf("  Ext ADC 3 : %i : %i\n", ext_adc.channelData[2], adc_data.i_iso_ext_mA );
    printf("  Ext ADC 4 : %i : %i\n", ext_adc.channelData[3], adc_data.i_out_ext_mA );
}}


void cmd_pulse(){
	//HRTIM_SimpleOnePulseChannelCfgTypeDef pSimpleOnePulseChannelCfg;
	// pSimpleOnePulseChannelCfg.Pulse = 0x10;
	// pSimpleOnePulseChannelCfg.OutputPolarity = HRTIM_OUTPUTPOLARITY_HIGH;
	// pSimpleOnePulseChannelCfg.OutputIdleLevel = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
	// pSimpleOnePulseChannelCfg.Event = HRTIM_EVENT_NONE;
	// pSimpleOnePulseChannelCfg.EventPolarity = HRTIM_EVENTPOLARITY_HIGH;
	//HAL_HRTIM_SimpleOnePulseChannelConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_OUTPUT_TB1, &pSimpleOnePulseChannelCfg);

	//HAL_HRTIM_SimpleOnePulseStart(&hhrtim1, HRTIM_TIMERINDEX_TIMER_B, HRTIM_OUTPUT_TB1);
	//HAL_HRTIM_WaveformCountStop(&hhrtim1, HRTIM_TIMERID_TIMER_B);
	HRTIM1_COMMON->CR2 |= (0x04 << 8);
	  //HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB1);  // Enable the generation of the waveform signal on the designated output
	  //HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_B);  // Start the counter of the Timer A operating in waveform mode
	//HAL_Delay(1);

}
#endif

/****************************** Redirect Std Out to UART **************************/

//static void uart_write(uint8_t c){
//	HAL_UART_Transmit(&_huart, &c, 1, 10);
//}
//
//static char uart_read(){
//	uint8_t c;
//	HAL_UART_Receive(&_huart, &c, 1, 10);
//	return (char)c;
//
//}

