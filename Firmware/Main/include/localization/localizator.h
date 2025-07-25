/*
 * localizator.h
 *
 *  Created on: Jul 18, 2025
 *      Author: Shakti
 */

#ifndef INCLUDE_LOCALIZATION_LOCALIZATOR_H_
#define INCLUDE_LOCALIZATION_LOCALIZATOR_H_

#include "../converters/units_converter.h"
#include "../configuration/config_reader_writer.h"

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
	LOCALIZATION_PRESSURE_UNIT_INHG = 2
};

/**
 * Localization settings are stored here
 */
typedef struct
{
	/**
	 * Localization config file context
	 */
	ConfigContextStruct LocalizationConfigContext;

	/**
	 * Use this temperature unit
	 */
	enum LOCALIZATION_TEMPERATURE_UNITS TemperatureUnit;

	/**
	 * Use this pressure unit
	 */
	enum LOCALIZATION_PRESSURE_UNITS PressureUnit;

} LocalizationContextStruct;

/**
 * Initialize localizator and load settings from given file
 */
LocalizationContextStruct LocalizatorInit(char* path);

/**
 * Set temperature unit
 */
void LocalizatorSetTemperatureUnit(LocalizationContextStruct* context, enum LOCALIZATION_TEMPERATURE_UNITS unit);

/**
 * Set pressure unit
 */
void LocalizatorSetPressureUnit(LocalizationContextStruct* context, enum LOCALIZATION_PRESSURE_UNITS unit);

/**
 * Convert temperature from Kelvins to local format
 */
double LocalizatorGetLocalizedTemperature(LocalizationContextStruct* context, double t);

/**
 * Get localized temperature unit (like K, C or F)
 */
char* LocalizatorGetLocalizedTemperatureUnit(LocalizationContextStruct* context);

/**
 * Get localized temperature precision template (like "%.1f")
 */
char* LocalizatorGetLocalizedTemperaturePrecisionTemplate(LocalizationContextStruct* context);

/**
 * Convert pressure from Pascals to local format
 */
double LocalizatorGetLocalizedPressure(LocalizationContextStruct* context, double p);

/**
 * Get localized pressure unit (like mmHg, hPa or inHg)
 */
char* LocalizatorGetLocalizedPressureUnit(LocalizationContextStruct* context);

/**
 * Get localized pressure precision template (like "%.1f", "%.0f", "%.4f%)
 */
char* LocalizatorGetLocalizedPressurePrecisionTemplate(LocalizationContextStruct* context);

#endif /* INCLUDE_LOCALIZATION_LOCALIZATOR_H_ */
