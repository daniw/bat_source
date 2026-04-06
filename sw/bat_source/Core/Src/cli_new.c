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
#include "ctrl_main.h"

#ifdef CLI_ENABLED

char line_buffer[LINE_BUF_SIZE];
char *arg_locs[MAX_NUM_ARGS];
uint8_t number_of_args;
uint8_t line_buffer_index;

#define CLI_HISTORY_SIZE 5
static char cli_history[CLI_HISTORY_SIZE][LINE_BUF_SIZE];
static int  cli_history_count = 0;
static int  cli_history_index = -1;


extern BQ76905_handle bms;
extern ADC_MEAS_DATA adc_data;
extern HRTIM_HandleTypeDef hhrtim1;
extern ADS131M04_handle ext_adc;
extern ctrl_main_t ctrl_main_handle;
extern I2C_HandleTypeDef *i2c_handle;
extern opt3004_handle hamb;
extern PCA9554_handle hpca_hw_rev;

// Uart handling
UART_HandleTypeDef *_huart;

// Forward declarations
static void cli_process_line(void);

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
void cmd_setDuty(void);
void cmd_PWM_en(void);
void cmd_setRef(void);
void cmd_test_i2c(void);
void cmd_readLux(void);

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
	cmd_setDuty,
	cmd_PWM_en,
	cmd_setRef,
	cmd_test_i2c,
	cmd_readLux
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
		"setDuty",
		"enPWM",
		"setRef",
		"test_i2c",
		"readLux"
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
    memset(cli_history, 0, sizeof(cli_history));
    cli_history_count = 0;
    cli_history_index = -1;
	printf("Serial Interface enabled.\r\n");
	printf(">>\r\n");
#endif
}

#ifdef CLI_ENABLED


int _write(int fd, char *ptr, int len) {
	(void) fd;
	HAL_UART_Transmit(_huart, (uint8_t*) ptr, len, 100);
	return len;
}

//size_t _read(int fd, char *ptr, size_t len){
//  (void)fd;
//  size_t i;
//  for(i=0;i<len;i++){
//    *ptr++ = uart_read();
//    uart_write(*ptr++);  //For Terminal Echo
//
//    HAL_UART_Receive(&_huart, ptr, len);
//  }
//  return i;
//}
void myprintf(const char *fmt, ...) {
  static char buffer[64];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  uint16_t len = strlen(buffer);

  // Wait until the USB interface is ready
  HAL_UART_Transmit(_huart,(uint8_t*)buffer, len, 100);
}


/**
 * Read the line from the buffer
 *
 * @return Returns true if line is available in buffer.
 */
bool readline(void) {
	char c;
	bool ret = false;
	// 0=normal, 1=ESC seen, 2='[' seen
    static uint8_t esc_state = 0;
	while (HAL_UART_Receive(_huart, (uint8_t*) &c,1, 1) == 0) {
		ret = true;
        // --- Handle escape sequences (arrow keys) ---
        if (esc_state == 1) {
            if (c == '[') { esc_state = 2; continue; }
            esc_state = 0;
        } else if (esc_state == 2) {
            if (c == 'A') { // ↑ Up arrow
                if (cli_history_count > 0) {
                    if (cli_history_index > 0)
                        cli_history_index--;
                    else
                        cli_history_index = 0;

                    strcpy(line_buffer, cli_history[cli_history_index]);
                    line_buffer_index = strlen(line_buffer);
                    myprintf("\r> %s\x1B[K", line_buffer);
                }
            } else if (c == 'B') { // ↓ Down arrow
                if (cli_history_index < cli_history_count - 1)
                    cli_history_index++;
                else
                    cli_history_index = cli_history_count;

                if (cli_history_index < cli_history_count)
                    strcpy(line_buffer, cli_history[cli_history_index]);
                else
                    line_buffer[0] = '\0';

                line_buffer_index = strlen(line_buffer);
                myprintf("\r> %s\x1B[K", line_buffer);
            }
            esc_state = 0;
            continue;
        }

        // ESC = start of escape sequence
        if (c == 27) { esc_state = 1; continue; }

        // --- Handle ENTER ---
        if (c == '\r' || c == '\n') {
            if (line_buffer_index > 0) {
                line_buffer[line_buffer_index] = 0;
                myprintf("\r\n");
                cli_process_line();
                line_buffer_index = 0;
                memset(line_buffer, 0, sizeof(line_buffer));
                myprintf("> ");
            } else {
            	myprintf("\r\n> ");
            }
        }

        else if (c == '\t') { // Tab key for autocomplete
            char *current = line_buffer;
            size_t len = strlen(current);

            int matches = 0;
            const char *match = NULL;

            for (int i = 0; i < num_commands; i++) {
                if (strncmp(cmd_str[i], current, len) == 0) {
                    if (matches == 0)
                        match = cmd_str[i];
                    matches++;
                }
            }

            if (matches == 1 && match) {
                // Single match -> autocomplete
                strcpy(line_buffer, match);
                line_buffer_index = strlen(line_buffer);
                myprintf("\r> %s\x1B[K", line_buffer); // Update line
            } else if (matches > 1) {
                // Multiple matches -> show list
            	myprintf("\r\n");
                for (int i = 0; i < num_commands; i++) {
                    if (strncmp(cmd_str[i], current, len) == 0)
                    	myprintf("%s\r\n", cmd_str[i]);
          }
                myprintf("> %s", line_buffer); // Reprint current input
            }
        }

        else if ((c == '\b' || c == 127) && line_buffer_index > 0) {
            line_buffer_index--;
            myprintf("\b \b");
        }
        else if (line_buffer_index < LINE_BUF_SIZE - 1 && c >= 32 && c <= 126) {
            line_buffer[line_buffer_index++] = c;
            myprintf("%c", c);
        }
    }
	return ret;
}

/**
 * Parse the line into the arguments
 *
 * @return Returns the number of available arguments
 */
static void cli_process_line(void) {
    number_of_args = 0;
    line_buffer[strcspn(line_buffer, "\r\n")] = 0;

    char full_line_copy[LINE_BUF_SIZE];
    strncpy(full_line_copy, line_buffer, LINE_BUF_SIZE);
    full_line_copy[LINE_BUF_SIZE - 1] = '\0';

    arg_locs[number_of_args] = strtok(line_buffer, " ");
    while (arg_locs[number_of_args] != NULL && number_of_args < MAX_NUM_ARGS - 1) {
        number_of_args++;
        arg_locs[number_of_args] = strtok(NULL, " ");
    }

    if (arg_locs[0] != NULL)
        number_of_args++;

    if (number_of_args == 0)
        return;


    for (int i = 0; i < num_commands; i++) {
        if (strcmp(arg_locs[0], cmd_str[i]) == 0) {
            // --- Store command line in history ---
            if (strlen(full_line_copy) > 0) {
                if (cli_history_count < CLI_HISTORY_SIZE) {
                    strcpy(cli_history[cli_history_count++], full_line_copy);
                } else {
                    for (int j = 1; j < CLI_HISTORY_SIZE; j++)
                        strcpy(cli_history[j - 1], cli_history[j]);
                    strcpy(cli_history[CLI_HISTORY_SIZE - 1], full_line_copy);
                }
            }
            cli_history_index = cli_history_count;
            (*cmd_func[i])();;
            return;
        }
    }

    myprintf("Invalid command.\r\n");
}
#endif

/**
 * CLI Worker function that executes the command if correct
 */
void cli_loop(void) {
#ifdef CLI_ENABLED
	if (!readline())
		return;
	//line_buffer_index = 0;
	//memset(line_buffer, 0, LINE_BUF_SIZE);
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
	printf("Input Pins: \n");
	printf("Button Out: 	%d\n", HAL_GPIO_ReadPin(BUTTON_OUT_GPIO_Port, BUTTON_OUT_Pin));
	printf("Button OK: 		%d\n", HAL_GPIO_ReadPin(BUTTON_OK_GPIO_Port, BUTTON_OK_Pin));
	printf("Button ESC:	 	%d\n", HAL_GPIO_ReadPin(BUTTON_ESC_GPIO_Port, BUTTON_ESC_Pin));

	printf("I2C Alert: 		%d\n", HAL_GPIO_ReadPin(I2C_ALERT_GPIO_Port, I2C_ALERT_Pin));
	printf("ADC_DRDY: 		%d\n", HAL_GPIO_ReadPin(ADC_DRDY_N_GPIO_Port, ADC_DRDY_N_Pin));
	printf("UI Present: 	%d\n", HAL_GPIO_ReadPin(UI_PRESENT_GPIO_Port, UI_PRESENT_Pin));
	printf("OVP Pin Latch:	%d\n", HAL_GPIO_ReadPin(OVP_N_GPIO_Port, OVP_N_Pin));
	printf("OCP Pin Latch:	%d\n", HAL_GPIO_ReadPin(OCP_N_GPIO_Port, OCP_N_Pin));

	printf("HW Revision: 	%d\n", pca9554_read_input(&hpca_hw_rev));
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
	//if (pin == 0)
		//dac_setValueRef(value);
	//else
		//dac_setValue1ARef(value);
}

void cmd_setRef(void){
		uint8_t value;
		if (number_of_args != 2)
			return;

		char *end;
		ctrl_main_handle.poti_reference = strtol(arg_locs[1], &end, 10);

}


void cmd_printBMS(void){
	  BQ76905_readAllValues(&bms);
	  printf("SafetyRegisters.safetyAlertA         = 0x%02X\n", bms.SafetyRegisters.safetyAlertA);
	  printf("SafetyRegisters.safetyStatusA        = 0x%02X\n", bms.SafetyRegisters.safetyStatusA);
	  printf("SafetyRegisters.safetyAlertB         = 0x%02X\n", bms.SafetyRegisters.safetyAlertB);
	  printf("SafetyRegisters.safetyStatusB        = 0x%02X\n", bms.SafetyRegisters.safetyStatusB);
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
	//for(int i=0; i<10; i++){
	adc_convert_data();
	printf("Triggered ADC Measurements: \n");
    printf(" 	 v_in    (ADC3_IN3 ) : %u \t: %d mV\n", adc_data.raw.v_in   , adc_data.converted.v_in  );
    printf(" 	 v_out   (ADC4_IN2 ) : %u \t: %d mV\n", adc_data.raw.v_out  , adc_data.converted.v_out );
    printf(" 	 v_term  (ADC2_IN2 ) : %u \t: %d mV\n", adc_data.raw.v_term , adc_data.converted.v_term);
    printf(" 	 v_hv    (ADC4_IN5 ) : %u \t: %d mV\n", adc_data.raw.v_hv   , adc_data.converted.v_hv  );
    printf(" 	 i_bat   (ADC5_IN1 ) : %u \t: %d mA\n", adc_data.raw.i_bat  , adc_data.converted.i_bat );
    printf(" 	 i_out   (ADC1_IN1)  : %u \t: %d mA\n", adc_data.raw.i_out  , adc_data.converted.i_out );
    printf(" 	 i_iso   (ADC5_IN2)  : %u \t: %d mA\n", adc_data.raw.i_iso  , adc_data.converted.i_iso );

    printf("ADC Measurements:\n");
    printf(" 		v_3v3          (ADC5_IN6 ) : %u \t: %u mV\n", adc_data.raw.v_3v3        , adc_data.converted.v_3v3       );
    printf(" 		temp_sec       (ADC5_IN7 ) : %u \t: %u °C\n", adc_data.raw.temp_sec     , adc_data.converted.temp_sec    );
    printf(" 		v_3v3a         (ADC5_IN8 ) : %u \t: %u mV\n", adc_data.raw.v_3v3a       , adc_data.converted.v_3v3a      );
    printf(" 		temp_trafo     (ADC5_IN9 ) : %u \t: %u °C\n", adc_data.raw.temp_trafo   , adc_data.converted.temp_trafo  );
    printf(" 		temp_current   (ADC5_IN12) : %u \t: %u °C\n", adc_data.raw.temp_current , adc_data.converted.temp_current);
    printf(" 		temp_prim      (ADC5_IN13) : %u \t: %u °C\n", adc_data.raw.temp_prim    , adc_data.converted.temp_prim   );
    printf(" 		v_15v          (ADC5_IN14) : %u \t: %u mV\n", adc_data.raw.v_15v        , adc_data.converted.v_15v       );
    printf(" 		v_vcc          (ADC5_IN15) : %u \t: %u mV\n", adc_data.raw.v_vcc        , adc_data.converted.v_vcc       );
    printf(" 		v_5v           (ADC5_IN16) : %u \t: %u mV\n", adc_data.raw.v_5v         , adc_data.converted.v_5v        );
    printf(" 		int_temp       (ADC5     ) : %u \t: %i °C\n", adc_data.raw.int_temp     , adc_data.converted.int_temp    );
    printf(" 		v_bat          (ADC5     ) : %u \t: %u mV\n", adc_data.raw.v_bat        , adc_data.converted.v_bat       );
    printf(" 		v_ref_int      (ADC5     ) : %u \t: %u mV\n", adc_data.raw.v_ref_int    , adc_data.converted.v_ref_int   );

    //printf("  Ext V_Term : %i : %i mV\n", ext_adc.channelData[0], adc_data.v_term_ext_mv);
    //printf("  Ext V_Sns  : %i : %i uV\n", ext_adc.channelData[1], adc_data.v_sens_ext_uv);
    //printf("  Ext I_Iso  : %i : %i uA\n", ext_adc.channelData[2], adc_data.i_iso_ext_uA );
    //printf("  Ext I_Out  : %i : %i mA\n", ext_adc.channelData[3], adc_data.i_out_ext_mA );
//}
}



void cmd_setDuty(void){
	uint8_t  channel;
	float value;
	if (number_of_args != 3)
		return;

	char *end;
	channel = strtol(arg_locs[1], &end, 10);
	value = strtof(arg_locs[2], &end);
	switch(channel){
	case 0: hrtim_set_duty(HRTIM_CHANNEL_PRIM, value);
	return;
	case 1: hrtim_set_duty(HRTIM_CHANNEL_SEK, value);
	return;
	case 2: hrtim_set_duty(HRTIM_CHANNEL_HV, value);
	return;
	}

}


void cmd_PWM_en(void){
	uint8_t  channel;
	uint16_t value;
	if (number_of_args != 3)
		return;

	char *end;
	channel = strtol(arg_locs[1], &end, 10);
	value = strtol(arg_locs[2], &end, 10);
	if(value == 1)
		hrtim_enable(channel);
	else
		hrtim_disable(channel);
}

void cmd_test_i2c(void){
	uint64_t fw_version;
	uint8_t buffer[2] = {0x68,2};
	/*printf("Test write 0x%x, 0x%x\r\n", i2c_WriteBlocking(bms.address, buffer, 2), i2c_handle->ErrorCode);
	buffer[0] = 0x12;
	printf("Test write 0x%x, 0x%x\r\n", i2c_WriteBlocking(bms.address, buffer, 1), i2c_handle->ErrorCode);
	printf("Test write 0x%x, 0x%x\r\n", i2c_WriteBlocking(bms.address, buffer, 1), i2c_handle->ErrorCode);
		buffer[0] = 0;
	buffer[1] = 0;
	i2c_ReadBlocking(bms.address, buffer, 2);
	printf("Test Read 0x%x\r\n", *((uint16_t*)&buffer));*/

	printf("Read HW Version: 0x%04X\r\n", BQ76905_GetHWVersion(&bms));
	BQ76905_GetFWVersion(&bms, (uint8_t*)&fw_version);
	printf("Read FW Version: 0x%08x%08x\r\n", (uint32_t) fw_version&0xFFFFFFFF, (uint32_t) fw_version>>32);
	printf("Read Device Number: 0x%04X\r\n", BQ76905_GetDeviceNumber(&bms));

}

void cmd_readLux(void){

	printf("Read Optical Sensor: %d cLux\r\n", opt3004_readLux(&hamb));
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

