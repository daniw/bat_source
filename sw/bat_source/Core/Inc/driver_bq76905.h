/*
 * driver_bq76905.h
 *
 *  Created on: Jun 14, 2025
 *      Author: ahorat
 */

#ifndef SRC_DRIVER_BQ76905_H_
#define SRC_DRIVER_BQ76905_H_

#include <stdint.h>
#include <stdbool.h>

// Register addresses
#define BQ76905_COMMAND_ALERT_A        0x02
#define BQ76905_COMMAND_STATUS_A       0x03
#define BQ76905_COMMAND_ALERT_B        0x04
#define BQ76905_COMMAND_STATUS_B       0x05

#define BQ76905_COMMAND_BAT_STATUS     0x12
#define BQ76905_COMMAND_VCELL_1        0x14
#define BQ76905_COMMAND_VCELL_2        0x16
#define BQ76905_COMMAND_VCELL_3        0x28
#define BQ76905_COMMAND_VCELL_4        0x1A
#define BQ76905_COMMAND_VCELL_5        0x1C
#define BQ76905_COMMAND_VREG18         0x22
#define BQ76905_COMMAND_VSS            0x24
#define BQ76905_COMMAND_VSTACK         0x26
#define BQ76905_COMMAND_TEMP_INT       0x28
#define BQ76905_COMMAND_TEMP_SENSOR    0x2A

#define BQ76905_COMMAND_CURRENT_RAW    0x36
#define BQ76905_COMMAND_CURRENT_CC2    0x3A
#define BQ76905_COMMAND_CURRENT_CC1    0x3C
#define BQ76905_COMMAND_ALARM_STATUS   0x62
#define BQ76905_COMMAND_ALARM_RAW      0x64
#define BQ76905_COMMAND_ALARM_EN       0x66
#define BQ76905_COMMAND_CTRL_FET       0x68
#define BQ76905_COMMAND_CTRL_REGOUT    0x69
#define BQ76905_COMMAND_CTRL_DSG_PWM   0x6A
#define BQ76905_COMMAND_CTRL_CHG_PWM   0x6C



// Use little Endian
#define BQ76905_SUBCOMMAND_ADDRESS_LOW    0x3E
#define BQ76905_SUBCOMMAND_ADDRESS_HIGH   0x3F

#define BQ76905_SUBCOMMAND_RESET_PASSQ      0x0005 
#define BQ76905_SUBCOMMAND_EXIT_DEEPSLEEP   0x000E
#define BQ76905_SUBCOMMAND_DEEPSLEEP        0x000F
#define BQ76905_SUBCOMMAND_SHUTDOWN         0x0010
#define BQ76905_SUBCOMMAND_RESET            0x0012
#define BQ76905_SUBCOMMAND_FET_ENABLE       0x0022
#define BQ76905_SUBCOMMAND_SEAL             0x0030
#define BQ76905_SUBCOMMAND_SET_CFGUPDATE    0x0090
#define BQ76905_SUBCOMMAND_EXIT_CFGUPDATE   0x0092
#define BQ76905_SUBCOMMAND_SLEEP_ENABLE     0x0099
#define BQ76905_SUBCOMMAND_SLEEP_DISABLE    0x009A

#define BQ76905_TRANSFER_BUFFER_LOW       0x40
#define BQ76905_TRANSFER_BUFFER_HIGH      0x5F


class BQ76905 {
public:
    struct{
    uint8_t safetyAlertA;
    uint8_t safetyStatusA;
    uint8_t safetyAlertB;
    uint8_t safetyStatusB;
} SafetyRegisters;

struct{
    uint16_t BatteryStatus;
    uint16_t CellVoltages[5];
} CellVoltageRegisters;


struct{
    uint16_t Reg18Voltage;
    uint16_t VssVoltage;
    uint16_t StackVoltage;
    uint16_t IntTemp;
    uint16_t TSTemp;
} VoltageRegisters;

struct{
    int32_t RawCurrent;
    int16_t CC2Current;
    int16_t CC1Current;
} CurrentRegisters;


struct{
    uint16_t AlarmStatus;
    uint16_t AlarmRawStatus;
    uint16_t AlarmEnable;
    uint8_t FETCtrl;
    uint8_t RegoutCtrl;
    uint16_t DSGFetPWM;
    uint16_t CHGFetPWM;
} SystemCtrl;

    BQ76905(uint8_t i2c_address);

    /**
     * Initializes the BQ76905 device and applies configuration.
     * Should be called once during setup.
     */
    void begin(void);

    /**
     * Starts asynchronous read of all measurement values.
     * Results will be stored in internal arrays.
     */
    void readAllValuesAsync(void);
    
    /**
     * Starts asynchronous read of all measurement values.
     * Results will be stored in internal arrays.
     */
    void readAllValues(void);

    
private:
    uint8_t address;
    bool crcEnabled;

    enum AsyncReadState {
        IDLE,
        READING_CELL_1,
        READING_CELL_2,
        READING_CELL_3,
        READING_CELL_4,
        READING_TEMP,
        READING_ALERT,
        READING_PROTECTION
    } asyncState;

    void processNextAsyncRead(void);
    void onI2CComplete(void);
};


#endif /* SRC_DRIVER_BQ76905_H_ */
