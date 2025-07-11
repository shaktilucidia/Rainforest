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

#ifndef L2HAL_DRIVERS_INTERNAL_CRC_INCLUDE_L2HAL_CRC_H_
#define L2HAL_DRIVERS_INTERNAL_CRC_INCLUDE_L2HAL_CRC_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../../../mcu_dependent/l2hal_mcu.h"

/**
 * Maximal buffer size
 */
#define L2HAL_CRC_MAX_BUFFER_SIZE 1024

/**
 * CRC context
 */
typedef struct
{
	/**
	 * CRC Handle
	 */
	CRC_HandleTypeDef Handle;
}
L2HAL_CRCContextStruct;

/**
 * Initialize CRC calculator and return context.
 */
L2HAL_CRCContextStruct L2HAL_CRC_Init(void);

/**
 * Calculates CRC for the buffer of arbitrary size in bytes (i.e. alignment by 32bits is not necessary).
 * If buffer is aligned, then just calculate CRC, otherwise copies data to temporary aligned buffer and
 * calculate CRC over it.
 *
 * @param context Pointer to context
 * @param buffer Pointer to data buffer
 * @param size Data buffer size in BYTES
 * @return Calculated CRC
 */
uint32_t L2HAL_CRC_Calculate(L2HAL_CRCContextStruct* context, uint8_t* buffer, uint32_t size);

#endif /* L2HAL_DRIVERS_INTERNAL_CRC_INCLUDE_L2HAL_CRC_H_ */
