/*
 * driver_bq76905_config.h
 *
 *  Created on: Jun 14, 2025
 *      Author: ahorat
 */

#ifndef INC_DRIVER_BQ76905_CONFIG_H_
#define INC_DRIVER_BQ76905_CONFIG_H_

#define BQ76905_I2C_ADDRESS 0x10

// Configuration values in SI units
#define UV_THRESHOLD_V      2.5     // Undervoltage threshold in volts
#define OV_THRESHOLD_V      4.2     // Overvoltage threshold in volts
#define UV_DELAY_S          1.0     // Undervoltage delay in seconds
#define OV_DELAY_S          1.0     // Overvoltage delay in seconds
#define CHG_DETECTION_A     0.05    // Charge detection current in Amps
#define DSG_DETECTION_A     0.05    // Discharge detection current in Amps
#define TEMP_THRESHOLD_C    60.0    // Temperature threshold in Celsius

// Conversion macros
#define VOLTAGE_TO_REG(v)   ((uint8_t)(((v) * 1000.0 - 1000.0) / 20.0))  // Example: 2.5V -> 75
#define DELAY_TO_REG(s)     ((uint8_t)((s) * 10))                        // Example: 1s -> 10 (100ms units)
#define CURRENT_TO_REG(a)   ((uint8_t)((a) * 1000.0 / 10.0))             // Example: 0.05A -> 5
#define TEMP_TO_REG(c)      ((uint8_t)((c) + 40))                        // Example: 60C -> 100

// Register-compatible values
#define UV_THRESHOLD_REG    VOLTAGE_TO_REG(UV_THRESHOLD_V)
#define OV_THRESHOLD_REG    VOLTAGE_TO_REG(OV_THRESHOLD_V)
#define UV_DELAY_REG        DELAY_TO_REG(UV_DELAY_S)
#define OV_DELAY_REG        DELAY_TO_REG(OV_DELAY_S)
#define CHG_DETECTION_REG   CURRENT_TO_REG(CHG_DETECTION_A)
#define DSG_DETECTION_REG   CURRENT_TO_REG(DSG_DETECTION_A)
#define TEMP_THRESHOLD_REG  TEMP_TO_REG(TEMP_THRESHOLD_C)



#endif /* INC_DRIVER_BQ76905_CONFIG_H_ */
