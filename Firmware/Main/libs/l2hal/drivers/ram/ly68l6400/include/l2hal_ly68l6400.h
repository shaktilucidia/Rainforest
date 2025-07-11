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

#ifndef L2HAL_DRIVERS_RAM_LY68L6400_INCLUDE_L2HAL_LY68L6400_H_
#define L2HAL_DRIVERS_RAM_LY68L6400_INCLUDE_L2HAL_LY68L6400_H_

#include "../../../../mcu_dependent/l2hal_mcu.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * Chip capacity in bytes
 */
#define L2HAL_LY68L6400_CAPACITY 8388608U

/**
 * SPI-attached pSRAM context, SPI connection, pins etc
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
L2HAL_LY68L6400_ContextStruct;

/**
 * Init RAM chip, performs reset and checks if chip on bus (via ReadID), causes
 * L2HAL_Error() if not on bus
 */
void L2HAL_LY68L6400_Init
(
	L2HAL_LY68L6400_ContextStruct* context,
	SPI_HandleTypeDef *spiHandle,

	GPIO_TypeDef* chipSelectPort,
	uint16_t chipSelectPin
);

/**
 * Call it from BOTH SPI DMA TX and SPI DMA RX completion interrupts
 * @param context SD-card context
 */
void L2HAL_LY68L6400_MarkDataTransferAsCompleted(L2HAL_LY68L6400_ContextStruct *context);

/**
 * Read data from memory. Size can be up to chip capacity (i.e. 8MBytes), but internally transfers will be split to 32 bytes transactions to avoid
 * max #CE active time violation
 */
void L2HAL_LY68L6400_MemoryRead(L2HAL_LY68L6400_ContextStruct *context, uint32_t startAddress, uint32_t size, uint8_t* buffer);

/**
 * Write data to memory. Size can be up to chip capacity (i.e. 8MBytes), but internally transfers will be split to 32 bytes transactions to avoid
 * max #CE active time violation
 */
void L2HAL_LY68L6400_MemoryWrite(L2HAL_LY68L6400_ContextStruct *context, uint32_t startAddress, uint32_t size, uint8_t* buffer);

#endif /* L2HAL_DRIVERS_RAM_LY68L6400_INCLUDE_L2HAL_LY68L6400_H_ */
