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
 * @brief Errors handling stuff for level 2 HAL.
 */

#ifndef L2HAL_INCLUDE_L2HAL_ERRORS_H_
#define L2HAL_INCLUDE_L2HAL_ERRORS_H_

#include "../mcu_dependent/l2hal_mcu.h"

/**
 * Clock this port on error.
 */
#define L2HAL_ERROR_CLOCK_SIGNAL_PORT() __HAL_RCC_GPIOC_CLK_ENABLE()

/**
 * In case of error pin at this port will be changed.
 */
#define L2HAL_ERROR_SIGNAL_PORT GPIOC

/**
 * Change this pin on error.
 */
#define L2HAL_ERROR_SIGNAL_PIN GPIO_PIN_13

/**
 * Do this with L2HAL_ERROR_SIGNAL_PIN of L2HAL_ERROR_SIGNAL_PORT on error.
 */
#define L2HAL_ERROR_SIGNAL_PIN_STATE GPIO_PIN_RESET


/**
 * Possible error codes. 0-32767 is for stock codes, 32768-65535 is for user-defined ones.
 */
typedef enum
{
	/**
	 * No error.
	 */
	OK = 0,

	/**
	 * Generic error.
	 */
	Generic = 1,

	/**
	 * Function was called with wrong arguments.
	 */
	WrongArgument = 2,

	/**
	 * Generally possible, but impossible just now operation, like attempt to initialize already initialized port.
	 */
	WrongOperation = 3
} L2HAL_ErrorCode;


/**
 * Call it if error happened.
 * @param code Error code.
 */
void L2HAL_Error(L2HAL_ErrorCode code);

#endif /* L2HAL_INCLUDE_L2HAL_ERRORS_H_ */
