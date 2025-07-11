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

#include "../include/l2hal_crc.h"
#include "../../../../include/l2hal_errors.h"

L2HAL_CRCContextStruct L2HAL_CRC_Init(void)
{
	L2HAL_CRCContextStruct context = { 0 };
	context.Handle.Instance = CRC;

	if (HAL_CRC_Init(&context.Handle) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}

	return context;
}

uint32_t L2HAL_CRC_Calculate(L2HAL_CRCContextStruct* context, uint8_t* buffer, uint32_t size)
{
	if (size > L2HAL_CRC_MAX_BUFFER_SIZE)
	{
		L2HAL_Error(Generic);
	}

	if (0 == size % 4)
	{
		// Buffer is aligned, just calculating
		volatile uint32_t result = HAL_CRC_Calculate(&context->Handle, (uint32_t*)buffer, size / 4);
		return result;
	}
	else
	{
		uint32_t sizeIn32 = size / 4 + 1;
		uint32_t sizeIn8 = sizeIn32 * 4;

		uint8_t alignedBuffer[L2HAL_CRC_MAX_BUFFER_SIZE];
		memset(alignedBuffer, 0x00, sizeIn8);
		memcpy(alignedBuffer, buffer, size);

		volatile uint32_t result = HAL_CRC_Calculate(&context->Handle, (uint32_t*)alignedBuffer, sizeIn32);

		return result;
	}
}
