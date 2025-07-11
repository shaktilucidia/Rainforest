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

#ifndef L2HAL_MCU_DEPENDENT_MCUS_STM32F401CCU6_L2HAL_STM32F401CCU6_H_
#define L2HAL_MCU_DEPENDENT_MCUS_STM32F401CCU6_L2HAL_STM32F401CCU6_H_


#include <stm32f4xx_hal.h>
#include "../../../include/l2hal_errors.h"
#include "drivers/input/buttons/include/l2hal_stm32f401ccu6_buttons.h"

/**
 * Maximal ports count for given device.
 */
#define L2HAL_MAX_PORTS_COUNT 6

#pragma GCC diagnostic ignored "-Wunused-variable"
/**
 * Indexes to ports, i.e. 0 -> GPIOA, 1 -> GPIOB etc.
 */
static const GPIO_TypeDef* L2HAL_IndexesToPorts[L2HAL_MAX_PORTS_COUNT] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOH };

/**
 * Enable clock for given port
 * @param port Enable clock for given port
 */
void L2HAL_MCU_ClockPortIn(GPIO_TypeDef* port);


#endif /* L2HAL_MCU_DEPENDENT_MCUS_STM32F401CCU6_L2HAL_STM32F401CCU6_H_ */
