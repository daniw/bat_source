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

#include "stm32f3xx_hal.h"
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

#define LINE_BUF_SIZE 128
#define MAX_NUM_ARGS 8

#define CLI_ENABLED

void cli_init(UART_HandleTypeDef * huart1);
void cli_loop(void);

#endif
