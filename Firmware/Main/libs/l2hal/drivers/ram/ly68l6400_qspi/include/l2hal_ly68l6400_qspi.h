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

#ifndef L2HAL_DRIVERS_RAM_LY68L6400_QSPI_INCLUDE_L2HAL_LY68L6400_QSPI_H_
#define L2HAL_DRIVERS_RAM_LY68L6400_QSPI_INCLUDE_L2HAL_LY68L6400_QSPI_H_

#include "../../../../mcu_dependent/l2hal_mcu.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * QSPI-attached pSRAM context, QSPI connection, pins etc
 */
typedef struct
{
	/**
	 * QSPIhandle
	 */
	QSPI_HandleTypeDef* QSPIHandle;

	/**
	 * Chip select
	 */
	GPIO_TypeDef* ChipSelectPort;
	uint16_t ChipSelectPin;

	/**
	 * Clock
	 */
	GPIO_TypeDef* ClockPort;
	uint16_t ClockPin;

	/**
	 * MOSI
	 */
	GPIO_TypeDef* MosiPort;
	uint16_t MosiPin;

	/**
	 * MISO
	 */
	GPIO_TypeDef* MisoPort;
	uint16_t MisoPin;
}
L2HAL_LY68L6400_QSPI_ContextStruct;

/**
 * Init RAM chip, performs reset and checks if chip on bus (via ReadID), causes
 * L2HAL_Error() if not on bus
 */
void L2HAL_LY68L6400_QSPI_Init
(
	L2HAL_LY68L6400_QSPI_ContextStruct* context,

	QSPI_HandleTypeDef *qspiHandle,

	GPIO_TypeDef* chipSelectPort,
	uint16_t chipSelectPin,

	GPIO_TypeDef* clockPort,
	uint16_t clockPin,

	GPIO_TypeDef* mosiPort,
	uint16_t mosiPin,

	GPIO_TypeDef* misoPort,
	uint16_t misoPin
);

/**
 * Read data from memory. Size can be up to chip capacity (i.e. 8MBytes), but internally transfers will be split to L2HAL_LY68L6400_QSPI_READ_MAX_TRANSACTION_SIZE bytes transactions to avoid
 * max #CE active time violation
 */
void L2HAL_LY68L6400_QSPI_MemoryRead(L2HAL_LY68L6400_QSPI_ContextStruct *context, uint32_t startAddress, uint32_t size, uint8_t* buffer);

/**
 * Write data to memory. Size can be up to chip capacity (i.e. 8MBytes), but internally transfers will be split L2HAL_L2HAL_LY68L6400_QSPI_WRITE_MAX_TRANSACTION_SIZE 32 bytes transactions to avoid
 * max #CE active time violation
 */
void L2HAL_LY68L6400_QSPI_MemoryWrite(L2HAL_LY68L6400_QSPI_ContextStruct *context, uint32_t startAddress, uint32_t size, uint8_t* buffer);

#endif /* L2HAL_DRIVERS_RAM_LY68L6400_QSPI_INCLUDE_L2HAL_LY68L6400_QSPI_H_ */
