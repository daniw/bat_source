/*
 * driver_bq76905.c
 *
 *  Created on: Jun 14, 2025
 *      Author: ahorat
 */

#include "bq76905.h"
#include "bq76905_config.h"
#include "bq76905_data_memory_map.h"
#include "i2c.h"
#include "event.h"
#include "timer.h"


    /**
     * Configures the whole IC according to defined configuration macros
     */
    void BQ76905_configure(BQ76905_handle* handle);



    /**
     * read a direct command in blocking mode
     */
    uint16_t readCommand(BQ76905_handle* handle, uint8_t command, uint8_t len);

    /**
     * Read a direct command in blocking mode
     */
    void writeCommand(BQ76905_handle* handle, uint8_t command, uint16_t data, uint8_t len);


    /**
     * Starts next read, after the last one is finished.
     */
    void BQ76905_onI2CComplete(BQ76905_handle* handle);


    /**
     * Defined in bq76905_config.h
     */
    void BQ76905_configure(BQ76905_handle* handle);


    /**
     * Initializes the BQ76905 device and applies configuration.
     * Should be called once during setup.
     */
void BQ76905_init(BQ76905_handle* handle, uint8_t i2c_address){
	handle->address = i2c_address;
	handle->asyncState = IDLE;
	uint8_t command = BQ76905_COMMAND_BAT_STATUS;
	i2c_WriteBlocking(handle->address, &command, 1);
	i2c_ReadBlocking(handle->address, (uint8_t*) &handle->CellVoltageRegisters,
			sizeof(handle->CellVoltageRegisters));

	//if(handle->CellVoltageRegisters.BatteryStatus & 0x80 == 0){
		BQ76905_configure(handle);
	//}else{
	//	printf("Skipped BMS reconfiguration: %x\r\n", handle->CellVoltageRegisters.BatteryStatus);
	//}
	timer_add(1000,  TIMER_TYPE_TICK, EVENT_BMS_TIMER, 0);
}

/**************************** Public Functions ***********************************/
void BQ76905_readAllValues(BQ76905_handle* handle) {
	uint8_t command = BQ76905_COMMAND_ALERT_A;
	i2c_WriteBlocking(handle->address, &command, 1);
	i2c_ReadBlocking(handle->address, (uint8_t*) &handle->SafetyRegisters,
			sizeof(handle->SafetyRegisters));

	command = BQ76905_COMMAND_BAT_STATUS;
	i2c_WriteBlocking(handle->address, &command, 1);
	i2c_ReadBlocking(handle->address, (uint8_t*) &handle->CellVoltageRegisters,
			sizeof(handle->CellVoltageRegisters));

	command = BQ76905_COMMAND_VREG18;
	i2c_WriteBlocking(handle->address, &command, 1);
	i2c_ReadBlocking(handle->address, (uint8_t*) &handle->VoltageRegisters,
			sizeof(handle->VoltageRegisters));

	command = BQ76905_COMMAND_CURRENT_RAW;
	i2c_WriteBlocking(handle->address, &command, 1);
	i2c_ReadBlocking(handle->address, (uint8_t*) &handle->CurrentRegisters,
			sizeof(handle->CurrentRegisters));

	command = BQ76905_COMMAND_ALARM_STATUS;
	i2c_WriteBlocking(handle->address, &command, 1);
	i2c_ReadBlocking(handle->address, (uint8_t*) &handle->SystemCtrl, sizeof(handle->SystemCtrl));

	BQ76905_ReadRamRegister(handle, BQ76905_SUBCOMMAND_PASSQ, (uint8_t*)&handle->Accumulator, 12);


}

// Initiates asynchronous read of all cell voltages
void BQ76905_readAllValuesAsync(BQ76905_handle* handle) {
	if(handle->asyncState == IDLE){
	handle->asyncState = READ_SAFETY;
	handle->command_tx = BQ76905_COMMAND_ALERT_A;
	i2c_Write(handle->address, &handle->command_tx, 1, 0, 0);
	i2c_Read(handle->address, (uint8_t*)&handle->SafetyRegisters, sizeof(handle->SafetyRegisters),&BQ76905_onI2CComplete, handle);
}
}


inline void BQ76905_resetAlarmStatus(BQ76905_handle* handle){
	writeCommand(handle, BQ76905_COMMAND_ALARM_STATUS, 0xFFFF,2 );
}

inline void BQ76905_resetChargeAccumulator(BQ76905_handle* handle){
	BQ76905_WriteRamRegister(handle,BQ76905_SUBCOMMAND_RESET_PASSQ, 0, 0);
}

inline void BQ76905_resetDevice(BQ76905_handle* handle){
	BQ76905_WriteRamRegister(handle,BQ76905_SUBCOMMAND_RESET, 0, 0);
}

inline void BQ76905_Deepsleep(BQ76905_handle* handle){
	BQ76905_WriteRamRegister(handle,BQ76905_SUBCOMMAND_DEEPSLEEP, 0, 0);
}

inline void BQ76905_ExitDeepsleep(BQ76905_handle* handle){
	BQ76905_WriteRamRegister(handle,BQ76905_SUBCOMMAND_EXIT_DEEPSLEEP, 0, 0);

}

inline void BQ76905_Shutdown(BQ76905_handle* handle){
	BQ76905_WriteRamRegister(handle,BQ76905_SUBCOMMAND_SHUTDOWN, 0, 0);
}

inline void BQ76905_FETEnable(BQ76905_handle* handle){
	BQ76905_WriteRamRegister(handle,BQ76905_SUBCOMMAND_FET_ENABLE, 0, 0);
}

inline void BQ76905_Seal(BQ76905_handle* handle){
	BQ76905_WriteRamRegister(handle,BQ76905_SUBCOMMAND_SEAL, 0, 0);
}

inline void BQ76905_SleepEnable(BQ76905_handle* handle){
	BQ76905_WriteRamRegister(handle,BQ76905_SUBCOMMAND_SLEEP_ENABLE, 0, 0);
}

inline void BQ76905_SleepDisable(BQ76905_handle* handle){
	BQ76905_WriteRamRegister(handle,BQ76905_SUBCOMMAND_SLEEP_DISABLE, 0, 0);
}

inline void BQ76905_GetFWVersion(BQ76905_handle* handle, uint8_t* out){
	BQ76905_ReadRamRegister(handle, BQ76905_SUBCOMMAND_FW_VERSION, out, 6);

}

inline uint16_t BQ76905_GetHWVersion(BQ76905_handle* handle){
	uint16_t buffer;
	BQ76905_ReadRamRegister(handle, BQ76905_SUBCOMMAND_HW_VERSION, (uint8_t*)&buffer, 2);
	return buffer;
}

    inline void BQ76905_WriteSecurityKey(BQ76905_handle* handle){
    	uint8_t command[7];

    	command[0] = BQ76905_SUBCOMMAND_ADDRESS_LOW;
    	command[1] = BQ76905_SUBCOMMAND_SECURITY_KEYS & 0xFF;
    	command[2] = (BQ76905_SUBCOMMAND_SECURITY_KEYS>>8) & 0xFF;
    	command[3] = BQ76905_CONFIG_VALUE_SECURITY_ACCESS_KEY_1 & 0xFF;
    	command[4] = (BQ76905_CONFIG_VALUE_SECURITY_ACCESS_KEY_1>>8) & 0xFF;
    	command[5] = BQ76905_CONFIG_VALUE_SECURITY_ACCESS_KEY_2 & 0xFF;
    	command[6] = (BQ76905_CONFIG_VALUE_SECURITY_ACCESS_KEY_2>>8) & 0xFF;
    	i2c_WriteBlocking(handle->address, command, 7);
    	for (int i=1; i<7; i++)
    	{
    		command[1] += command[i];
    	}

    	command[0] = BQ76905_TRANSFER_CHKSUM;
    	command[1] = ~command[1];
    	command[2] = 8;
    	i2c_WriteBlocking(handle->address, command, 3);
}

    inline void BQ76905_EnableBalancing(BQ76905_handle* handle, uint8_t cellmask){
	BQ76905_WriteRamRegister(handle,BQ76905_SUBCOMMAND_CB_ACTIVE_CELLS, cellmask<<1, 1);
}

    inline void BQ76905_RecoverProtection(BQ76905_handle* handle, uint8_t mask){
	BQ76905_WriteRamRegister(handle,BQ76905_SUBCOMMAND_PROT_RECOVERY, mask, 1);
}


void BQ76905_WriteRamRegister(BQ76905_handle* handle, uint16_t reg, uint16_t data, uint8_t datalen)
{
	uint8_t command[5];

	command[0] = BQ76905_SUBCOMMAND_ADDRESS_LOW;
	command[1] = reg & 0xFF;
	command[2] = (reg>>8) & 0xFF;
	command[3] = data & 0xFF;
	command[4] = (data>>8) & 0xFF;
	i2c_WriteBlocking(handle->address, command, 3+datalen);
	for (int i=2; i<3+datalen; i++)
	{
		command[1] += command[i];
	}

	command[0] = BQ76905_TRANSFER_CHKSUM;
	command[1] = ~command[1];
	command[2] = 4+datalen;
	i2c_WriteBlocking(handle->address, command, 3);
}

/**************************** Private Functions ***********************************/

void BQ76905_ReadRamRegister(BQ76905_handle* handle, uint16_t command, uint8_t* out, uint8_t len){
	uint8_t buffer[3];
	buffer[0] = BQ76905_SUBCOMMAND_ADDRESS_LOW;
	buffer[1] = command & 0Xff;
	buffer[2] = command>>8;
	i2c_WriteBlocking(handle->address, buffer, 3);
	buffer[0] = BQ76905_TRANSFER_BUFFER_LOW;
	i2c_WriteBlocking(handle->address, buffer, 1);
	i2c_ReadBlocking(handle->address, (uint8_t*) out, len);
}

/**
 * read a direct command in blocking mode
 */
uint16_t readCommand(BQ76905_handle* handle, uint8_t command, uint8_t len)
{
	uint8_t buffer[2];
	buffer[0]= command;
	i2c_WriteBlocking(handle->address, buffer, 1);
	i2c_ReadBlocking(handle->address, (uint8_t*) buffer,len);
	return *(uint16_t*)buffer;
}

/**
 * Read a direct command in blocking mode
 */
void writeCommand(BQ76905_handle* handle, uint8_t command, uint16_t data, uint8_t len)
{
	uint8_t buffer[3];
	buffer[0] = command;
	buffer[1] = data & 0xFF;
	buffer[2] = data >> 8;
	i2c_WriteBlocking(handle->address, buffer, len+1);

}


// This function should be called by the I2C system when a read completes
void BQ76905_onI2CComplete(BQ76905_handle* handle) {
	switch (handle->asyncState) {
	case READ_SAFETY:
		handle->asyncState = READ_CELLVOLTAGE;
		handle->command_tx = BQ76905_COMMAND_BAT_STATUS;
		i2c_Write(handle->address, &handle->command_tx, 1, 0, 0);
		i2c_Read(handle->address, (uint8_t*) &handle->CellVoltageRegisters, sizeof(handle->CellVoltageRegisters),&BQ76905_onI2CComplete, handle);
		break;

	case READ_CELLVOLTAGE:
		handle->asyncState = READ_VOLTAGE;
		handle->command_tx = BQ76905_COMMAND_VREG18;
		i2c_Write(handle->address, &handle->command_tx, 1, 0, 0);
		i2c_Read(handle->address, (uint8_t*) &handle->VoltageRegisters, sizeof(handle->VoltageRegisters),&BQ76905_onI2CComplete, handle);
		break;

	case READ_VOLTAGE:
		handle->asyncState = READ_CURRENT;
		handle->command_tx = BQ76905_COMMAND_CURRENT_RAW;
		i2c_Write(handle->address, &handle->command_tx, 1, 0, 0);
		i2c_Read(handle->address, (uint8_t*) &handle->CurrentRegisters, sizeof(handle->CurrentRegisters),&BQ76905_onI2CComplete, handle);
		break;

	case READ_CURRENT:
		handle->asyncState = READ_ALARM;
		handle->command_tx = BQ76905_COMMAND_ALARM_STATUS;
		i2c_Write(handle->address, &handle->command_tx, 1, 0, 0);
		i2c_Read(handle->address, (uint8_t*) &handle->SystemCtrl, sizeof(handle->SystemCtrl),&BQ76905_onI2CComplete, handle);
		break;

	case READ_ALARM:
		handle->asyncState = READ_PASSQ;
		handle->command_tx2[0] = BQ76905_SUBCOMMAND_ADDRESS_LOW;
		handle->command_tx2[1] = BQ76905_SUBCOMMAND_PASSQ & 0Xff;
		handle->command_tx2[2] = BQ76905_SUBCOMMAND_PASSQ >> 8;
		handle->command_tx2[3] = BQ76905_TRANSFER_BUFFER_LOW;
		i2c_Write(handle->address, handle->command_tx2, 3, 0, 0);
		i2c_Write(handle->address, &handle->command_tx2[3], 1, 0, 0);
		i2c_Read(handle->address, (uint8_t*) &handle->Accumulator, sizeof(handle->Accumulator),&BQ76905_onI2CComplete, handle);
		break;

	case READ_PASSQ:
		handle->asyncState = IDLE;
		break;
	default:
		break;
	}
}


