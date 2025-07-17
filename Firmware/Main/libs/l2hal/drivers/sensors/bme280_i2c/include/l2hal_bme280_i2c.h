/*
 * l2hal_bme280.h
 *
 *  Created on: Jul 17, 2025
 *      Author: Shakti
 */

#ifndef DRIVERS_SENSORS_BME280_I2C_INCLUDE_L2HAL_BME280_I2C_H_
#define DRIVERS_SENSORS_BME280_I2C_INCLUDE_L2HAL_BME280_I2C_H_

#include "../../../../mcu_dependent/l2hal_mcu.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * Sensor main address (SDO tied to ground)
 */
#define L2HAL_BME280_I2C_MAIN_ADDRESS (0x76 << 1)

/**
 * Sensor alternate address (SDO tied to VCC)
 */
#define L2HAL_BME280_I2C_ALTERNATE_ADDRESS (0x77 << 1)

/**
 * Sensor context
 */
typedef struct
{
	/**
	 * Talk with sensor via this I2C instance.
	 */
	I2C_HandleTypeDef* I2CHandle;

	/**
	 * Sensor bus address.
	 */
	uint8_t BusAddress;

} L2HAL_BME280_I2C_ContextStruct;

/**
 * Raw measurements
 */
typedef struct
{
	/**
	 * Raw temperature
	 */
	uint32_t temperature;

	/**
	 * Raw humidity
	 */
	uint16_t humidity;

	/**
	 * Raw pressure
	 */
	uint32_t pressure;

} L2HAL_BME280_I2C_RawMeasurementsStruct;


/**
 * Initialize sensor (we support only forced mode for now)
 */
void L2HAL_BME280_I2C_Init(L2HAL_BME280_I2C_ContextStruct* context, I2C_HandleTypeDef* bus, uint8_t address);

/**
 * Reset sensor (it will go to sleep mode)
 */
void L2HAL_BME280_I2C_ResetSensor(L2HAL_BME280_I2C_ContextStruct* context);

/**
 * Wake up sensor and start forced measurement, when measurement will be completed sensor will go to sleep again
 */
void L2HAL_BME280_I2C_StartForcedMeasurement(L2HAL_BME280_I2C_ContextStruct* context);

/**
 * Call it after L2HAL_BME280_I2C_StartForcedMeasurement to check if measurement completed
 */
bool L2HAL_BME280_I2C_IsMeasurementCompleted(L2HAL_BME280_I2C_ContextStruct* context);

/**
 * Get raw measurement data
 */
L2HAL_BME280_I2C_RawMeasurementsStruct L2HAL_BME280_I2C_GetMeasurementRaw(L2HAL_BME280_I2C_ContextStruct* context);


#endif /* DRIVERS_SENSORS_BME280_I2C_INCLUDE_L2HAL_BME280_I2C_H_ */
