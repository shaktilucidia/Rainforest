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
 * @brief MCU-specific stuff for STM32F103C8T6.
*/

#ifndef L2HAL_INCLUDE_DEVICE_DEPENDENT_DEVICES_L2HAL_STM32F103C8T6_H_
#define L2HAL_INCLUDE_DEVICE_DEPENDENT_DEVICES_L2HAL_STM32F103C8T6_H_

#include <stm32f1xx_hal.h>
#include "drivers/input/buttons/include/l2hal_stm32f103c8t6_buttons.h"

/* Maximal ports count for given device. A-D for STM32F103C8T6 */
#define L2HAL_MAX_PORTS_COUNT 4

#pragma GCC diagnostic ignored "-Wunused-variable"

 /* Indexes to ports, i.e. 0 -> GPIOA, 1 -> GPIOB etc. */
static const GPIO_TypeDef* L2HAL_IndexesToPorts[L2HAL_MAX_PORTS_COUNT] = { GPIOA, GPIOB, GPIOC, GPIOD };


#endif  L2HAL_INCLUDE_DEVICE_DEPENDENT_DEVICES_L2HAL_STM32F103C8T6_H_
