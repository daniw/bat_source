/*
 * aux_io_ctrl.c
 *
 *  Created on: May 25, 2025
 *      Author: Andreas
 */


#include "aux_io_ctrl.h"
#include "main.h"

PCA9554_handle hpca_hw_rev;
PCA9554_handle hpca_UI;

void aux_io_ctrl_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, I_OUT_GAIN_Pin|CONF_CURR_Pin|CONF_ISO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SHUNT_ISO_Pin|ADC_SYNC_RESET_Pin|I_1A_DIS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CTRL_EN_GPIO_Port, CTRL_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : BTN_OK_Pin BTN_ESC_Pin DISCHARGE_Pin ON_REQ_Pin
                           UI_PRESENT_Pin CROWBAR_LATCH_Pin */
  GPIO_InitStruct.Pin = BTN_OK_Pin|BTN_ESC_Pin|DISCHARGE_Pin
                          |UI_PRESENT_Pin|CROWBAR_LATCH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : ADC_DRDY_Pin BTN_OUT_Pin I2C_ALERT_Pin */
  GPIO_InitStruct.Pin = ADC_DRDY_Pin|BTN_OUT_Pin|I2C_ALERT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : I_OUT_GAIN_Pin CONF_CURR_Pin CONF_ISO_Pin */
  GPIO_InitStruct.Pin = I_OUT_GAIN_Pin|CONF_CURR_Pin|CONF_ISO_Pin|ON_REQ_Pin
		  | DISCHARGE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : SHUNT_ISO_Pin ADC_SYNC_RESET_Pin I_1A_DIS_Pin */
  GPIO_InitStruct.Pin = SHUNT_ISO_Pin|ADC_SYNC_RESET_Pin|I_1A_DIS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : CTRL_EN_Pin */
  GPIO_InitStruct.Pin = CTRL_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CTRL_EN_GPIO_Port, &GPIO_InitStruct);
}

void aux_io_ctrl_PCA9554_Init(void)
{

  hpca_hw_rev.address = PCA9554_ADDRESS_HW_REV;
  pca9554_set_direction(&hpca_hw_rev, 0xFF);

  hpca_UI.address = PCA9554_ADDRESS_UI;
  pca9554_set_direction(&hpca_UI, 0x0);
}


/**
 * Prints all input pin states
 */
void aux_io_ctrl_print_inputs(void){
	printf("Input Pins: \n");
	printf("Button Out: 	%d\n", HAL_GPIO_ReadPin(GPIOB, BTN_OUT_Pin));
	printf("Button OK: 		%d\n", HAL_GPIO_ReadPin(GPIOC, BTN_OK_Pin));
	printf("Button ESC:	 	%d\n", HAL_GPIO_ReadPin(GPIOC, BTN_ESC_Pin));

	printf("I2C Alert: 		%d\n", HAL_GPIO_ReadPin(GPIOB, I2C_ALERT_Pin));
	printf("ADC_DRDY: 		%d\n", HAL_GPIO_ReadPin(GPIOB, ADC_DRDY_Pin));
	printf("UI Present: 	%d\n", HAL_GPIO_ReadPin(GPIOC, UI_PRESENT_Pin));
	printf("Crowbar Latch:	%d\n", HAL_GPIO_ReadPin(GPIOC, CROWBAR_LATCH_Pin));

	printf("HW Revision: 	%d\n", pca9554_read_input(&hpca_hw_rev));
}

/**
 * Prints all output pin states
 */
void aux_io_ctrl_print_Output(void){
	printf("Output Pins: \n");
	printf("[0] Ctrl En: 		%d\n", HAL_GPIO_ReadPin(GPIOD, CTRL_EN_Pin));
	printf("[1] Config Current: %d\n", HAL_GPIO_ReadPin(GPIOC, CONF_CURR_Pin));
	printf("[2] Config Iso: 	%d\n", HAL_GPIO_ReadPin(GPIOC, CONF_ISO_Pin));
	printf("[3] Shunt ISO: 		%d\n", HAL_GPIO_ReadPin(GPIOA, SHUNT_ISO_Pin));
	printf("[4] Current Gain: 	%d\n", HAL_GPIO_ReadPin(GPIOC, I_OUT_GAIN_Pin));
	printf("[5] 1A Disable: 	%d\n", HAL_GPIO_ReadPin(GPIOA, I_1A_DIS_Pin));
	printf("[6] Discharge: 		%d\n", HAL_GPIO_ReadPin(GPIOC, DISCHARGE_Pin));

	printf("[7] ADC Sync/Reset: %d\n", HAL_GPIO_ReadPin(GPIOA, ADC_SYNC_RESET_Pin));
	printf("[8] ON Request: 	%d\n", HAL_GPIO_ReadPin(GPIOC, ON_REQ_Pin));

	printf("[9] UI LEDs: 		%d\n", pca9554_read_input(&hpca_UI));
}

/**
 * Function used to manually set the GPIOs from CLI
 * @param pin pin number as defined above
 * @param value value to set the pin (0,1)
 */
void aux_io_ctrl_manual_set_io(uint8_t pin, uint8_t value)
{
	switch (pin) {
	case 0: HAL_GPIO_WritePin(GPIOD, CTRL_EN_Pin, value);
	return;
	case 1: HAL_GPIO_WritePin(GPIOC, CONF_CURR_Pin, value);
	return;
	case 2: HAL_GPIO_WritePin(GPIOC, CONF_ISO_Pin, value);
	return;
	case 3: HAL_GPIO_WritePin(GPIOA, SHUNT_ISO_Pin, value);
	return;
	case 4: HAL_GPIO_WritePin(GPIOC, I_OUT_GAIN_Pin, value);
	return;
	case 5: HAL_GPIO_WritePin(GPIOA, I_1A_DIS_Pin, value);
	return;
	case 6: HAL_GPIO_WritePin(GPIOC, DISCHARGE_Pin, value);
	return;
	case 7: HAL_GPIO_WritePin(GPIOA, ADC_SYNC_RESET_Pin, value);
	return;
	case 8: HAL_GPIO_WritePin(GPIOC, ON_REQ_Pin, value);
	return;
	case 9: pca9554_set_output(&hpca_UI, value);
	return;

		default:
			return;
	}
}
