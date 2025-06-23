/*
 * driver_bq76905.c
 *
 *  Created on: Jun 14, 2025
 *      Author: ahorat
 */


#include "driver_bq76905.h"

extern "C" {
#include "i2c.h"
}
BQ76905::BQ76905(uint8_t address)
    : address(address) {}

void BQ76905::begin(void) {
    // Apply configuration from config header
 //   configure();
	uint8_t command[16];
	command[0] = BQ76905_SUBCOMMAND_ADDRESS_LOW;
	command[1] = 0x90;
	command[2] = 0x00;
	i2c_WriteBlocking(address, command, 3);

	command[0] = BQ76905_SUBCOMMAND_ADDRESS_LOW;
	command[1] = 0x1B;
	command[2] = 0x90;
	command[3] = 0x04;
	i2c_WriteBlocking(address, command, 4);

	command[0] = 0x60;
	command[1] = 0x50;
	command[2] = 0x05;
	i2c_WriteBlocking(address, command, 3);

	command[0] = BQ76905_SUBCOMMAND_ADDRESS_LOW;
	command[1] = 0x92;
	command[2] = 0x00;
	i2c_WriteBlocking(address, command, 3);

	command[0] = BQ76905_SUBCOMMAND_ADDRESS_LOW;
	command[1] = 0x22;
	command[2] = 0x00;
	i2c_WriteBlocking(address, command, 3);
}

//void BQ76905::configure() {
//    // Not implemented
//}

//uint8_t BQ76905::readRegister(uint8_t reg) {
//    uint8_t value = 0;
//	i2c_WriteBlocking(address, &reg, 1);
//    i2c_ReadBlocking(address, &value, 1);
//    return value;
//}
//
//void BQ76905::writeRegister(uint8_t reg, uint8_t value) {
//    uint8_t data[2] = { reg, value };
//
//	i2c_WriteBlocking(address, data, 2);
//}

void BQ76905::readAllValues(void) {
	uint8_t command = BQ76905_COMMAND_ALERT_A;
	i2c_WriteBlocking(address, &command, 1);
	i2c_ReadBlocking(address, (uint8_t*)&SafetyRegisters , sizeof(SafetyRegisters));


	command = BQ76905_COMMAND_BAT_STATUS;
	i2c_WriteBlocking(address, &command, 1);
	i2c_ReadBlocking(address, (uint8_t*)&CellVoltageRegisters , sizeof(CellVoltageRegisters));


	command = BQ76905_COMMAND_VREG18;
	i2c_WriteBlocking(address, &command, 1);
	i2c_ReadBlocking(address, (uint8_t*)&VoltageRegisters , sizeof(VoltageRegisters));


	command = BQ76905_COMMAND_CURRENT_RAW;
	i2c_WriteBlocking(address, &command, 1);
	i2c_ReadBlocking(address, (uint8_t*)&CurrentRegisters , sizeof(CurrentRegisters));


	command = BQ76905_COMMAND_ALARM_STATUS;
	i2c_WriteBlocking(address, &command, 1);
	i2c_ReadBlocking(address, (uint8_t*)&SystemCtrl , sizeof(SystemCtrl));



}


// Initiates asynchronous read of all cell voltages
void BQ76905::readAllValuesAsync(void) {
	// ToDo: Not implemented properly
    //asyncState = READ_CELL_1;
    //uint8_t reg = BQ76905_CELL_VOLTAGE_1;
    //i2c_Read(i2cAddress, asyncBuffer, 2, this);
}


// This function should be called by the I2C system when a read completes
void BQ76905::onI2CComplete(void) {
	// ToDo: Not implemented properly
    //switch (asyncState) {
    //    case READ_CELL_1:
    //        cellVoltages[0] = (asyncBuffer[0] << 8) | asyncBuffer[1];
    //        asyncState = READ_CELL_2;
    //        asyncBuffer[0] = BQ76905_CELL_VOLTAGE_2;
    //        i2c_Read(i2cAddress, asyncBuffer, 2, this);
    //        break;
    //    case READ_CELL_2:
    //        cellVoltages[1] = (asyncBuffer[0] << 8) | asyncBuffer[1];
    //        asyncState = READ_CELL_3;
    //        asyncBuffer[0] = BQ76905_CELL_VOLTAGE_3;
    //        i2c_Read(i2cAddress, asyncBuffer, 2, this);
    //        break;
    //    case READ_CELL_3:
    //        cellVoltages[2] = (asyncBuffer[0] << 8) | asyncBuffer[1];
    //        asyncState = READ_CELL_4;
    //        asyncBuffer[0] = BQ76905_CELL_VOLTAGE_4;
    //        i2c_Read(i2cAddress, asyncBuffer, 2, this);
    //        break;
    //    case READ_CELL_4:
    //        cellVoltages[3] = (asyncBuffer[0] << 8) | asyncBuffer[1];
    //        asyncState = IDLE;
    //        break;
    //    case READ_TEMP:
    //        {
    //            uint16_t raw = (asyncBuffer[0] << 8) | asyncBuffer[1];
    //            temperature = (raw * 0.000763f) - 273.15f;
    //            asyncState = IDLE;
    //        }
    //        break;
    //    case READ_ALERT:
    //        alertStatus = asyncBuffer[0];
    //        asyncState = IDLE;
    //        break;
    //    case READ_PROTECTION:
    //        protectionStatus = asyncBuffer[0];
    //        asyncState = IDLE;
    //        break;
    //    default:
    //        asyncState = IDLE;
    //        break;
    //}
}




