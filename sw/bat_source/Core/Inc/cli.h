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
#ifndef CLI_H
#define CLI_H

#include "main.h"


#define LINE_BUF_SIZE 128
#define MAX_NUM_ARGS 8

#define CLI_ENABLED

#define CLI_FLASH_TEST_STATUS_REG_READ	1 // Status register read test
#define CLI_FLASH_TEST_STATUS_REG_WRITE	1 // Status register write test
#define CLI_FLASH_TEST_BUF_EN_DIS		1 // Buffer mode enable / disable test
#define CLI_FLASH_TEST_WRITE_EN_DIS		1 // Write enable and disable test
#define CLI_FLASH_TEST_READ_BLOCK		1 // Read block test
#define CLI_FLASH_TEST_READ_OTP_BLOCK	1 // Read OTP block test
#define CLI_FLASH_TEST_BAD_BLOCK_LUT	1 // Read bad block look up table test
#define CLI_FLASH_TEST_BAD_BLOCK		1 // Bad block test
#define CLI_FLASH_TEST_LAST_ECC_FAIL	1 // Last ECC failure page address test

#define CLI_FLASH_READ_OTP				0 // Read OTP section
#define CLI_FLASH_READ_BUFFER_MODE_DIS	1 // Disable buffer mode for reading flash memory
#define CLI_FLASH_READ_SHOW_ASCII		1 // Show data read from memory as ascii string
#define CLI_FLASH_READ_HEX_LINEWIDTH	16 // Number of characters printed on one line in ascii mode
#define CLI_FLASH_READ_ASCII_LINEWIDTH	128 // Number of characters printed on one line in ascii mode

#define CLI_FLASH_ERASE_DISABLE			0 // Disable flash erase operation

#define CLI_FLASH_WRITE_LIPSUM			0 // Write Lorem Ipsum sample data to
#define CLI_FLASH_WRITE_EXEC			1 // enables program execute after loading buffer to write data to flash memory

void cli_init(UART_HandleTypeDef * huart1);
void cli_loop(void);

#endif
