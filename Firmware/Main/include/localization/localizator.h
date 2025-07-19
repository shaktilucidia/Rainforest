/*
 * localizator.h
 *
 *  Created on: Jul 18, 2025
 *      Author: Shakti
 */

#ifndef INCLUDE_LOCALIZATION_LOCALIZATOR_H_
#define INCLUDE_LOCALIZATION_LOCALIZATOR_H_

#include "../converters/units_converter.h"

/**
 * Possible temperature units
 */
enum LOCALIZATION_TEMPERATURE_UNITS
{
	LOCALIZATION_TEMPERATURE_UNIT_KELVIN = 0,
	LOCALIZATION_TEMPERATURE_UNIT_CELSIUS = 1,
	LOCALIZATION_TEMPERATURE_UNIT_FAHRENHEIT = 2
};

/**
 * Possible pressure units
 */
enum LOCALIZATION_PRESSURE_UNITS
{
	LOCALIZATION_PRESSURE_UNIT_MMHG = 0,
	LOCALIZATION_PRESSURE_UNIT_HPA = 1,
	LOCALIZATION_PRESSURE_UNIT_INHG = 3
};

/**
 * Localization settings are stored here
 */
typedef struct
{
	/**
	 * Use this temperature unit
	 */
	enum LOCALIZATION_TEMPERATURE_UNITS TemperatureUnit;

	/**
	 * Use this pressure unit
	 */
	enum LOCALIZATION_PRESSURE_UNITS PressureUnit;

} LocalizationContextrStruct;

/**
 * Initialize localizator and provide default settings
 */
LocalizationContextrStruct LocalizatorInit(void);

/**
 * Set temperature unit
 */
void LocalizatorSetTemperatureUnit(LocalizationContextrStruct* context, enum LOCALIZATION_TEMPERATURE_UNITS unit);

/**
 * Set pressure unit
 */
void LocalizatorSetPressureUnit(LocalizationContextrStruct* context, enum LOCALIZATION_PRESSURE_UNITS unit);

/**
 * Convert temperature from Kelvins to local format
 */
double LocalizatorGetLocalizedTemperature(LocalizationContextrStruct* context, double t);

/**
 * Get localized temperature unit (like K, C or F)
 */
char* LocalizatorGetLocalizedTemperatureUnit(LocalizationContextrStruct* context);

/**
 * Convert pressure from Pascals to local format
 */
double LocalizatorGetLocalizedPressure(LocalizationContextrStruct* context, double p);

/**
 * Get localized pressure unit (like mmHg, hPa or inHg)
 */
char* LocalizatorGetLocalizedPressureUnit(LocalizationContextrStruct* context);

#endif /* INCLUDE_LOCALIZATION_LOCALIZATOR_H_ */
