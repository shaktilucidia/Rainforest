/*
	This file is part of Shakti Lucidia's STM32 level 2 HAL.

	STM32 level 2 HAL is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	-------------------------------------------------------------------------

	Created by Shakti Lucidia

	Feel free to contact: shakti_lucidia@proton.me

	Repository: https://github.com/shaktilucidia/stm32-l2hal

	-------------------------------------------------------------------------
 */

#ifndef L2HAL_DRIVERS_SDCARD_INCLUDE_L2HAL_SDCARD_H_
#define L2HAL_DRIVERS_SDCARD_INCLUDE_L2HAL_SDCARD_H_

#include <stdbool.h>
#include "../../../mcu_dependent/l2hal_mcu.h"

/**
 * SD-card block size in bytes
 */
#define L2HAL_SDCARD_BLOCK_SIZE 512

/**
 * SD-card initialization result
 */
enum L2HAL_SDCard_InitResult
{
	Success, /* Successfully initialized */
	NoCardInserted, /* Card not inserted */
	Error, /* Various generic initialization errors */
	UnsupportedCard /* Unsupported card (version or voltage) */
};

/**
 * Display context, SPI connection and various stuff is stored here
 */
typedef struct
{
	/**
	 * SPI bus handle
	 */
	SPI_HandleTypeDef* SPIHandle;

	/**
	 * Chip select pin
	 */
	GPIO_TypeDef* ChipSelectPort;
	uint16_t ChipSelectPin;

	/**
	 * If true, then data transfer in progress and we must wait for next one
	 */
	volatile bool IsDataTransferInProgress;
}
L2HAL_SDCard_ContextStruct;

/**
 * Initialize SD-card
 */
enum L2HAL_SDCard_InitResult L2HAL_SDCard_Init
(
	L2HAL_SDCard_ContextStruct* context,
	SPI_HandleTypeDef *spiHandle,

	GPIO_TypeDef* chipSelectPort,
	uint16_t chipSelectPin
);

/**
 * Read SD-card size in 512-bytes blocks
 * @param context SD-card context
 * @return Number of blocks
 */
uint32_t L2HAL_SDCard_ReadBlocksCount(L2HAL_SDCard_ContextStruct* context);

/**
 * Read given block to buffer. Buffer must be L2HAL_SDCARD_BLOCK_SIZE-bytes long array
 * @param context SD-card context
 * @param blockNumber Block number
 * @param buffer Buffer to put data in
 */
void L2HAL_SDCard_ReadSingleBlock(L2HAL_SDCard_ContextStruct* context, uint32_t blockNumber, uint8_t* buffer);

/**
 * Write from buffer to given block. Buffer must be L2HAL_SDCARD_BLOCK_SIZE-bytes long array
 * @param context SD-card context
 * @param blockNumber Block number
 * @param buffer Buffer with data to write
 */
void L2HAL_SDCard_WriteSingleBlock(L2HAL_SDCard_ContextStruct* context, uint32_t blockNumber, uint8_t* buffer);

/**
 * Call it from BOTH SPI DMA TX and SPI DMA RX completion interrupts
 * @param context SD-card context
 */
void L2HAL_SDCard_MarkDataTransferAsCompleted(L2HAL_SDCard_ContextStruct *context);

#endif /* L2HAL_DRIVERS_SDCARD_INCLUDE_L2HAL_SDCARD_H_ */
