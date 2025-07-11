/*
 * global_variables.h
 *
 *  Created on: Apr 10, 2025
 *      Author: shakti
 */

#ifndef INCLUDE_GLOBAL_VARIABLES_H_
#define INCLUDE_GLOBAL_VARIABLES_H_

#include "../libs/l2hal/l2hal_config.h"

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

#endif /* INCLUDE_GLOBAL_VARIABLES_H_ */
