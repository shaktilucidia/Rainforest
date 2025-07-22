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

		CONSTANTS_GENERIC_IS_DO_PERIODIC_FULL_REFRESH, /* Do periodic full refresh? */
		CONSTANTS_GENERIC_PERIODIC_FULL_REFRESH_PERIOD /* Do periodic full refresh each this frames count */
	);

	/* Local sensor initialization */
	L2HAL_BME280_I2C_Init
	(
		&LocalSensor,
		&I2C1_Handle,
		L2HAL_BME280_I2C_MAIN_ADDRESS
	);

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

	/* Early monospaced font */
	FMGL_API_Font earlyFontData = FMGL_FontTerminusRegular12Init();
	FMGL_API_XBMTransparencyMode transparencyMode = FMGL_XBMTransparencyModeNormal;

	EarlyFont.Font = &earlyFontData;
	EarlyFont.Scale = 1;
	EarlyFont.CharactersSpacing = 0;
	EarlyFont.LinesSpacing = 0;
	EarlyFont.FontColor = &OnColor;
	EarlyFont.BackgroundColor = &OffColor;
	EarlyFont.Transparency = &transparencyMode;

	/* Console init */
	Console = FMGL_ConsoleInit(&FmglContext, &EarlyFont);

	/* SD Card driver initialization */
	FMGL_ConsoleAddLine(&Console, "SD card initialization...");

	enum L2HAL_SDCard_InitResult sdCardInitResult = L2HAL_SDCard_Init
	(
		&SDCardContext,
		&SPI1Handle,

		HAL_SDCARD_CS_PORT,
		HAL_SDCARD_CS_PIN
	);

	if (NoCardInserted == sdCardInitResult)
	{
		/* SD Card is absent */
		FMGL_ConsoleAddLine(&Console, "SD card is absent, please insert it and reboot");

		L2HAL_Error(Generic);
	}
	else if (Success != sdCardInitResult)
	{
		FMGL_ConsoleAddLine(&Console, "Failed to initialize SD card");

		L2HAL_Error(Generic); /* Failed to initialize SD-card */
	}

	/* Hardware self-test */
	FMGL_ConsoleAddLine(&Console, "Hardware self-test, it will take a while...");
	// TODO: Uncomment me
	//HAL_HardwareSelfTest();
	FMGL_ConsoleAddLine(&Console, "Success");

	/* Mounting SD card */
	FMGL_ConsoleAddLine(&Console, "Mounting SD card...");
	if (!FS_MountSDCard())
	{
		/* TODO: Offer to format SD-card*/
		FMGL_ConsoleAddLine(&Console, "FAILED");

		L2HAL_Error(Generic);
	}

	FMGL_ConsoleAddLine(&Console, "Success");

	/* Loading fonts */
	FMGL_ConsoleAddLine(&Console, "Loading font:");
	FMGL_ConsoleAddLine(&Console, CONSTANTS_PATHS_MAIN_FONT);
	FMGL_API_Font mainFontData = FMGL_LoadableFont_Init
	(
		&MainFontContext,
		CONSTANTS_PATHS_MAIN_FONT,
		&RamContext,
		(void (*)(void*, uint32_t, uint32_t, uint8_t*))&L2HAL_LY68L6400_MemoryWrite,
		(void (*)(void*, uint32_t, uint32_t, uint8_t*))&L2HAL_LY68L6400_MemoryRead,
		CONSTANTS_ADDRESSES_MAIN_FONT_BASE_ADDRESS
	);

	MainFont.Font = &mainFontData;
	MainFont.Scale = 1;
	MainFont.CharactersSpacing = 0;
	MainFont.LinesSpacing = 0;
	MainFont.FontColor = &OnColor;
	MainFont.BackgroundColor = &OffColor;
	MainFont.Transparency = &transparencyMode;

	FMGL_ConsoleAddLine(&Console, "Success");

	/* Main font ready */
	FMGL_API_ClearScreen(&FmglContext);

	/* Loading localization settings */
	FMGL_ConsoleAddLine(&Console, "Loading localization settings:");
	FMGL_ConsoleAddLine(&Console, CONSTANTS_PATHS_LOCALIZATION_CONFIG);

	LocalizationContext = LocalizatorInit(CONSTANTS_PATHS_LOCALIZATION_CONFIG);

	FMGL_ConsoleAddLine(&Console, "Success");

	/* Setup SysTick handlers */
	L2HAL_SysTick_RegisterHandler(&OnSysTick);

	/* Main loop enter */
	FMGL_API_ClearScreen(&FmglContext);
	FMGL_API_PushFramebuffer(&FmglContext);

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
		char template[32];
		uint16_t linePosition = 0;
		uint16_t width;
		uint16_t height;

		FMGL_API_ClearScreen(&FmglContext);

		/* Temperature */
		sprintf(template, "Temperature: %s%%s", LocalizatorGetLocalizedTemperaturePrecisionTemplate(&LocalizationContext));
		sprintf
		(
			buffer,
			template,
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
		sprintf(template, "Pressure: %s %%s", LocalizatorGetLocalizedPressurePrecisionTemplate(&LocalizationContext));
		sprintf
		(
			buffer,
			template,
			LocalizatorGetLocalizedPressure(&LocalizationContext, creatureReadableValues.Pressure),
			LocalizatorGetLocalizedPressureUnit(&LocalizationContext)
		);

		FMGL_API_RenderTextWithLineBreaks(&FmglContext, &MainFont, 0, linePosition, &width, &height, false, buffer);
		linePosition += height;

		FMGL_API_PushFramebuffer(&FmglContext);
	}
}

/**
 * Called every SysTick, executed in interrupt context
 */
void OnSysTick(void)
{

}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
