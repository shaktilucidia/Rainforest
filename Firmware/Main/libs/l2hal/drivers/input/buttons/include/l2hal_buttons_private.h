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
 * @brief Polling buttons level 2 HAL driver (private stuff).
 */

#ifndef L2HAL_DRIVERS_INPUT_BUTTONS_INCLUDE_L2HAL_BUTTONS_PRIVATE_H_
#define L2HAL_DRIVERS_INPUT_BUTTONS_INCLUDE_L2HAL_BUTTONS_PRIVATE_H_

#include "../../../../include/l2hal_aux.h"
#include "../include/l2hal_buttons.h"
#include "../../../../include/l2hal_systick.h"


/**
 * Mask data, read from IDR with this mask.
 */
#define L2HAL_BUTTONS_PORT_DATA_MASK 0xFFFFU

/**
 * Metacontext -united context for all button driver instances.
 */
typedef struct
{
	/**
	 * False until first driver initialization, true after it.
	 */
	bool IsInitialized;

	/**
	 * How many instances of button driver we have.
	 */
	uint8_t ContextsCount;

	/**
	 * Array of pointers to contexts.
	 */
	L2HAL_Buttons_ContextStruct** Contexts;

} L2HAL_Buttons_MetacontextStruct;

/**
 * Driver metacontext.
 */
L2HAL_Buttons_MetacontextStruct L2HAL_Buttons_Metacontext = { false, 0, NULL };



/**
 * Loads data from physical ports into context, blocking reading data during update.
 */
void L2HAL_Buttons_GetPortsData(L2HAL_Buttons_ContextStruct* context);

/**
 * Returns port index (see L2HAL_Buttons_IndexesToPorts to reverse conversion).
 */
uint8_t L2HAL_Buttons_GetPortIndex(GPIO_TypeDef* port);

/**
 * SysTick handler, registered during driver first instance initialization.
 */
void L2HAL_Buttons_SysTickHandler(void);


#endif /* L2HAL_DRIVERS_INPUT_BUTTONS_INCLUDE_L2HAL_BUTTONS_PRIVATE_H_ */
