/*
 * localizator.c
 *
 *  Created on: Jul 19, 2025
 *      Author: Shakti
 */

#include "../../include/localization/localizator.h"
#include "../../libs/l2hal/include/l2hal_errors.h"

LocalizationContextrStruct LocalizatorInit(void)
{
	LocalizationContextrStruct localization = { 0 };

	localization.TemperatureUnit = LOCALIZATION_TEMPERATURE_UNIT_CELSIUS;
	localization.PressureUnit = LOCALIZATION_PRESSURE_UNIT_MMHG;

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
