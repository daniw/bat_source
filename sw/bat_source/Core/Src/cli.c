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

UART_HandleTypeDef *_huart;

// Function declarations
void cmd_help(void);
void cmd_saveEEPROM(void);
void cmd_readEEPROM(void);
void cmd_turnOff(void);
void cmd_printGPIO(void);
void cmd_setGPIO(void);
void cmd_setDAC(void);

// List of functions pointers corresponding to each command
void (*cmd_func[])(void) = {
	cmd_help,
	cmd_saveEEPROM,
	cmd_readEEPROM,
	cmd_turnOff,
	cmd_printGPIO,
	cmd_setGPIO,
	cmd_setDAC
};

// List of command names
const char *cmd_str[] = {
		"help",
		"saveEEPROM",
		"readEEPROM",
		"turnOff" ,
		"printGPIO",
		"setGPIO",
		"setDAC"};

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
	uint8_t pin, value;
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

