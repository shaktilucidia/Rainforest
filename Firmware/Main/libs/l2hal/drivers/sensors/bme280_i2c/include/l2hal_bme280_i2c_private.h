/*
 * l2hal_bme280_i2c_private.h
 *
 *  Created on: Jul 17, 2025
 *      Author: Shakti
 */

#ifndef DRIVERS_SENSORS_BME280_I2C_INCLUDE_L2HAL_BME280_I2C_PRIVATE_H_
#define DRIVERS_SENSORS_BME280_I2C_INCLUDE_L2HAL_BME280_I2C_PRIVATE_H_

#include "l2hal_bme280_i2c.h"

/**
 * Sensor detection timeout in milliseconds
 */
#define L2HAL_BME280_I2C_DETECTION_TIMEOUT 100U

/**
 * Sensor I/O timeout (we use blocking mode)
 */
#define L2HAL_BME280_I2C_IO_TIMEOUT 100U

/**
 * Registers
 */
#define L2HAL_BME280_I2C_REGISTER_BASE_TEMPERATURE_COMPENSATION 0x88

#define L2HAL_BME280_I2C_REGISTER_ID 0xD0
#define L2HAL_BME280_I2C_REGISTER_RESET 0xE0
#define L2HAL_BME280_I2C_REGISTER_HUMIDITY_CONTROL 0xF2
#define L2HAL_BME280_I2C_REGISTER_STATUS 0xF3
#define L2HAL_BME280_I2C_REGISTER_MEASUREMENT_CONTROL 0xF4
#define L2HAL_BME280_I2C_REGISTER_BASE_DATA_REGISTER 0xF7


/**
 * Expected device ID
 */
#define L2HAL_BME280_I2C_DEVICE_ID 0x60

/**
 * Send this to reset register to reset device
 */
#define L2HAL_BME280_I2C_RESET_COMMAND 0xB6

/**
 * Temperature compensation data
 */
typedef struct
{
	uint16_t T1;

	int16_t T2;

	int16_t T3;

} L2HAL_BME280_I2C_TemperatureCompensationRawStruct;

/**
 * Read data from count registers starting from baseAddress into buffer
 */
void L2HAL_BME280_I2C_ReadRegisters(L2HAL_BME280_I2C_ContextStruct* context, uint8_t baseAddress, uint8_t* buffer, uint8_t count);

/**
 * Write value to register
 */
void L2HAL_BME280_I2C_WriteRegister(L2HAL_BME280_I2C_ContextStruct* context, uint8_t address, uint8_t value);

#endif /* DRIVERS_SENSORS_BME280_I2C_INCLUDE_L2HAL_BME280_I2C_PRIVATE_H_ */
