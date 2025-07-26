/*
 * global_variables.h
 *
 *  Created on: Apr 10, 2025
 *      Author: shakti
 */

#ifndef INCLUDE_GLOBAL_VARIABLES_H_
#define INCLUDE_GLOBAL_VARIABLES_H_

#include "../libs/l2hal/l2hal_config.h"
#include "../libs/fatfs/ff.h"
#include "localization/localizator.h"
#include "../libs/l2hal/fmgl/console/include/console.h"
#include "bluetooth/bluetooth.h"

/* Put global variables (like contexts) here */

/**
 * SysTick driver context.
 */
L2HAL_SysTick_ContextStruct L2HAL_SysTick_Context = { 0 };

/**
 * SPI1 bus handle.
 */
SPI_HandleTypeDef SPI1Handle = { 0 };

/**
 * SPI1 TX DMA handle.
 */
DMA_HandleTypeDef SPI1TxDmaHandle = { 0 };

/**
 * SPI1 RX DMA handle.
 */
DMA_HandleTypeDef SPI1RxDmaHandle = { 0 };

/**
 * SPI2 bus handle.
 */
SPI_HandleTypeDef SPI2Handle = { 0 };

/**
 * SPI2 TX DMA handle.
 */
DMA_HandleTypeDef SPI2TxDmaHandle = { 0 };

/**
 * SPI2 RX DMA handle.
 */
DMA_HandleTypeDef SPI2RxDmaHandle = { 0 };

/**
 * pSRAM context
 */
L2HAL_LY68L6400_ContextStruct RamContext;

/**
 *  Display context
 */
L2HAL_SSD1683_ContextStruct DisplayContext;

/**
 * FMGL context.
 */
FMGL_API_DriverContext FmglContext;

/**
 * SD-card context
 */
L2HAL_SDCard_ContextStruct SDCardContext;

/**
 * SD card filesystem pointer
 */
FATFS* SDCardFsPtr = NULL;

/**
 * Main font context
 */
FMGL_LoadableFont_ContextStruct MainFontContext;

/**
 * Monospaced early-stage font
 */
FMGL_API_FontSettings EarlyFont;

/**
 * Main font
 */
FMGL_API_FontSettings MainFont;

/**
 * I2C1 bus handle
 */
I2C_HandleTypeDef I2C1_Handle = { 0 };

/**
 * Local sensor
 */
L2HAL_BME280_I2C_ContextStruct LocalSensor;

/**
 * Localization context
 */
LocalizationContextStruct LocalizationContext;

/**
 * Console
 */
FMGL_Console_ContextStruct Console;

/**
 * UART1 Handle
 */
UART_HandleTypeDef UART1Handle;

/**
 * Bluetooth module (HC-06) context
 */
L2HAL_HC06_ContextStruct BluetoothModuleContext;

/**
 * Bluetooth context
 */
BluetoothContextStruct BluetoothContext;

/**
 * CRC calculator context
 */
L2HAL_CRCContextStruct CrcContext;

#endif /* INCLUDE_GLOBAL_VARIABLES_H_ */
