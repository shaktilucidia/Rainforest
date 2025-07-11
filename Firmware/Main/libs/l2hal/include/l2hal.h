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
 * @brief Level 2 HAL main file.
 */

/**
 * How to use level 2 HAL.
 *
 * 1) Select your MCU by including one into mcu_dependent/l2hal_mcu.h
 *
 * 2) Put your custom hardware-initialization code into l2hal_custom, call functions, setting up hardware from L2HAL_InitCustomHardware()
 *
 * 3) Call L2HAL_Init() from your code before any L2HAL functions use.
 *
 */

#ifndef L2HAL_INCLUDE_L2HAL_H_
#define L2HAL_INCLUDE_L2HAL_H_

#include "l2hal_errors.h"
#include "l2hal_custom.h"
#include "l2hal_systick.h"
#include "../mcu_dependent/l2hal_mcu.h"

/**
 * Default ticks count for SysTick (will give 1KHz)
 */
#define L2HAL_DEFAULT_SYSTICK_TICKS_COUNT 84000


/**
 * Call it to setup hardware.
 */
void L2HAL_Init(void);

/**
 * Set up MCU clocks. Being called from L2HAL_Init() automatically.
 */
void L2HAL_SetupClocks(void);

/**
 * As HAL_SYSTICK_Config() function, but sets highest priority for
 * SysTick timer interrupt. Please note that SysTick timer is clocked from
 * AHB, not from core itself.
 */
uint32_t L2HAL_ConfigSysTickWithHighPriority(uint32_t ticksNumber);

/**
 * As HAL_RCC_ClockConfig(), but after configuring clocks re-setups SysTick timer with high priority
 */
HAL_StatusTypeDef L2HAL_RCC_ClockConfigWithSysTickHighPriority(RCC_ClkInitTypeDef* RCC_ClkInitStruct, uint32_t FLatency, uint32_t ticksNumber);

#endif /* L2HAL_INCLUDE_L2HAL_H_ */
