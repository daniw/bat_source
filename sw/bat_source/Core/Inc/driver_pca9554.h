/*
 * driver_pca9554.h
 *
 *  Created on: May 25, 2025
 *      Author: Andreas
 */

#ifndef INC_DRIVER_PCA9554_H_
#define INC_DRIVER_PCA9554_H_

#include <stdint.h>
#include "i2c.h"

// Local register state
typedef struct {
	uint8_t address;
    uint8_t output;
    uint8_t config;
} PCA9554_handle;



// PCA9554 Registers
#define PCA9554_INPUT    0x00
#define PCA9554_OUTPUT   0x01
#define PCA9554_POLARITY 0x02
#define PCA9554_CONFIG   0x03

/**
 * Set direction for all pins at once
 * @param address i2c device address
 * @param direction 8 bit word containing all direction bits. 1 = input
 */
void pca9554_set_direction(PCA9554_handle* hpca9554, uint8_t direction);

/**
 * Set output for all pins at once
 * @param address i2c device address
 * @param value 8 bit word containing output values
 */
void pca9554_set_output(PCA9554_handle* hpca9554, uint8_t value);

/**
 * Read Input of all pins
 * @param address i2c device address
 * @return value of the pins
 *
 */
uint8_t pca9554_read_input(PCA9554_handle *hpca9554) ;


/**
 * Set direction for single pin
 * @param address i2c device address
 * @param pin pin index
 * @param direction 1= input, 0 = output
 */
void pca9554_set_pin_direction(PCA9554_handle* hpca9554, uint8_t pin, uint8_t direction) ;

/**
 * Set output of single pin
 * @param address i2c device address
 * @param pin pin index
 * @param value 8 bit value of single bit
 */
void pca9554_set_pin_output(PCA9554_handle* hpca9554, uint8_t pin, uint8_t value) ;

/**
 * Read Input at single pin
 * @param address i2c device address
 * @param pin pin index
 * @return value of the  pin
 */
uint8_t pca9554_read_pin(PCA9554_handle* hpca9554, uint8_t pin) ;


#endif /* INC_DRIVER_PCA9554_H_ */
