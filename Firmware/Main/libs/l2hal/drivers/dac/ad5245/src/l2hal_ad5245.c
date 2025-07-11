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

#include "../include/l2hal_ad5245.h"


L2HAL_AD5245_ContextStruct L2HAL_AD5245_DetectDACAtAddress(I2C_HandleTypeDef* i2CHandle,
		uint8_t busAddress)
{
	L2HAL_AD5245_ContextStruct context;
	context.IsFound = false;

	if (HAL_OK == HAL_I2C_IsDeviceReady(i2CHandle, busAddress, 1, L2HAL_AD5245_BUS_TIMEOUT))
		{
			context.I2CHandle = i2CHandle;
			context.BusAddress = busAddress;
			context.IsFound = true;
		}

	return context;
}

void L2HAL_AD5245_SetValue(L2HAL_AD5245_ContextStruct* context, uint8_t value)
{
	uint8_t data[2];
	data[0] = 0x00; /* We don't want any control functions */
	data[1] = value;

	if (HAL_OK != HAL_I2C_Master_Transmit(context->I2CHandle, context->BusAddress, data, 2, L2HAL_AD5245_BUS_TIMEOUT))
	{
		L2HAL_Error(Generic);
	}
}
