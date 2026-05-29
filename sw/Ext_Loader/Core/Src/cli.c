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

#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "stdbool.h"
#include <stdarg.h>
#include "gpio.h"
#include "w25n01gv.h"
#include "Loader_Src.h"

#ifdef CLI_ENABLED

char line_buffer[LINE_BUF_SIZE];
char *arg_locs[MAX_NUM_ARGS];
uint8_t number_of_args;
uint8_t line_buffer_index;

#if CLI_HAS_HISTORY
// ToDo Check why Index of Command gets wrong some times.
char history[history_SIZE][LINE_BUF_SIZE];
int  history_count = 0;
int  history_index = -1;
#endif //CLI_HAS_HISTORY

extern w25n01gv_handle flash;

// Uart handling
UART_HandleTypeDef *cli_huart;

// Forward declarations
static void cli_process_line(void);

// Function declarations
void cmd_help(void);

// List of functions pointers corresponding to each command
void (*cmd_func[])(void) = {
	cmd_help
};

// List of command names
const char *cmd_str[] = {
		"help"
};

// List of command names including arguments
const char *cmd_arg_str[] = {
		"help"
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
#if CLI_HAS_HISTORY
    memset(history, 0, sizeof(history));
    history_count = 0;
    history_index = -1;
#endif //CLI_HAS_HISTORY
	printf(">\n");
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
#if CLI_HAS_HISTORY
        } else if (esc_state == 2) {
            if (c == 'A') { // ↑ Up arrow
                if (history_count > 0) {
                    if (history_index > 0)
                        history_index--;
                    else
                        history_index = 0;

                    strcpy(line_buffer, history[history_index]);
                    line_buffer_index = strlen(line_buffer);
                    cli_printf("\n> %s\x1B[K", line_buffer);
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
                cli_printf("\n> %s\x1B[K", line_buffer);
            }
            esc_state = 0;
            continue;
        }

        // ESC = start of escape sequence
        if (c == 27) {
        	esc_state = 1;
        	continue;
#endif //CLI_HAS_HISTORY
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
            	cli_printf("\n> ");
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
                    	cli_printf("%s\n", cmd_str[i]);
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
#if CLI_HAS_HISTORY
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
#endif //CLI_HAS_HISTORY
        	cmd_index = i;
            (*cmd_func[i])();;
            return;
        }
    }

    printf("Inv cmd\n");
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
	if (number_of_args == 17) {
		Init();
		uint8_t buffer[1];
		Write(0, 0, buffer);
		SectorErase(0, 0);
		Read(0, 0, buffer);
		//Verify(0, 0, 0);
		MassErase();
		//Checksum(); // Todo: Parameters not specified in documentation
	}
}

/*********************** Functions ************************************/


#endif

