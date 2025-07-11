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

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"

#include "../include/l2hal.h"
#include "../include/l2hal_errors.h"
#include "../include/l2hal_systick_private.h"

void L2HAL_Init(void)
{
	/* Setting up clocks */
	L2HAL_SetupClocks();

	/* Setting up SysTick handler */
	L2HAL_SysTick_Init();

	/* Initializing custom hardware */
	L2HAL_InitCustomHardware();
}

void L2HAL_SetupClocks(void)
{
	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
	{
		L2HAL_Error(Generic);
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clock dividers */
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; /* 84MHz */
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; /* 84MHz */
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2; /* 42MHz */
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; /* 84MHz */
	if (L2HAL_RCC_ClockConfigWithSysTickHighPriority(&RCC_ClkInitStruct, FLASH_LATENCY_5, L2HAL_DEFAULT_SYSTICK_TICKS_COUNT) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}
}

uint32_t L2HAL_ConfigSysTickWithHighPriority(uint32_t ticksNumber)
{
	__disable_irq(); /* To be atomic */
	uint32_t result = HAL_SYSTICK_Config(ticksNumber);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	__enable_irq();

	return result;
}

HAL_StatusTypeDef L2HAL_RCC_ClockConfigWithSysTickHighPriority(RCC_ClkInitTypeDef* RCC_ClkInitStruct, uint32_t FLatency, uint32_t ticksNumber)
{
	__disable_irq(); /* To be atomic */
	HAL_StatusTypeDef result = HAL_RCC_ClockConfig(RCC_ClkInitStruct, FLatency);

	if (L2HAL_ConfigSysTickWithHighPriority(ticksNumber) != 0)
	{
		L2HAL_Error(Generic);
	}
	__enable_irq();

	return result;
}
