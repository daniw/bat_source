/*
 * aux_io_ctrl.h
 *
 *  Created on: May 25, 2025
 *      Author: Andreas
 */

#ifndef SRC_AUX_IO_CTRL_H_
#define SRC_AUX_IO_CTRL_H_

#include "stdint.h"
#include "stm32f3xx_hal.h"
#include "stdio.h"
#include "driver_pca9554.h"

#define PCA9554_ADDRESS_HW_REV 0x70
#define PCA9554_ADDRESS_UI     0x72

#define AUX_IO_CTRL_EN        0
#define AUX_IO_CONF_CURR      1
#define AUX_IO_CONF_ISO       2
#define AUX_IO_SHUNT_ISO      3
#define AUX_IO_I_OUT_GAIN     4
#define AUX_IO_I_1A_DIS       5
#define AUX_IO_DISCHARGE      6
#define AUX_IO_ADC_SYNC_RESET 7
#define AUX_IO_ON_REQ         8
#define AUX_IO_EXPANDER       9

#define BTN_OK_Pin GPIO_PIN_13
#define BTN_OK_GPIO_Port GPIOC
#define ENCODER_A_Pin GPIO_PIN_0
#define ENCODER_A_GPIO_Port GPIOC
#define ENCODER_B_Pin GPIO_PIN_1
#define ENCODER_B_GPIO_Port GPIOC
#define TEMP_PRIM_Pin GPIO_PIN_2
#define TEMP_PRIM_GPIO_Port GPIOC
#define BTN_ESC_Pin GPIO_PIN_3
#define BTN_ESC_GPIO_Port GPIOC
#define V_IN_Pin GPIO_PIN_0
#define V_IN_GPIO_Port GPIOA
#define V_HV_Pin GPIO_PIN_1
#define V_HV_GPIO_Port GPIOA
#define I_BAT_Pin GPIO_PIN_2
#define I_BAT_GPIO_Port GPIOA
#define V_OUT_Pin GPIO_PIN_3
#define V_OUT_GPIO_Port GPIOA
#define VREF_UC_Pin GPIO_PIN_4
#define VREF_UC_GPIO_Port GPIOA
#define I_OUT_Pin GPIO_PIN_4
#define I_OUT_GPIO_Port GPIOC
#define TEMP_CURRENT_Pin GPIO_PIN_5
#define TEMP_CURRENT_GPIO_Port GPIOC
#define TEMP_SEC_Pin GPIO_PIN_2
#define TEMP_SEC_GPIO_Port GPIOB
#define ADC_DRDY_Pin GPIO_PIN_10
#define ADC_DRDY_GPIO_Port GPIOB
#define ADC_DRDY_EXTI_IRQn EXTI15_10_IRQn
#define I_ISO_Pin GPIO_PIN_12
#define I_ISO_GPIO_Port GPIOB
#define V_TERM_Pin GPIO_PIN_13
#define V_TERM_GPIO_Port GPIOB
#define I_1A_REF_Pin GPIO_PIN_14
#define I_1A_REF_GPIO_Port GPIOB
#define TEMP_TRAFO_Pin GPIO_PIN_15
#define TEMP_TRAFO_GPIO_Port GPIOB
#define I_OUT_GAIN_Pin GPIO_PIN_6
#define I_OUT_GAIN_GPIO_Port GPIOC
#define CONF_CURR_Pin GPIO_PIN_7
#define CONF_CURR_GPIO_Port GPIOC
#define DISCHARGE_Pin GPIO_PIN_8
#define DISCHARGE_GPIO_Port GPIOC
#define CONF_ISO_Pin GPIO_PIN_9
#define CONF_ISO_GPIO_Port GPIOC
#define SHUNT_ISO_Pin GPIO_PIN_8
#define SHUNT_ISO_GPIO_Port GPIOA
#define ADC_SYNC_RESET_Pin GPIO_PIN_9
#define ADC_SYNC_RESET_GPIO_Port GPIOA
#define I_1A_DIS_Pin GPIO_PIN_12
#define I_1A_DIS_GPIO_Port GPIOA
#define SPI_NSS_Pin GPIO_PIN_15
#define SPI_NSS_GPIO_Port GPIOA
#define ON_REQ_Pin GPIO_PIN_10
#define ON_REQ_GPIO_Port GPIOC
#define UI_PRESENT_Pin GPIO_PIN_11
#define UI_PRESENT_GPIO_Port GPIOC
#define CROWBAR_LATCH_Pin GPIO_PIN_12
#define CROWBAR_LATCH_GPIO_Port GPIOC
#define CTRL_EN_Pin GPIO_PIN_2
#define CTRL_EN_GPIO_Port GPIOD
#define BTN_OUT_Pin GPIO_PIN_4
#define BTN_OUT_GPIO_Port GPIOB
#define I2C_ALERT_Pin GPIO_PIN_5
#define I2C_ALERT_GPIO_Port GPIOB


#define AUX_IO_READ_OUT (HAL_GPIO_ReadPin(BTN_OUT_GPIO_Port, BTN_OUT_Pin) == 0 )
#define AUX_IO_READ_ESC (HAL_GPIO_ReadPin(BTN_ESC_GPIO_Port, BTN_ESC_Pin) == 0 )
#define AUX_IO_READ_OK  (HAL_GPIO_ReadPin(BTN_OK_GPIO_Port, BTN_OK_Pin) == 0   )

#define AUX_IO_UI_LED_SENSP_GREEN  0x40
#define AUX_IO_UI_LED_SENSP_ORANGE 0x80
#define AUX_IO_UI_LED_SENSN_GREEN  0x01
#define AUX_IO_UI_LED_SENSN_ORANGE 0x02
#define AUX_IO_UI_LED_OUTP_GREEN  0x10
#define AUX_IO_UI_LED_OUTP_ORANGE 0x20
#define AUX_IO_UI_LED_OUTN_GREEN  0x04
#define AUX_IO_UI_LED_OUTN_ORANGE 0x08

void aux_io_ctrl_GPIO_Init(void);

void aux_io_ctrl_PCA9554_Init(void);
/**
 * Prints all input pin states
 */
void aux_io_ctrl_print_inputs(void);

/**
 * Prints all output pin states
 */
void aux_io_ctrl_print_Output(void);

/**
 * Function used to manually set the GPIOs from CLI
 * @param pin pin number as defined above
 * @param value value to set the pin (0,1)
 */
void aux_io_ctrl_manual_set_io(uint8_t pin, uint8_t value);



#endif /* SRC_AUX_IO_CTRL_H_ */
