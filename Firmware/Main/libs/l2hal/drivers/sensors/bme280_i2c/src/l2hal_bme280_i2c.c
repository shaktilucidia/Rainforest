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

	context->temperatureCompensationData.T1_DIV_1K = (double)temperatureCompensationRaw.T1 / 1024.0;
	context->temperatureCompensationData.T1_DIV_8K = (double)temperatureCompensationRaw.T1 / 8192.0;
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

	uint32_t data_msb;
	uint32_t data_lsb;
	uint32_t data_xlsb;

    data_msb = (uint32_t)data[3] << 12;
    data_lsb = (uint32_t)data[4] << 4;
    data_xlsb = (uint32_t)data[5] >> 4;
    result.Temperature = data_msb | data_lsb | data_xlsb;

    data_msb = (uint32_t)data[6] << 8;
	data_lsb = (uint32_t)data[7];
	result.Humidity = data_msb | data_lsb;

    data_msb = (uint32_t)data[0] << 12;
    data_lsb = (uint32_t)data[1] << 4;
    data_xlsb = (uint32_t)data[2] >> 4;
    result.Pressure = data_msb | data_lsb | data_xlsb;

	return result;
}

double L2HAL_BME280_I2C_GetTemperatureKelvin(L2HAL_BME280_I2C_ContextStruct* context, double rawTemperature)
{
	double var1 = (rawTemperature / 16384.0 - context->temperatureCompensationData.T1_DIV_1K) * context->temperatureCompensationData.T2;

	double var2 = (rawTemperature / 131072.0) - context->temperatureCompensationData.T1_DIV_8K;

	double var3 = var2 * var2 * context->temperatureCompensationData.T3;

	return (var1 + var3) / 5120.0 + L2HAL_BME280_I2C_ZERO_CELSIUS_IN_KELVINS;
}

