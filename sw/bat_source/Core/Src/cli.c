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

#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "stdbool.h"
#include "event.h"
#include "error.h"
#include "aux_io_ctrl.h"
#include "bq76905.h"
#include "adc.h"
#include "ADS131M04.h"
#include "bq76905.h"
#include <stdarg.h>
#include "tim.h"
#include "hrtim.h"
#include "ui_ctrl.h"
#include "gpio.h"
#include "w25n01gv.h"

#ifdef CLI_ENABLED

#define CLI_CHECK_ARG_CNT(nof_args)                 if (number_of_args != nof_args)                             {printf("Insufficient number of arguments\nUsage: %s\n", cmd_arg_str[cmd_index]); return;}
#define CLI_CHECK_ARG_CNT_MIN(nof_args)             if (number_of_args < nof_args)                              {printf("Insufficient number of arguments\nUsage: %s\n", cmd_arg_str[cmd_index]); return;}
#define CLI_CHECK_ARG_CNT_MAX(nof_args)             if (number_of_args > nof_args)                              {printf("Insufficient number of arguments\nUsage: %s\n", cmd_arg_str[cmd_index]); return;}
#define CLI_CHECK_ARG_CNT_RANGE(min_args, max_args) if (number_of_args < min_args || number_of_args > max_args) {printf("Insufficient number of arguments\nUsage: %s\n", cmd_arg_str[cmd_index]); return;}

char line_buffer[LINE_BUF_SIZE];
char *arg_locs[MAX_NUM_ARGS];
uint8_t number_of_args;
uint8_t line_buffer_index;

// ToDo Check why Index of Command gets wrong sime times.
#define history_SIZE 5
char history[history_SIZE][LINE_BUF_SIZE];
int  history_count = 0;
int  history_index = -1;


extern BQ76905_handle bms;
extern ADC_MEAS_DATA adc_data;
extern HRTIM_HandleTypeDef hhrtim1;
extern ADS131M04_handle ext_adc;
extern ctrl_main_t ctrl_main_handle;
extern I2C_HandleTypeDef *i2c_handle;
extern w25n01gv_handle flash;

// Uart handling
UART_HandleTypeDef *cli_huart;

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
void cmd_setLED(void);
void cmd_setBacklight(void);
void cmd_readLux(void);
void cmd_printFlashID(void);

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
	cmd_setLED,
	cmd_setBacklight,
	cmd_readLux,
	cmd_printFlashID
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
		"setLED",
		"setBacklight",
		"readLux",
		"pFlashID"
};

// List of command names including arguments
const char *cmd_arg_str[] = {
		"help {command}",
		"saveEEPROM",
		"readEEPROM",
		"turnOff" ,
		"pGPIO",
		"setGPIO [pin] [value]",
		"setDAC [pin] [value]",
		"pBMS",
		"pADC {loops}",
		"setDuty [channel] [duty]",
		"enPWM [channel] [0 disable | 1 enable]",
		"setRef [value]",
		"test_i2c",
		"setLED [current]",
		"setBacklight [current]",
		"readLux",
		"pFlashID"
};

int cmd_index;

int num_commands = sizeof(cmd_str) / sizeof(char*);
#endif

/**
 * Initialises the CLI
 */
void cli_init(UART_HandleTypeDef *huart1) {
	cli_huart = huart1;
	setbuf(stdin, NULL); //TO HANDLE INPUT BUFFER WHEN USING SCANF/COUT

#ifdef CLI_ENABLED
	line_buffer_index = 0;
	number_of_args = 0;
	memset(line_buffer, 0, LINE_BUF_SIZE);
    memset(history, 0, sizeof(history));
    history_count = 0;
    history_index = -1;
	printf("Serial Interface enabled.\r\n");
	printf(">>\r\n");
#endif
}

#ifdef CLI_ENABLED

int _write(int fd, char *ptr, int len) {
	(void) fd;
	HAL_UART_Transmit(cli_huart, (uint8_t*) ptr, len, 100);
	return len;
}

void cli_printf(const char *fmt, ...) {
  static char buffer[64];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  uint16_t len = strlen(buffer);
  HAL_UART_Transmit(cli_huart,(uint8_t*)buffer, len, 100);
}


/**
 * Read the line from the buffer
 *
 * @return Returns true if line is available in buffer.
 */
bool cli_readline(void) {
	char c;
	bool ret = false;
	// 0=normal, 1=ESC seen, 2='[' seen
    static uint8_t esc_state = 0;
	while (HAL_UART_Receive(cli_huart, (uint8_t*) &c,1, 1) == 0) {
		ret = true;
        // --- Handle escape sequences (arrow keys) ---
        if (esc_state == 1) {
            if (c == '[') {
            	esc_state = 2;
            	continue;
            }
            esc_state = 0;
        } else if (esc_state == 2) {
            if (c == 'A') { // ↑ Up arrow
                if (history_count > 0) {
                    if (history_index > 0)
                        history_index--;
                    else
                        history_index = 0;

                    strcpy(line_buffer, history[history_index]);
                    line_buffer_index = strlen(line_buffer);
                    cli_printf("\r> %s\x1B[K", line_buffer);
                }
            } else if (c == 'B') { // ↓ Down arrow
                if (history_index < history_count - 1)
                    history_index++;
                else
                    history_index = history_count;

                if (history_index < history_count)
                    strcpy(line_buffer, history[history_index]);
                else
                    line_buffer[0] = '\0';

                line_buffer_index = strlen(line_buffer);
                cli_printf("\r> %s\x1B[K", line_buffer);
            }
            esc_state = 0;
            continue;
        }

        // ESC = start of escape sequence
        if (c == 27) {
        	esc_state = 1;
        	continue;
        }

        // --- Handle ENTER ---
        if (c == '\r' || c == '\n') {
            if (line_buffer_index > 0) {
                line_buffer[line_buffer_index] = 0;
                cli_printf("\r\n");
                cli_process_line();
                line_buffer_index = 0;
                memset(line_buffer, 0, sizeof(line_buffer));
                cli_printf("> ");
            } else {
            	cli_printf("\r\n> ");
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
                cli_printf("\r> %s\x1B[K", line_buffer); // Update line
            } else if (matches > 1) {
                // Multiple matches -> show list
            	cli_printf("\r\n");
                for (int i = 0; i < num_commands; i++) {
                    if (strncmp(cmd_str[i], current, len) == 0)
                    	cli_printf("%s\r\n", cmd_str[i]);
          }
                cli_printf("> %s", line_buffer); // Reprint current input
            }
        }

        else if ((c == '\b' || c == 127) && line_buffer_index > 0) {
            line_buffer_index--;
            cli_printf("\b \b");
        }
        else if (line_buffer_index < LINE_BUF_SIZE - 1 && c >= 32 && c <= 126) {
            line_buffer[line_buffer_index++] = c;
            cli_printf("%c", c);
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
        arg_locs[++number_of_args] = strtok(NULL, " ");
    }



    if (number_of_args == 0)
        return;

    if (arg_locs[0] != NULL)
        number_of_args--;

    for (int i = 0; i < num_commands; i++) {
        if (strcmp(arg_locs[0], cmd_str[i]) == 0) {
        	if ((history_count <= 0) || (strcmp(history[history_count-1], full_line_copy) != 0)){
            // --- Store command line in history ---
                if (strlen(full_line_copy) > 0) {
                    if (history_count < history_SIZE) {
                        strcpy(history[history_count++], full_line_copy);
                    } else {
                        for (int j = 1; j < history_SIZE; j++)
                            strcpy(history[j - 1], history[j]);
                        strcpy(history[history_SIZE - 1], full_line_copy);
                    }
                }
                history_index = history_count;
            }
        	cmd_index = i;
            (*cmd_func[i])();;
            return;
        }
    }

    printf("Invalid command.\r\n");
}
#endif

/**
 * CLI Worker function that executes the command if correct
 */
void cli_loop(void) {
#ifdef CLI_ENABLED
	if (!cli_readline())
		return;
#endif
}

#ifdef CLI_ENABLED
/**
 * Print Help Message
 */
void cmd_help(void) {
	CLI_CHECK_ARG_CNT_MAX(1);

	if (number_of_args == 1) {
		for (int i = 0; i < num_commands; i++) {
			if (strcmp(arg_locs[1], cmd_str[i]) == 0) {
				printf("Usage: \r\n");
				printf("  %s\n", cmd_arg_str[i]);
				return;
			}
		}
		printf("Invalid command\r\n");
	}
	printf("Simple Interface for configuration only.\r\n");
	for (int i = 0; i < num_commands; i++){
		printf("\t%s\n", cmd_arg_str[i]);
	}
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
	printf("Turning off ...\r\n");
	//Todo: disable any controller or converter that might be running before shutdown to prevent damage!
	HAL_Delay(10);
	gpio_turnOff();
	for (uint16_t i = 0; i < 100; i++){
		if (i%10 == 0){
			printf("Waiting ...\n");
		}
		HAL_Delay(1);
	}
	gpio_turnOn();
	printf("Turning off unsuccessful, reenabling on/off controller\r\n");
}

void cmd_printGPIO(void) {
	printf("Input Pins: \n");
	printf("Button Out:         %d\n", HAL_GPIO_ReadPin(BUTTON_OUT_GPIO_Port, BUTTON_OUT_Pin));
	printf("Button OK:          %d\n", HAL_GPIO_ReadPin(BUTTON_OK_GPIO_Port, BUTTON_OK_Pin));
	printf("Button ESC:         %d\n", HAL_GPIO_ReadPin(BUTTON_ESC_GPIO_Port, BUTTON_ESC_Pin));

	printf("I2C Alert:          %d\n", HAL_GPIO_ReadPin(I2C_ALERT_GPIO_Port, I2C_ALERT_Pin));
	printf("ADC_DRDY:           %d\n", HAL_GPIO_ReadPin(ADC_DRDY_N_GPIO_Port, ADC_DRDY_N_Pin));
	printf("UI Present:         %d\n", HAL_GPIO_ReadPin(UI_PRESENT_GPIO_Port, UI_PRESENT_Pin));
	printf("OVP Pin Latch:      %d\n", HAL_GPIO_ReadPin(OVP_N_GPIO_Port, OVP_N_Pin));
	printf("OCP Pin Latch:      %d\n", HAL_GPIO_ReadPin(OCP_N_GPIO_Port, OCP_N_Pin));

	printf("HW Revision: 	    %d\n", aux_io_ctrl_readHW_Revision());

	printf("Encoder State:      %d\n", tim_encoder_read());
	printf("\n");

	printf("Output Pins: \n");
	printf("DISPLAY_WR:         %d\n", HAL_GPIO_ReadPin(DISPLAY_WR_GPIO_Port, DISPLAY_WR_Pin));
	printf("CONV_CTRL_EN:       %d\n", HAL_GPIO_ReadPin(CONV_CTRL_EN_GPIO_Port, CONV_CTRL_EN_Pin));
	printf("ON_REQ:             %d\n", HAL_GPIO_ReadPin(ON_REQ_GPIO_Port, ON_REQ_Pin));
	printf("OUT_SEL_ISO:        %d\n", HAL_GPIO_ReadPin(OUT_SEL_ISO_GPIO_Port, OUT_SEL_ISO_Pin));
	printf("BMS_CTRL_WAKEUP:    %d\n", HAL_GPIO_ReadPin(BMS_CTRL_WAKEUP_GPIO_Port, BMS_CTRL_WAKEUP_Pin));
	printf("SHUNT_EN:           %d\n", HAL_GPIO_ReadPin(SHUNT_EN_GPIO_Port, SHUNT_EN_Pin));
	printf("OUT_SEL_HV:         %d\n", HAL_GPIO_ReadPin(OUT_SEL_HV_GPIO_Port, OUT_SEL_HV_Pin));
	printf("OVP_RESET:          %d\n", HAL_GPIO_ReadPin(OVP_RESET_GPIO_Port, OVP_RESET_Pin));
	printf("DISCHARGE:          %d\n", HAL_GPIO_ReadPin(DISCHARGE_N_GPIO_Port, DISCHARGE_N_Pin));
	printf("HV_CTRL_EN:         %d\n", HAL_GPIO_ReadPin(HV_CTRL_EN_GPIO_Port, HV_CTRL_EN_Pin));
	printf("SHUNT_ISO_EN:       %d\n", HAL_GPIO_ReadPin(SHUNT_ISO_EN_GPIO_Port, SHUNT_ISO_EN_Pin));
	printf("ADC_SYNC_RESET:     %d\n", HAL_GPIO_ReadPin(ADC_SYNC_RESET_N_GPIO_Port, ADC_SYNC_RESET_N_Pin));
}

void cmd_setGPIO(void) {
	uint8_t pin, value;
	CLI_CHECK_ARG_CNT(2);

	char *end;
	pin = strtol(arg_locs[1], &end, 10);
	value = strtol(arg_locs[2], &end, 10);

	aux_io_ctrl_manual_set_io(pin, value);
}

void cmd_setDAC(void) {
	uint8_t  pin;
	uint16_t value;
	CLI_CHECK_ARG_CNT(2);

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
		CLI_CHECK_ARG_CNT(1);

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
	  printf("VoltageRegisters.Reg18Voltage        = %u\n",    bms.VoltageRegisters.Reg18Voltage);
	  printf("VoltageRegisters.VssVoltage          = %u\n",    bms.VoltageRegisters.VssVoltage);
	  printf("VoltageRegisters.StackVoltage        = %u mV\n", bms.VoltageRegisters.StackVoltage);
	  printf("VoltageRegisters.IntTemp             = %u\n",    bms.VoltageRegisters.IntTemp);
	  printf("VoltageRegisters.TSTemp              = %u\n",    bms.VoltageRegisters.TSTemp);
	  printf("\n");
	  printf("CurrentRegisters.RawCurrent          = 0x%04X%04X\n",  (uint16_t)(bms.CurrentRegisters.RawCurrent>>16),(uint16_t)(bms.CurrentRegisters.RawCurrent));
	  printf("CurrentRegisters.CC2Current          = %d mA\n",       bms.CurrentRegisters.CC2Current);
	  printf("CurrentRegisters.CC1Current          = %d mA\n",       bms.CurrentRegisters.CC1Current);
	  printf("Passed Charge                        = %ld mAs\n",     bms.Accumulator.accumulatedCharge);
	  printf("Passed Charge (low bytes)            = %ld mAs\n",     (int32_t)(bms.Accumulator.accumulatedCharge&0xFFFFFFFF));
	  printf("Passed Charge (high bytes)           = %ld mAs\n",     (int32_t)(bms.Accumulator.accumulatedCharge>>32));
	  printf("Passed Time                          = %ld s\n",       bms.Accumulator.passedTime/4);
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
	uint16_t loopcount = 1;
	CLI_CHECK_ARG_CNT_MAX(1);
	if (number_of_args == 1) {
		char *end;
		loopcount = strtol(arg_locs[1], &end, 10);
	}
	for(int i=0; i<loopcount; i++)	{
		adc_convert_data();
		printf("Triggered ADC Measurements: \n");
		printf(" 	 v_in    (ADC3_IN3 ) : %u \t: %d mV\n",  adc_data.raw.v_in   , adc_data.converted.v_in  );
		printf(" 	 v_out   (ADC4_IN2 ) : %u \t: %ld mV\n", adc_data.raw.v_out  , adc_data.converted.v_out );
		printf(" 	 v_term  (ADC2_IN2 ) : %u \t: %ld mV\n", adc_data.raw.v_term , adc_data.converted.v_term);
		printf(" 	 v_hv    (ADC4_IN5 ) : %u \t: %ld mV\n", adc_data.raw.v_hv   , adc_data.converted.v_hv  );
		printf(" 	 i_bat   (ADC5_IN1 ) : %u \t: %d mA\n",  adc_data.raw.i_bat  , adc_data.converted.i_bat );
		printf(" 	 i_out   (ADC1_IN1)  : %u \t: %d mA\n",  adc_data.raw.i_out  , adc_data.converted.i_out );
		printf(" 	 i_iso   (ADC5_IN2)  : %u \t: %d mA\n",  adc_data.raw.i_iso  , adc_data.converted.i_iso );

		printf("ADC Measurements:\n");
		printf(" 		v_3v3          (ADC5_IN6 ) : %u \t: %u mV\n", adc_data.raw.v_3v3        , adc_data.converted.v_3v3       );
		printf(" 		temp_sec       (ADC5_IN7 ) : %u \t: %i °C\n", adc_data.raw.temp_sec     , adc_data.converted.temp_sec    );
		printf(" 		v_3v3a         (ADC5_IN8 ) : %u \t: %u mV\n", adc_data.raw.v_3v3a       , adc_data.converted.v_3v3a      );
		printf(" 		temp_inductor  (ADC5_IN9 ) : %u \t: %i °C\n", adc_data.raw.temp_trafo   , adc_data.converted.temp_trafo  );
		printf(" 		temp_current   (ADC5_IN12) : %u \t: %i °C\n", adc_data.raw.temp_current , adc_data.converted.temp_current);
		printf(" 		temp_prim      (ADC5_IN13) : %u \t: %i °C\n", adc_data.raw.temp_prim    , adc_data.converted.temp_prim   );
		printf(" 		v_15v          (ADC5_IN14) : %u \t: %u mV\n", adc_data.raw.v_15v        , adc_data.converted.v_15v       );
		printf(" 		v_vcc          (ADC5_IN15) : %u \t: %u mV\n", adc_data.raw.v_vcc        , adc_data.converted.v_vcc       );
		printf(" 		v_5v           (ADC5_IN16) : %u \t: %u mV\n", adc_data.raw.v_5v         , adc_data.converted.v_5v        );
		printf(" 		int_temp       (ADC5     ) : %u \t: %i °C\n", adc_data.raw.int_temp     , adc_data.converted.int_temp    );
		printf(" 		v_bat          (ADC5     ) : %u \t: %u mV\n", adc_data.raw.v_bat        , adc_data.converted.v_bat       );
		printf(" 		v_ref_int      (ADC5     ) : %u \t: %u mV\n", adc_data.raw.v_ref_int    , adc_data.converted.v_ref_int   );

		printf("  Ext V_Term : %li : %li mV\n", ext_adc.channelData[0], adc_data.converted.v_term_ext_mv);
		printf("  Ext I_Out  : %li : %li mA\n", ext_adc.channelData[1], adc_data.converted.i_out_ext_mA );
		printf("  Ext V_Sns  : %li : %li uV\n", ext_adc.channelData[2], adc_data.converted.v_sens_ext_uv);
		printf("  Ext I_Iso  : %li : %li uA\n", ext_adc.channelData[3], adc_data.converted.i_iso_ext_uA );
	}
}

void cmd_setDuty(void){
	uint8_t  channel;
	float value;
	CLI_CHECK_ARG_CNT(2);

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
	CLI_CHECK_ARG_CNT(2);

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
	printf("Read BMS HW Version: 0x%04X\r\n", BQ76905_GetHWVersion(&bms));
	BQ76905_GetFWVersion(&bms, (uint8_t*)&fw_version);
	printf("Read BMS FW Version: 0x%08lx%08lx\r\n", (uint32_t) fw_version&0xFFFFFFFF, (uint32_t) (fw_version>>32));
	printf("Read BMS Device Number: 0x%04X\r\n", BQ76905_GetDeviceNumber(&bms));
}

void cmd_setLED(void){
    uint8_t led_current;
	uint8_t led_currents[3];
    CLI_CHECK_ARG_CNT_RANGE(1, 3);

	if (number_of_args == 3) {
		char *end;
		for (uint8_t i = 0; i < 3; i++){
			led_currents[i] = strtol(arg_locs[i+1], &end, 10)/2;
			if (led_currents[i] > 150) {
				led_currents[i] = 150;
				printf("LED current limited to 30mA\r\n");
			}
		}
	}
	else{
		char *end;
		led_current = strtol(arg_locs[1], &end, 10)/2;
		if (led_current > 150) {
			led_current = 150;
			printf("LED current limited to 20mA\r\n");
		}
		led_currents[0] = led_current;
		led_currents[1] = led_current/2;
		led_currents[2] = led_current/2;
	}
	lp581x_setAnalogDimming(&hled, led_currents);
	//lp581x_setAnalogDimmingChannel(&hled, 0, led_currents[0]);
	//lp581x_setAnalogDimmingChannel(&hled, 1, led_currents[1]);
	//lp581x_setAnalogDimmingChannel(&hled, 2, led_currents[2]);
}

void cmd_setBacklight(void){
    uint16_t led_current;
	uint8_t led_currents[4];
	CLI_CHECK_ARG_CNT(1);

	char *end;
	led_current = strtol(arg_locs[1], &end, 10);
	if (led_current > 400) {
		led_current = 400;
		printf("LED current limited to 40mA\r\n");
	}
	for (uint8_t i = 0; i < 4; i++){
		led_currents[i] = (uint8_t) (led_current/2);
	}
	lp581x_setAnalogDimming(&hbacklight, led_currents);
	//lp581x_setAnalogDimmingChannel(&hbacklight, 0, (uint8_t) led_current/2);
	//lp581x_setAnalogDimmingChannel(&hbacklight, 1, (uint8_t) led_current/2);
	//lp581x_setAnalogDimmingChannel(&hbacklight, 2, (uint8_t) led_current/2);
	//lp581x_setAnalogDimmingChannel(&hbacklight, 3, (uint8_t) led_current/2);
}

void cmd_readLux(void){

	printf("Read Optical Sensor: %lu cLux\r\n", ui_ctrl_readBrightness());
}

void cmd_printFlashID(void){
	uint8_t id[4];
	HAL_StatusTypeDef status = HAL_OK;
	status = w25n01gv_read_id(&flash, id);
	if (status == HAL_OK){
		printf("Flash Manufacturer ID:        0x%02X\r\n", id[0]);
		printf("Flash Device ID:              0x%02X%02X\r\n", id[1], id[2]);
	}
	else{
		printf("Error: Unable to read flash JEDEC ID. \r\n");
		return;
	}
	// Status register read test
	printf("\r\n= Flash status register read test: \r\n");
	w25n01gv_prot_reg_t reg_prot = {0};
	if (status == HAL_OK){
		status = w25n01gv_read_reg_prot(&flash, &reg_prot);
	}
	if (status == HAL_OK){
		printf("Flash protection register:    0x%02X\r\n", reg_prot.reg);
	}
	else{
		printf("Error: Unable to read flash protection register. \r\n");
		return;
	}
	w25n01gv_conf_reg_t reg_conf = {0};
	if (status == HAL_OK){
		status = w25n01gv_read_reg_conf(&flash, &reg_conf);
	}
	if (status == HAL_OK){
		printf("Flash configuration register: 0x%02X\r\n", reg_conf.reg);
	}
	else{
		printf("Error: Unable to read flash configuration register. \r\n");
		return;
	}
	w25n01gv_status_reg_t reg_status = {0};
	if (status == HAL_OK){
		status = w25n01gv_read_reg_status(&flash, &reg_status);
	}
	if (status == HAL_OK){
		printf("Flash status register:        0x%02X\r\n", reg_status.reg);
	}
	else{
		printf("Error: Unable to read flash status register. \r\n");
		return;
	}
	// Status register write test
	printf("\r\n= Flash status register write test: \r\n");
	reg_conf.fields.buf_mode=0;
	if (status == HAL_OK){
		status = w25n01gv_write_reg_conf(&flash, &reg_conf);
	}
	if (status == HAL_OK){
		status = w25n01gv_read_reg_conf(&flash, &reg_conf);
	}
	if (status == HAL_OK){
		printf("Flash configuration register: 0x%02X (after writing)\r\n", reg_conf.reg);
		reg_conf.fields.buf_mode=1;
		status = w25n01gv_write_reg_conf(&flash, &reg_conf);
	}
	else{
		printf("Error: Unable to read flash configuration register. \r\n");
		return;
	}
	// Write enable and disable test
	printf("\r\n= Flash write enable / disable test: \r\n");
	if (status == HAL_OK){
		status = w25n01gv_read_reg_status(&flash, &reg_status);
	}
	if (status == HAL_OK){
		printf("Status register before write enable: 0x%02X\r\n", reg_status.reg);
		status = w25n01gv_write_enable(&flash);
	}
	if (status == HAL_OK){
		status = w25n01gv_read_reg_status(&flash, &reg_status);
	}
	if (status == HAL_OK){
		printf("Status register after write enable:  0x%02X\r\n", reg_status.reg);
		status = w25n01gv_write_disable(&flash);
	}
	if (status == HAL_OK){
		status = w25n01gv_read_reg_status(&flash, &reg_status);
	}
	if (status == HAL_OK){
		printf("Status register after write disable: 0x%02X\r\n", reg_status.reg);
	}
	// Read block test
	printf("\r\n= Read block test: \r\n");
	uint16_t test_read_block_nof_bytes = 16;
	for (uint16_t page_addr = 0; page_addr < 16; page_addr++) {
		if (status == HAL_OK){
			status = w25n01gv_page_read(&flash, page_addr);
		}
		do {
			status = w25n01gv_read_reg_status(&flash, &reg_status);
		} while(status == HAL_OK && reg_status.fields.busy != 0);
		uint8_t data[test_read_block_nof_bytes];
		if (status == HAL_OK){
			status = w25n01gv_read(&flash, data, 0x00, test_read_block_nof_bytes);
		}
		if (status == HAL_OK){
			printf("Page: %4d Data:", page_addr);
			for (uint16_t i = 0; i < test_read_block_nof_bytes; i++) {
				if ((i % 16 == 0) && (i > 0)) {
					printf("\r\n                ");
				}
				printf(" 0x%02X", data[i]);
			}
			if (data[0] != 0xFF) {
				printf(" <= Bad Block present ==");
			}
			printf("\r\n");
		}
	}
	// Read OTP block test
	printf("\r\n= Read OTP block test: \r\n");
	test_read_block_nof_bytes = 256;
	if (status == HAL_OK){
		reg_conf.fields.otp_enter = 1;
		status = w25n01gv_write_reg_conf(&flash, &reg_conf);
	}
	for (uint16_t page_addr = 0; page_addr < 3; page_addr++) {
		if (status == HAL_OK){
			status = w25n01gv_page_read(&flash, page_addr);
		}
		do {
			status = w25n01gv_read_reg_status(&flash, &reg_status);
		} while(status == HAL_OK && reg_status.fields.busy != 0);
		uint8_t data[test_read_block_nof_bytes];
		if (status == HAL_OK){
			status = w25n01gv_read(&flash, data, 0x00, test_read_block_nof_bytes);
		}
		if (status == HAL_OK){
			printf("Page: %4d Data:", page_addr);
			for (uint16_t i = 0; i < test_read_block_nof_bytes; i++) {
				if ((i % 16 == 0) && (i > 0)) {
					printf("\r\n                ");
				}
				printf(" 0x%02X", data[i]);
			}
			printf("\r\n");
		}
	}
	if (status == HAL_OK){
		reg_conf.fields.otp_enter = 0;
		status = w25n01gv_write_reg_conf(&flash, &reg_conf);
	}
	// Read bad block look up table test
	printf("\r\n= Read bad block look up table test: \r\n");
	if (status == HAL_OK){
		status = w25n01gv_read_badblock(&flash);
	}
	if (status == HAL_OK){
		printf("Bad block LUT: \r\n");
		printf("ID | Logic  | Physical\r\n");
		printf("---+--------+---------\r\n");
		for (uint8_t i = 0; i < W25N01GV_BADBLOCK_LUT_SIZE; i++) {
			printf("%2d | 0x%04X | 0x%04X\r\n", i, flash.badblock_lut[i].addr.logic_block_addr, flash.badblock_lut[i].addr.phys_block_addr);
		}
	}
	// Bad block test
	printf("\r\n= Bad block test: \r\n");
	uint16_t bad_block_present = 0;
	uint16_t bb_loop_count = 0;
	uint8_t data_page;
	uint8_t data_spare;
	test_read_block_nof_bytes = 1;
	for (uint32_t page_addr = 0; page_addr < 0xffff; page_addr+=0x40) {
		bb_loop_count++;
		if (status == HAL_OK){
			status = w25n01gv_page_read(&flash, (uint16_t) page_addr);
		}
		do {
			status = w25n01gv_read_reg_status(&flash, &reg_status);
		} while(status == HAL_OK && reg_status.fields.busy != 0);
		if (status == HAL_OK){
			status = w25n01gv_read(&flash, &data_page, 0x00, 1);
		}
		if (status == HAL_OK){
			status = w25n01gv_read(&flash, &data_spare, 0x800, 1);
		}
		if (status == HAL_OK){
			if ((data_page != 0xFF) || (data_spare != 0xFF)) {
				bad_block_present = 1;
				printf("Bad block on page %4d, Page: 0x%02X  Spare: 0x%02X\r\n", (uint16_t) (page_addr / 0x40), data_page, data_spare);
			}
		}
	}
	printf("%d blocks scanned\r\n", bb_loop_count);
	if (bad_block_present == 0) {
		printf("No bad blocks\r\n");
	}
}

void cmd_change_state(void){
	uint8_t  newstate;
	CLI_CHECK_ARG_CNT(1);

	char *end;
	newstate = strtol(arg_locs[1], &end, 10);
	//statemachine_step();
}


#endif

