/*
 * localizator.c
 *
 *  Created on: Jul 19, 2025
 *      Author: Shakti
 */

#include "../../include/localization/localizator.h"
#include "../../libs/l2hal/l2hal_config.h"
#include <stdio.h>
#include "../include/constants/addresses.h"
#include "../include/constants/localization.h"

LocalizationContextStruct LocalizatorInit(char* path)
{
	LocalizationContextStruct localization = { 0 };

	localization.LocalizationConfigContext = ConfigLoad
	(
		path,
		CONSTANTS_ADDRESSES_LOCALIZATION_CONFIG_BASE_ADDRESS,
		&RamContext,
		(void (*)(void*, uint32_t, uint32_t, uint8_t*))&L2HAL_LY68L6400_MemoryRead,
		(void (*)(void*, uint32_t, uint32_t, uint8_t*))&L2HAL_LY68L6400_MemoryWrite
	);

	bool isSuccess;

	/* Temperature unit */
	localization.TemperatureUnit = ConfigGetIntValueByKey
	(
		&localization.LocalizationConfigContext,
		CONSTATNS_LOCALIZATION_TEMPERATURE_UNIT_CONFIG_KEY,
		&isSuccess
	);

	if (!isSuccess)
	{
		L2HAL_Error(Generic);
	}

	/* Pressure */
	localization.PressureUnit = ConfigGetIntValueByKey
	(
		&localization.LocalizationConfigContext,
		CONSTATNS_LOCALIZATION_PRESSURE_UNIT_CONFIG_KEY,
		&isSuccess
	);

	if (!isSuccess)
	{
		L2HAL_Error(Generic);
	}

	return localization;
}

void LocalizatorSetTemperatureUnit(LocalizationContextStruct* context, enum LOCALIZATION_TEMPERATURE_UNITS unit)
{
	context->TemperatureUnit = unit;

	ConfigSetIntValueByKey(&context->LocalizationConfigContext, CONSTATNS_LOCALIZATION_TEMPERATURE_UNIT_CONFIG_KEY, unit);
}


void LocalizatorSetPressureUnit(LocalizationContextStruct* context, enum LOCALIZATION_PRESSURE_UNITS unit)
{
	context->PressureUnit = unit;

	ConfigSetIntValueByKey(&context->LocalizationConfigContext, CONSTATNS_LOCALIZATION_PRESSURE_UNIT_CONFIG_KEY, unit);
}

double LocalizatorGetLocalizedTemperature(LocalizationContextStruct* context, double t)
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

char* LocalizatorGetLocalizedTemperatureUnit(LocalizationContextStruct* context)
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

char* LocalizatorGetLocalizedTemperaturePrecisionTemplate(LocalizationContextStruct* context)
{
	/* For now we always return %.1f */
	return "%.1f";
}

double LocalizatorGetLocalizedPressure(LocalizationContextStruct* context, double p)
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

char* LocalizatorGetLocalizedPressureUnit(LocalizationContextStruct* context)
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

char* LocalizatorGetLocalizedPressurePrecisionTemplate(LocalizationContextStruct* context)
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
