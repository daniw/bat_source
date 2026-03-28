/*
 * OPT3004.h
 *
 *  Created on: October 10, 2025
 *      Author: ahorat
 */

#ifndef INC_OPT3004_H_
#define INC_OPT3004_H_

#include <stdint.h>
#include "i2c.h"

// Local register state
typedef struct {
	uint8_t address;
	uint16_t config;
} opt3004_handle;

// OPT3004 Register Map
#define OPT3004_REG_RESULT                    0x00
#define OPT3004_REG_CONFIGURATION             0x01
#define OPT3004_REG_LOW_LIMIT                 0x02
#define OPT3004_REG_HIGH_LIMIT                0x03
#define OPT3004_REG_MANUFACTURER_ID           0x7E
#define OPT3004_REG_DEVICE_ID                 0x7F

// OPT3004 Result Register
#define OPT3004_RESULT_EXP                    0xF000u
#define OPT3004_RESULT_FRACT                  0x0FFFu
#define OPT3004_RESULT_MIN                    0x0000u
#define OPT3004_RESULT_MAX                    0xBFFFu
#define OPT3004_RESULT_LUX_PER_LSB            0.01f


// OPT3004 Configuration Register Flags
#define OPT3004_CONFIG_RN_AUTO_SCALE          0xC000
#define OPT3004_CONFIG_RN_MASK                0xF000
#define OPT3004_CONFIG_CT_100_mS              0x0000
#define OPT3004_CONFIG_CT_800_mS              0x0800
#define OPT3004_CONFIG_CT_MASK                0x0800
#define OPT3004_CONFIG_M_SHUTDOWN             0x0000
#define OPT3004_CONFIG_M_SINGLE               0x0200
#define OPT3004_CONFIG_M_CONTINUOUS           0x0400
#define OPT3004_CONFIG_M_MASK                 0x0600
#define OPT3004_CONFIG_OVF_MASK               0x0100
#define OPT3004_CONFIG_CRF_MASK               0x0080
#define OPT3004_CONFIG_FH_MASK                0x0040
#define OPT3004_CONFIG_FL_MASK                0x0020
#define OPT3004_CONFIG_L_TRANSPARENT          0x0000
#define OPT3004_CONFIG_L_LATCH                0x0010
#define OPT3004_CONFIG_L_MASK                 0x0010
#define OPT3004_CONFIG_POL_LOW                0x0000
#define OPT3004_CONFIG_POL_HIGH               0x0008
#define OPT3004_CONFIG_POL_MASK               0x0008
#define OPT3004_CONFIG_ME_MASK                0x0004
#define OPT3004_CONFIG_FC_ONE                 0x0000
#define OPT3004_CONFIG_FC_TWO                 0x0001
#define OPT3004_CONFIG_FC_FOUR                0x0002
#define OPT3004_CONFIG_FC_EIGHT               0x0003
#define OPT3004_CONFIG_FC_MASK                0x0003

#define OPT3004_CONFIG_DEFAULT (OPT3004_CONFIG_RN_AUTO_SCALE | \
                                OPT3004_CONFIG_CT_100_mS | \
                                OPT3004_CONFIG_M_CONTINUOUS)

// OPT3004 Manufacturer ID and Device ID
#define OPT3004_MANUFACTURER_ID               0x5449u
#define OPT3004_DEVICE_ID                     0x3001u

// OPT3004 Device Addresses
#define OPT3004_DEVICE_ADDRESS_GND              0x88
#define OPT3004_DEVICE_ADDRESS_VDD              0x8A
#define OPT3004_DEVICE_ADDRESS_SDA              0x8C
#define OPT3004_DEVICE_ADDRESS_SCL              0x8E



/**
 * Initialise the OPT3004 Device with the default config
 * @param hopt3004 opt3004 handle to the device. 
 * @param address i2c device address
 */
uint8_t opt3004_init(opt3004_handle *hopt3004, uint8_t address);

/**
 * Write the specified register to the OPT3004
 * @param hopt3004 opt3004 handle to the device. 
 * @param reg_address register address
 * @param value data to be written
 */
uint8_t opt3004_writeRegister(opt3004_handle *hopt3004, uint8_t reg_address, uint16_t value);

/**
 * read the specified register from the OPT3004
 * @param hopt3004 opt3004 handle to the device. 
 * @param reg_address register address
 * @result data to be written
 */
uint16_t opt3004_readRegister(opt3004_handle *hopt3004, uint8_t reg_address);

/**
 * Checks the communication with the OPT3004 device
 * @param hopt3004 opt3004 handle to the device. 
 */
uint8_t opt3004_check(opt3004_handle *hopt3004);

/**
 * Read the light intensity
 * @param hopt3004 opt3004 handle to the device.
 */
uint32_t opt3004_readLux(opt3004_handle *hopt3004);


/**
 * Shutdown the OPT3004 device
 * @param hopt3004 opt3004 handle to the device. 
 */
uint8_t opt3004_shutdown(opt3004_handle *hopt3004);

#endif /* INC_OPT3004_H_ */
