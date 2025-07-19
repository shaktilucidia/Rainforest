/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "diag/trace.h"
#include "main.h"
#include "global_variables.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 empty sample (trace via NONE).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the NONE output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace-impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

/* Colors */
FMGL_API_ColorStruct OffColor = { .R = FMGL_API_MAX_CHANNEL_BRIGHTNESS, .G = FMGL_API_MAX_CHANNEL_BRIGHTNESS, .B = FMGL_API_MAX_CHANNEL_BRIGHTNESS };
FMGL_API_ColorStruct OnColor = { .R = 0x00, .G = 0x00, .B = 0x00 };

int main(int argc, char* argv[])
{
	/* Mandatory for STM32 HAL */
	HAL_Init();

	/* Our hardware */
	L2HAL_Init();
	HAL_InitHardware();

	/* RAM initialization */
	L2HAL_LY68L6400_Init
	(
		&RamContext,
		&SPI1Handle,

		HAL_PSRAM_CS_PORT,
		HAL_PSRAM_CS_PIN
	);

	/* Display initialization */
	L2HAL_SSD1683_Init
	(
		&DisplayContext,
		&SPI2Handle,

		HAL_DISPLAY_BUSY_PORT,
		HAL_DISPLAY_BUSY_PIN,

		HAL_DISPLAY_RESET_PORT,
		HAL_DISPLAY_RESET_PIN,

		HAL_DISPLAY_DC_PORT,
		HAL_DISPLAY_DC_PIN,

		HAL_DISPLAY_CS_PORT,
		HAL_DISPLAY_CS_PIN,

		OffColor,

		false, /* Do periodic full refresh? */
		3 /* Do periodic full refresh each this frames count */
	);

	/* Local sensor initialization */
	L2HAL_BME280_I2C_Init
	(
		&LocalSensor,
		&I2C1_Handle,
		L2HAL_BME280_I2C_MAIN_ADDRESS
	);

	/* SD Card driver initialization */
	enum L2HAL_SDCard_InitResult sdCardInitResult = L2HAL_SDCard_Init
	(
		&SDCardContext,
		&SPI1Handle,

		HAL_SDCARD_CS_PORT,
		HAL_SDCARD_CS_PIN
	);

	if (NoCardInserted == sdCardInitResult)
	{
		L2HAL_Error(Generic); /* TODO: Show "Insert SD-card" message */
	}
	else if (Success != sdCardInitResult)
	{
		L2HAL_Error(Generic); /* Failed to initialize SD-card */
	}

	/* Attaching FMGL to display */
	FmglContext = FMGL_API_AttachToDriver
	(
		&DisplayContext,
		&L2HAL_SSD1683_GetWidth,
		&L2HAL_SSD1683_GetHeight,
		(void (*) (void* deviceContext, FMGL_API_ColorStruct color))&L2HAL_SSD1683_SetActiveColor,
		(void (*) (void* deviceContext, uint16_t x, uint16_t y))&L2HAL_SSD1683_DrawPixel,
		(FMGL_API_ColorStruct (*) (void* deviceContext, uint16_t x, uint16_t y))&L2HAL_SSD1683_GetPixel,
		(void (*) (void* deviceContext))&L2HAL_SSD1683_PushFramebufferPartial,

		OffColor, /* Blanking color */
		(void (*)(void *, FMGL_API_ColorStruct))&L2HAL_SSD1683_ClearFramebuffer /* Blanking method */
	);

	FMGL_API_FontSettings font;
	FMGL_API_Font fontData = FMGL_FontTerminusRegular12Init();
	FMGL_API_XBMTransparencyMode transparencyMode = FMGL_XBMTransparencyModeNormal;

	/* Font settings */
	font.Font = &fontData;
	font.Scale = 1;
	font.CharactersSpacing = 0;
	font.LinesSpacing = 0;
	font.FontColor = &OnColor;
	font.BackgroundColor = &OffColor;
	font.Transparency = &transparencyMode;

	uint16_t linePosition = 0;
	uint16_t width;
	uint16_t height;

	/* Hardware self-test */
	FMGL_API_RenderTextWithLineBreaks(&FmglContext, &font, 0, linePosition, &width, &height, false, "Hardware self-test, it will take a while...");
	linePosition += height;
	FMGL_API_PushFramebuffer(&FmglContext);

	HAL_HardwareSelfTest();

	FMGL_API_RenderTextWithLineBreaks(&FmglContext, &font, 0, linePosition, &width, &height, false, "OK, mounting SD card...");
	linePosition += height;
	FMGL_API_PushFramebuffer(&FmglContext);

	/* Mounting SD card */
	if (!FS_MountSDCard())
	{
		/* TODO: Offer to format SD-card*/
		FMGL_API_RenderTextWithLineBreaks(&FmglContext, &font, 0, linePosition, &width, &height, false, "FAILED");
		linePosition += height;
		FMGL_API_PushFramebuffer(&FmglContext);
		L2HAL_Error(Generic);
	}

	FMGL_API_RenderTextWithLineBreaks(&FmglContext, &font, 0, linePosition, &width, &height, false, "Done");
	linePosition += height;
	FMGL_API_PushFramebuffer(&FmglContext);

	/* Loading fonts */
	FMGL_API_Font mainFontData = FMGL_LoadableFont_Init
	(
		&MainFontContext,
		"System/Fonts/FreeSans32.fmglfont",
		&RamContext,
		(void (*)(void*, uint32_t, uint32_t, uint8_t*))&L2HAL_LY68L6400_MemoryWrite,
		(void (*)(void*, uint32_t, uint32_t, uint8_t*))&L2HAL_LY68L6400_MemoryRead,
		100000
	);

	MainFont.Font = &mainFontData;
	MainFont.Scale = 1;
	MainFont.CharactersSpacing = 0;
	MainFont.LinesSpacing = 0;
	MainFont.FontColor = &OnColor;
	MainFont.BackgroundColor = &OffColor;
	MainFont.Transparency = &transparencyMode;

	/* Main font ready */
	FMGL_API_ClearScreen(&FmglContext);

	/* Loading localization settings */
	LocalizationContext = LocalizatorInit();
	LocalizatorSetTemperatureUnit(&LocalizationContext, LOCALIZATION_TEMPERATURE_UNIT_CELSIUS);
	LocalizatorSetPressureUnit(&LocalizationContext, LOCALIZATION_PRESSURE_UNIT_MMHG);

	while(true)
	{
		L2HAL_BME280_I2C_StartForcedMeasurement
		(
			&LocalSensor,
			L2HAL_BME280_I2C_OVERSAMPLING_16,
			L2HAL_BME280_I2C_OVERSAMPLING_16,
			L2HAL_BME280_I2C_OVERSAMPLING_16
		);

		while (!L2HAL_BME280_I2C_IsMeasurementCompleted(&LocalSensor)) {}

		L2HAL_BME280_I2C_RawMeasurementsStruct rawMeasurements = L2HAL_BME280_I2C_GetMeasurementRaw(&LocalSensor);

		/* Creature-readable values */
		L2HAL_BME280_I2C_CreatureReadableMeasurementsStruct creatureReadableValues = L2HAL_BME280_I2C_GetCreatureReadableValues(&LocalSensor, rawMeasurements);

		char buffer[32];
		linePosition = 0;

		/* Temperature */
		sprintf
		(
			buffer,
			"Temperature: %.1f%s",
			LocalizatorGetLocalizedTemperature(&LocalizationContext, creatureReadableValues.Temperature),
			LocalizatorGetLocalizedTemperatureUnit(&LocalizationContext)
		);

		FMGL_API_RenderTextWithLineBreaks(&FmglContext, &MainFont, 0, linePosition, &width, &height, false, buffer);
		linePosition += height;

		/* Humidity */
		sprintf(buffer, "Humidity: %.1f%%", creatureReadableValues.Humidity);
		FMGL_API_RenderTextWithLineBreaks(&FmglContext, &MainFont, 0, linePosition, &width, &height, false, buffer);
		linePosition += height;

		/* Pressure */
		sprintf
		(
			buffer,
			"Pressure: %.1f %s",
			LocalizatorGetLocalizedPressure(&LocalizationContext, creatureReadableValues.Pressure),
			LocalizatorGetLocalizedPressureUnit(&LocalizationContext)
		);

		FMGL_API_RenderTextWithLineBreaks(&FmglContext, &MainFont, 0, linePosition, &width, &height, false, buffer);
		linePosition += height;

		FMGL_API_PushFramebuffer(&FmglContext);
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
