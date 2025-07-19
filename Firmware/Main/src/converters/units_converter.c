/*
 * units_converter.c
 *
 *  Created on: Jul 18, 2025
 *      Author: Shakti
 */

#include "../../include/converters/units_converter.h"

double TemperatureKelvinsToCelsius(double kelvins)
{
	return kelvins - ZERO_CELSIUS_IN_KELVINS;
}

double TemperatureKelvinsToFahrenheits(double kelvins)
{
	return kelvins * 1.8 - 459.67;
}

double PressurePascalsToMmHg(double pascals)
{
	return pascals * 0.00750062;
}

double PressurePascalsToHPa(double pascals)
{
	return pascals / 100.0;
}

double PressurePascalsToInHg(double pascals)
{
	return pascals * 0.0002952998751;
}
