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

/**
 * @file
 * @brief Level 2 HAL SysTick handler and related functions.
 *
 * Usage:
 * 1) Add call of L2HAL_SysTick_Callback() (defined in l2hal_systick_private.h) to SysTick_Handler(). You could find SysTick_Handler()
 * in file like stm32f1xx_it.c
 *
 * 2) Add handlers calling L2HAL_SysTick_RegisterHandler(). Added handler will be called on next SysTick:
 *
 * 	L2HAL_SysTick_RegisterHandler(&SysTickHandler);
 *
 * 	3) Remember, that handlers are executed within interrupt context, so handlers have to exit as soon as possible.
 */

#ifndef L2HAL_INCLUDE_L2HAL_SYSTICK_H_
#define L2HAL_INCLUDE_L2HAL_SYSTICK_H_

#include <stdint.h>

/**
 * Maximal possible count of SysTick handlers
 */
#define L2HAL_SYSTICK_MAX_HANDLERS 16U

/**
 * Context, associated with SysTick driver.
 */
typedef struct
{
	/**
	 * Amount of registered SysTick handlers.
	 */
	uint16_t HandlersCount;

	/**
	 * Array of pointers to registered SysTick handlers.
	 */
	void (*Handlers[L2HAL_SYSTICK_MAX_HANDLERS])(void);

} L2HAL_SysTick_ContextStruct;

extern L2HAL_SysTick_ContextStruct L2HAL_SysTick_Context;

/**
 * Call it from SysTick_Handler().
 */
void L2HAL_SysTick_Callback(void);

/**
 * Registers new SysTick handler. After registration handler will be called each SysTick.
 * @param hanlder Pointer to handler.
 */
void L2HAL_SysTick_RegisterHandler(void (*handler)(void));

#endif /* L2HAL_INCLUDE_L2HAL_SYSTICK_H_ */
