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

#ifndef L2HAL_DRIVERS_RAM_LY68L6400_INCLUDE_L2HAL_LY68L6400_PRIVATE_H_
#define L2HAL_DRIVERS_RAM_LY68L6400_INCLUDE_L2HAL_LY68L6400_PRIVATE_H_

#include "../../../../include/l2hal_errors.h"
#include "../include/l2hal_ly68l6400.h"

/**
 * Read no more than this amount of bytes per attempt
 */
#define L2HAL_LY68L6400_MAX_READ_BYTES 32U

/**
 * Select / deselect chip
 */
void L2HAL_LY68L6400_SelectChip(L2HAL_LY68L6400_ContextStruct *context, bool isSelected);

/**
 * Writes data into chip (NOT INTO CHIP'S MEMORY)
 */
void L2HAL_LY68L6400_WriteData(L2HAL_LY68L6400_ContextStruct *context, uint8_t* data, uint16_t dataSize);

/**
 * Reads data from chip (NOT FROM CHIP'S MEMORY)
 */
void L2HAL_LY68L6400_ReadData(L2HAL_LY68L6400_ContextStruct *context, uint8_t* dataBuffer, uint16_t readSize);

/**
 * Hang till the DMA transfer completed
 */
void L2HAL_LY68L6400_WaitForDataTransferCompletion(L2HAL_LY68L6400_ContextStruct *context);

/**
 * Read up to L2HAL_LY68L6400_MAX_READ_BYTES into buffer
 */
void L2HAL_LY68L6400_MemoryReadInternal(L2HAL_LY68L6400_ContextStruct *context, uint32_t startAddress, uint8_t size, uint8_t* buffer);

/**
 * Write up to L2HAL_LY68L6400_MAX_READ_BYTES from buffer
 */
void L2HAL_LY68L6400_MemoryWriteInternal(L2HAL_LY68L6400_ContextStruct *context, uint32_t startAddress, uint8_t size, uint8_t* buffer);

#endif /* L2HAL_DRIVERS_RAM_LY68L6400_INCLUDE_L2HAL_LY68L6400_PRIVATE_H_ */
