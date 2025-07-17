/*
 * l2hal_bme280_i2c.c
 *
 *  Created on: Jul 17, 2025
 *      Author: Shakti
 */

#include "../include/l2hal_bme280_i2c_private.h"
#include "../../../../include/l2hal_errors.h"
#include <stdbool.h>

void L2HAL_BME280_I2C_Init(L2HAL_BME280_I2C_ContextStruct* context, I2C_HandleTypeDef* bus, uint8_t address)
{
	context->I2CHandle = bus;
	context->BusAddress = address;

	if (HAL_OK != HAL_I2C_IsDeviceReady(context->I2CHandle, context->BusAddress, 1, L2HAL_BME280_I2C_DETECTION_TIMEOUT))
	{
		L2HAL_Error(Generic);
	}

	/* Check device ID */
	uint8_t deviceId;
	L2HAL_BME280_I2C_ReadRegisters(context, L2HAL_BME280_I2C_REGISTER_ID, &deviceId, 1);

	if (L2HAL_BME280_I2C_DEVICE_ID != deviceId)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_BME280_I2C_ResetSensor(context);

	/* Waiting for NVM data copied to registers */
	while (!L2HAL_BME280_I2C_IsMeasurementCompleted(context)) {}

	/* Reading temperature compensation */
	L2HAL_BME280_I2C_TemperatureCompensationRawStruct temperatureCompensationRaw;
	L2HAL_BME280_I2C_ReadRegisters(context, L2HAL_BME280_I2C_REGISTER_BASE_TEMPERATURE_COMPENSATION, (uint8_t*)&temperatureCompensationRaw, sizeof(temperatureCompensationRaw));

	context->temperatureCompensationData.T1 = (double)temperatureCompensationRaw.T1;
	context->temperatureCompensationData.T2 = (double)temperatureCompensationRaw.T2;
	context->temperatureCompensationData.T3 = (double)temperatureCompensationRaw.T3;
}

void L2HAL_BME280_I2C_ReadRegisters(L2HAL_BME280_I2C_ContextStruct* context, uint8_t baseAddress, uint8_t* buffer, uint8_t count)
{
	if (HAL_OK != HAL_I2C_Mem_Read(context->I2CHandle, context->BusAddress, baseAddress, I2C_MEMADD_SIZE_8BIT, buffer, count, L2HAL_BME280_I2C_IO_TIMEOUT))
	{
		L2HAL_Error(Generic);
	}
}

void L2HAL_BME280_I2C_WriteRegister(L2HAL_BME280_I2C_ContextStruct* context, uint8_t address, uint8_t value)
{
	if (HAL_OK != HAL_I2C_Mem_Write(context->I2CHandle, context->BusAddress, address, I2C_MEMADD_SIZE_8BIT, &value, 1, L2HAL_BME280_I2C_IO_TIMEOUT))
	{
		L2HAL_Error(Generic);
	}
}

void L2HAL_BME280_I2C_ResetSensor(L2HAL_BME280_I2C_ContextStruct* context)
{
	L2HAL_BME280_I2C_WriteRegister(context, L2HAL_BME280_I2C_REGISTER_RESET, L2HAL_BME280_I2C_RESET_COMMAND);
}

void L2HAL_BME280_I2C_StartForcedMeasurement
(
	L2HAL_BME280_I2C_ContextStruct* context,
	enum L2HAL_BME280_I2C_OVERSAMPLING_MODE temperatureOversampling,
	enum L2HAL_BME280_I2C_OVERSAMPLING_MODE humidityOversampling,
	enum L2HAL_BME280_I2C_OVERSAMPLING_MODE pressureOversampling
)
{
	/* Control humidity */
	L2HAL_BME280_I2C_WriteRegister(context, L2HAL_BME280_I2C_REGISTER_HUMIDITY_CONTROL, humidityOversampling);

	/* Control temperature and pressure and run measurement */
	L2HAL_BME280_I2C_WriteRegister(context, L2HAL_BME280_I2C_REGISTER_MEASUREMENT_CONTROL, (temperatureOversampling << 5) | (pressureOversampling << 2) | 0b01);

}

bool L2HAL_BME280_I2C_IsMeasurementCompleted(L2HAL_BME280_I2C_ContextStruct* context)
{
	uint8_t status;
	L2HAL_BME280_I2C_ReadRegisters(context, L2HAL_BME280_I2C_REGISTER_STATUS, &status, 1);

	return (0 == (status & 0b000010001));
}

L2HAL_BME280_I2C_RawMeasurementsStruct L2HAL_BME280_I2C_GetMeasurementRaw(L2HAL_BME280_I2C_ContextStruct* context)
{
	L2HAL_BME280_I2C_RawMeasurementsStruct result;
	uint8_t data[8];

	L2HAL_BME280_I2C_ReadRegisters(context, L2HAL_BME280_I2C_REGISTER_BASE_DATA_REGISTER, data, 8);

	((uint8_t*)&result.temperature)[3] = 0x00;
	((uint8_t*)&result.temperature)[2] = data[5] >> 4; /* 0xFC */
	((uint8_t*)&result.temperature)[1] = data[3]; /* 0xFA */
	((uint8_t*)&result.temperature)[0] = data[4]; /* 0xFB */

	((uint8_t*)&result.humidity)[1] = data[6]; /* 0xFD */
	((uint8_t*)&result.humidity)[0] = data[7]; /* 0xFE */

	((uint8_t*)&result.pressure)[3] = 0x00;
	((uint8_t*)&result.pressure)[2] = data[2] >> 4; /* 0xF9 */
	((uint8_t*)&result.pressure)[1] = data[0]; /* 0xF7 */
	((uint8_t*)&result.pressure)[0] = data[1]; /* 0xF8 */

	return result;
}

double L2HAL_BME280_I2C_GetTemperatureKelvin(L2HAL_BME280_I2C_ContextStruct* context, double rawTemperature)
{
	double var1;
	double var2;

	var1 = (rawTemperature / 16384.0 - (context->temperatureCompensationData.T1) / 1024.0) * context->temperatureCompensationData.T2;

	var2 = ((rawTemperature / 131072.0 - (context->temperatureCompensationData.T1) / 8192.0)
			* (rawTemperature / 131072.0 - (context->temperatureCompensationData.T1) / 8192.0))
			* context->temperatureCompensationData.T3;

	return (var1 + var2) / 5120.0 + L2HAL_BME280_I2C_ZERO_CELSIUS_IN_KELVINS;
}

