/*
 * units_converter.h
 *
 *  Created on: Jul 18, 2025
 *      Author: Shakti
 */

#ifndef INCLUDE_CONVERTERS_UNITS_CONVERTER_H_
#define INCLUDE_CONVERTERS_UNITS_CONVERTER_H_

#define ZERO_CELSIUS_IN_KELVINS (273.15)

/**
 * Convert temperature from Kelvins to Celsius
 */
double TemperatureKelvinsToCelsius(double kelvins);

/**
 * Convert temperature from Kelvins to Fahrenheits
 */
double TemperatureKelvinsToFahrenheits(double kelvins);

/**
 * Convert pressure from Pascals to mmHg
 */
double PressurePascalsToMmHg(double pascals);

/**
 * Convert pressure from Pascals to hPa
 */
double PressurePascalsToHPa(double pascals);

/**
 * Convert pressure from Pascals to inHg
 */
double PressurePascalsToInHg(double pascals);

#endif /* INCLUDE_CONVERTERS_UNITS_CONVERTER_H_ */
