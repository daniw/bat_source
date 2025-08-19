/*
 * driver_pca9554.c
 *
 *  Created on: May 25, 2025
 *      Author: Andreas
 */

#include "driver_pca9554.h"
#include "i2c.h"


/**
 * Internal function to read registers
 * @param address i2c address
 * @param reg register address
 * @return value at specific location
 */
uint8_t pca9554_read_register(PCA9554_handle* hpca9554, uint8_t reg) {
    uint8_t value = 0;
	i2c_WriteBlocking(hpca9554->address, &reg, 1);
    i2c_ReadBlocking(hpca9554->address, &value, 1);
    return value;
}

/**
 * Set direction for all pins at once
 * @param address i2c device address
 * @param direction 8 bit word containing all direction bits. 1 = input
 */
void pca9554_set_direction(PCA9554_handle* hpca9554, uint8_t direction)
{
	hpca9554->config = direction;
    uint8_t buffer[2] = {PCA9554_CONFIG, hpca9554->config};
    i2c_WriteBlocking(hpca9554->address, buffer, sizeof(buffer));
}
/**
 * Set output for all pins at once
 * @param address i2c device address
 * @param value 8 bit word containing output values
 */
void pca9554_set_output(PCA9554_handle* hpca9554, uint8_t value)
{
	hpca9554->output = value;
    uint8_t buffer[2] = {PCA9554_OUTPUT, hpca9554->output};
    i2c_WriteBlocking(hpca9554->address, buffer, sizeof(buffer));
}


/**
 * Read Input of all pins
 * @param address i2c device address
 * @return value of the pins
 *
 */
uint8_t pca9554_read_input(PCA9554_handle *hpca9554) {
    return pca9554_read_register(hpca9554, PCA9554_INPUT);
}



/**
 * Set direction for single pin
 * @param address i2c device address
 * @param pin pin index
 * @param direction 1= input, 0 = output
 */
void pca9554_set_pin_direction(PCA9554_handle* hpca9554, uint8_t pin, uint8_t direction) {
    if (direction) {
        hpca9554->config |= (1 << pin);
    } else {
        hpca9554->config &= ~(1 << pin);
    }
    uint8_t buffer[2] = {PCA9554_CONFIG, hpca9554->config};
    i2c_WriteBlocking(hpca9554->address, buffer, sizeof(buffer));
}

/**
 * Set output of single pin
 * @param address i2c device address
 * @param pin pin index
 * @param value 8 bit value of single bit
 */
void pca9554_set_pin_output(PCA9554_handle* hpca9554, uint8_t pin, uint8_t value) {
    if (value) {
        hpca9554->output |= (1 << pin);
    } else {
    	hpca9554->output &= ~(1 << pin);
    }
    uint8_t buffer[2] = {PCA9554_OUTPUT, hpca9554->output};
    i2c_WriteBlocking(hpca9554->address, buffer, sizeof(buffer));
}


/**
 * Read Input at single pin
 * @param address i2c device address
 * @param pin pin index
 * @return value of the  pin
 */
uint8_t pca9554_read_pin(PCA9554_handle* hpca9554, uint8_t pin) {
    return (pca9554_read_register(hpca9554, PCA9554_INPUT) >> pin) & 0x01;
}





