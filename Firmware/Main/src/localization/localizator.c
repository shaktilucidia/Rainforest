/*
 * localizator.c
 *
 *  Created on: Jul 19, 2025
 *      Author: Shakti
 */

#include "../../include/localization/localizator.h"
#include "../../libs/l2hal/l2hal_config.h"
#include <stdio.h>

LocalizationContextrStruct LocalizatorInit(char* path)
{
	LocalizationContextrStruct localization = { 0 };

	localization.LocalizationConfigContext = ConfigLoad
	(
		path,
		200000,
		&RamContext,
		(void (*)(void*, uint32_t, uint32_t, uint8_t*))&L2HAL_LY68L6400_MemoryRead,
		(void (*)(void*, uint32_t, uint32_t, uint8_t*))&L2HAL_LY68L6400_MemoryWrite
	);

	bool isSuccess;

	/* Temperature unit */
	char* temperatureUnitRaw = ConfigGetStringValueByKey
	(
		&localization.LocalizationConfigContext,
		"temperature_unit",
		&isSuccess
	);

	sscanf(temperatureUnitRaw, "%d", (int*)&localization.TemperatureUnit);
	free(temperatureUnitRaw);

	/* Pressure */
	char* pressureUnitRaw = ConfigGetStringValueByKey
	(
		&localization.LocalizationConfigContext,
		"pressure_unit",
		&isSuccess
	);

	sscanf(pressureUnitRaw, "%d", (int*)&localization.PressureUnit);
	free(pressureUnitRaw);

	return localization;
}

void LocalizatorSetTemperatureUnit(LocalizationContextrStruct* context, enum LOCALIZATION_TEMPERATURE_UNITS unit)
{
	context->TemperatureUnit = unit;
}


void LocalizatorSetPressureUnit(LocalizationContextrStruct* context, enum LOCALIZATION_PRESSURE_UNITS unit)
{
	context->PressureUnit = unit;
}

double LocalizatorGetLocalizedTemperature(LocalizationContextrStruct* context, double t)
{
	switch (context->TemperatureUnit)
	{
		case LOCALIZATION_TEMPERATURE_UNIT_KELVIN:
			return t;

		case LOCALIZATION_TEMPERATURE_UNIT_CELSIUS:
			return TemperatureKelvinsToCelsius(t);

		case LOCALIZATION_TEMPERATURE_UNIT_FAHRENHEIT:
			return TemperatureKelvinsToFahrenheits(t);

		default:
			L2HAL_Error(Generic);
			return 0;
	}
}

char* LocalizatorGetLocalizedTemperatureUnit(LocalizationContextrStruct* context)
{
	switch (context->TemperatureUnit)
	{
		case LOCALIZATION_TEMPERATURE_UNIT_KELVIN:
			return "\x9C" "K";

		case LOCALIZATION_TEMPERATURE_UNIT_CELSIUS:
			return "\x9C" "C";

		case LOCALIZATION_TEMPERATURE_UNIT_FAHRENHEIT:
			return "\x9C" "F";

		default:
			L2HAL_Error(Generic);
			return NULL;
	}
}

char* LocalizatorGetLocalizedTemperaturePrecisionTemplate(LocalizationContextrStruct* context)
{
	/* For now we always return %.1f */
	return "%.1f";
}

double LocalizatorGetLocalizedPressure(LocalizationContextrStruct* context, double p)
{
	switch (context->PressureUnit)
	{
		case LOCALIZATION_PRESSURE_UNIT_MMHG:
			return PressurePascalsToMmHg(p);

		case LOCALIZATION_PRESSURE_UNIT_HPA:
			return PressurePascalsToHPa(p);

		case LOCALIZATION_PRESSURE_UNIT_INHG:
			return PressurePascalsToInHg(p);

		default:
			L2HAL_Error(Generic);
			return 0;
	}
}

char* LocalizatorGetLocalizedPressureUnit(LocalizationContextrStruct* context)
{
	switch (context->PressureUnit)
	{
		case LOCALIZATION_PRESSURE_UNIT_MMHG:
			return "mmHg";

		case LOCALIZATION_PRESSURE_UNIT_HPA:
			return "hPa";

		case LOCALIZATION_PRESSURE_UNIT_INHG:
			return "inHg";

		default:
			L2HAL_Error(Generic);
			return NULL;
	}
}

char* LocalizatorGetLocalizedPressurePrecisionTemplate(LocalizationContextrStruct* context)
{
	switch (context->PressureUnit)
	{
		case LOCALIZATION_PRESSURE_UNIT_MMHG:
			return "%.1f";

		case LOCALIZATION_PRESSURE_UNIT_HPA:
			return "%.0f";

		case LOCALIZATION_PRESSURE_UNIT_INHG:
			return "%.2f";

		default:
			L2HAL_Error(Generic);
			return NULL;
	}
}
