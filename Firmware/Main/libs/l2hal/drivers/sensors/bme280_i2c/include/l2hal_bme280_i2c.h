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

#define L2HAL_BME280_I2C_ZERO_CELSIUS_IN_KELVINS (273.15)

/**
 * Oversampling modes for all three subsensors
 */
enum L2HAL_BME280_I2C_OVERSAMPLING_MODE
{
	L2HAL_BME280_I2C_OVERSAMPLING_DISABLE_SENSOR = 0b000,
	L2HAL_BME280_I2C_OVERSAMPLING_1 = 0b001,
	L2HAL_BME280_I2C_OVERSAMPLING_2 = 0b010,
	L2HAL_BME280_I2C_OVERSAMPLING_4 = 0b011,
	L2HAL_BME280_I2C_OVERSAMPLING_8 = 0b100,
	L2HAL_BME280_I2C_OVERSAMPLING_16 = 0b101,
};

/**
 * Temperature compensation data
 */
typedef struct
{
	/**
	 * Original T1 from Bosch datasheet, divided by 1024.0
	 */
	double T1_DIV_1K;

	/**
	 * Original T1 from Bosch datasheet, divided by 8192.0
	 */
	double T1_DIV_8K;

	double T2;

	double T3;

} L2HAL_BME280_I2C_TemperatureCompensationStruct;

/**
 * Pressure compensation data
 */
typedef struct
{
	double P1;

	double P2;

	double P3;

	double P4;

	double P5;

	double P6;

	double P7;

	double P8;

	double P9;

} L2HAL_BME280_I2C_PressureCompensationStruct;

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

	/**
	 * Data for temperature compensation
	 */
	L2HAL_BME280_I2C_TemperatureCompensationStruct TemperatureCompensationData;

	/**
	 * Data for pressure compensation
	 */
	L2HAL_BME280_I2C_PressureCompensationStruct PressureCompensationData;

} L2HAL_BME280_I2C_ContextStruct;

/**
 * Raw measurements
 */
typedef struct
{
	/**
	 * Raw temperature (always shifted to 20 bits-wide)
	 */
	int32_t Temperature;

	/**
	 * Raw humidity
	 */
	uint16_t Humidity;

	/**
	 * Raw pressure (always shifted to 20 bits-wide)
	 */
	uint32_t Pressure;

} L2HAL_BME280_I2C_RawMeasurementsStruct;

/**
 * Creature-readable values
 */
typedef struct
{
	/**
	 * Temperature in Kelvins
	 */
	double Temperature;

	double Humidity;

	double Pressure;
}
L2HAL_BME280_I2C_CreatureReadableMeasurementsStruct;

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
void L2HAL_BME280_I2C_StartForcedMeasurement
(
	L2HAL_BME280_I2C_ContextStruct* context,
	enum L2HAL_BME280_I2C_OVERSAMPLING_MODE temperatureOversampling,
	enum L2HAL_BME280_I2C_OVERSAMPLING_MODE humidityOversampling,
	enum L2HAL_BME280_I2C_OVERSAMPLING_MODE pressureOversampling
);

/**
 * Call it after L2HAL_BME280_I2C_StartForcedMeasurement to check if measurement completed
 */
bool L2HAL_BME280_I2C_IsMeasurementCompleted(L2HAL_BME280_I2C_ContextStruct* context);

/**
 * Get raw measurement data
 */
L2HAL_BME280_I2C_RawMeasurementsStruct L2HAL_BME280_I2C_GetMeasurementRaw(L2HAL_BME280_I2C_ContextStruct* context);

/**
 * Get creature-readable values
 */
L2HAL_BME280_I2C_CreatureReadableMeasurementsStruct L2HAL_BME280_I2C_GetCreatureReadableValues
(
	L2HAL_BME280_I2C_ContextStruct* context,
	L2HAL_BME280_I2C_RawMeasurementsStruct rawMeasurements
);


#endif /* DRIVERS_SENSORS_BME280_I2C_INCLUDE_L2HAL_BME280_I2C_H_ */
