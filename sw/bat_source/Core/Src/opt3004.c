/*
 * LP581X.h
 *
 *  Created on: October 10, 2025
 *      Author: ahorat
 */

#include "opt3004.h"

/**
 * Set direction for all pins at once
 * @param hopt3004 opt3004 handle to the device. 
 * @param address i2c device address
 */
uint8_t opt3004_init(opt3004_handle *hopt3004, uint8_t address) {
   hopt3004->address = address;
   hopt3004->config = OPT3004_CONFIG_DEFAULT;
   opt3004_writeRegister(hopt3004, OPT3004_REG_CONFIGURATION,hopt3004->config) ;
}

/**
 * Write the specified register to the OPT3004
 * @param hopt3004 opt3004 handle to the device. 
 * @param reg_address register address
 * @param value data to be written
 */
uint8_t opt3004_writeRegister(opt3004_handle *hopt3004, uint8_t reg_address, uint16_t value){
   uint8_t buf[3];
   buf[0] = reg_address;
   buf[1] = value>>8;
   buf[2] = value;
	return i2c_WriteBlocking(hopt3004->address, buf, 3);
}

/**
 * read the specified register from the OPT3004
 * @param hopt3004 opt3004 handle to the device. 
 * @param reg_address register address
 * @result data to be written
 */
uint16_t opt3004_readRegister(opt3004_handle *hopt3004, uint8_t reg_address){
   uint8_t result[2];
   result[0] = reg_address;
	i2c_WriteBlocking(hopt3004->address, result, 1);
	i2c_ReadBlocking(hopt3004->address, (uint8_t*) &result, 2);
   return (result[0] << 8) |result[1];
}

/**
 * Checks the communication with the OPT3004 device
 * @param hopt3004 opt3004 handle to the device. 
 */
uint8_t opt3004_check(opt3004_handle *hopt3004){
   uint16_t result;
   result = opt3004_readRegister(hopt3004, OPT3004_REG_MANUFACTURER_ID);
   if(result != OPT3004_MANUFACTURER_ID)
      return 0x1;
   result = opt3004_readRegister(hopt3004, OPT3004_REG_DEVICE_ID);
   if(result != OPT3004_DEVICE_ID)
      return 0x2;
   else
      return 0x0;
}

/**
 * Read the light intensity
 * @param hopt3004 opt3004 handle to the device.
 */
uint32_t opt3004_readLux(opt3004_handle *hopt3004){
   uint16_t result;
   result = opt3004_readRegister(hopt3004, OPT3004_REG_RESULT);
   return (uint32_t) (result & OPT3004_RESULT_FRACT) << (result >> 12);
}


/**
 * Shutdown the OPT3004 device
 * @param hopt3004 opt3004 handle to the device. 
 */
uint8_t opt3004_shutdown(opt3004_handle *hopt3004){
   hopt3004->config &= (~OPT3004_CONFIG_M_MASK);
   opt3004_writeRegister(hopt3004, OPT3004_REG_CONFIGURATION,hopt3004->config) ;
}

