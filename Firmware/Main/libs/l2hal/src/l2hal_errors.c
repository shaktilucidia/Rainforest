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

#include "../include/l2hal_errors.h"

void L2HAL_Error(L2HAL_ErrorCode code)
{
	L2HAL_ERROR_CLOCK_SIGNAL_PORT();

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = L2HAL_ERROR_SIGNAL_PIN;
	HAL_GPIO_Init(L2HAL_ERROR_SIGNAL_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(L2HAL_ERROR_SIGNAL_PORT, L2HAL_ERROR_SIGNAL_PIN, L2HAL_ERROR_SIGNAL_PIN_STATE);

	/* Halt */
	while(1) {}
}
