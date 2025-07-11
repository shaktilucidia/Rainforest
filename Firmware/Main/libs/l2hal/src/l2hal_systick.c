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

#include "../include/l2hal_systick.h"
#include "../include/l2hal_systick_private.h"
#include "../include/l2hal_errors.h"

void L2HAL_SysTick_Init(void)
{
	L2HAL_SysTick_Context.HandlersCount = 0;
}

void L2HAL_SysTick_RegisterHandler(void (*handler)(void))
{
	if (L2HAL_SysTick_Context.HandlersCount >= L2HAL_SYSTICK_MAX_HANDLERS)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_SysTick_Context.Handlers[L2HAL_SysTick_Context.HandlersCount] = handler;
	L2HAL_SysTick_Context.HandlersCount ++;
}

void L2HAL_SysTick_Callback(void)
{
	for (uint16_t handlerIndex = 0; handlerIndex < L2HAL_SysTick_Context.HandlersCount; handlerIndex ++)
	{
		L2HAL_SysTick_Context.Handlers[handlerIndex]();
	}
}

