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

#ifndef L2HAL_DRIVERS_BLUETOOTH_HC06_INCLUDE_L2HAL_HC06_H_
#define L2HAL_DRIVERS_BLUETOOTH_HC06_INCLUDE_L2HAL_HC06_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../../../../mcu_dependent/l2hal_mcu.h"
#include "../../../../include/l2hal_aux.h"
#include "../../../../include/l2hal_errors.h"

/**
 * Timeout for blocking operations over UART
 */
#define L2HAL_HC06_UART_TIMEOUT 1000

/**
 * Maximal length of bluetooth name
 */
#define L2HAL_HC06_MAX_NAME_LENGTH 16

/**
 * Maximal memory size for bluetooth name
 */
#define L2HAL_HC06_MAX_NAME_MEMORY_SIZE (L2HAL_HC06_MAX_NAME_LENGTH + 1)

/**
 * PIN code length (4 characters + null-termination)
 */
#define L2HAL_HC06_PIN_CODE_LENGTH 5

/**
 * Driver context struct
 */
typedef struct
{
	/**
	 * True, if HC06 is connected to UART
	 */
	bool IsFound;

	/**
	 * Pointer to UART handle to what HC-06 is connected
	 */
	UART_HandleTypeDef* UART_Handle;
}
L2HAL_HC06_ContextStruct;


/**
 * Tries to attach to HC-06, connected to given UART. Issues "AT" command to test if device present or not
 */
L2HAL_HC06_ContextStruct L2HAL_HC06_AttachToDevice(UART_HandleTypeDef* uart);

/**
 * Set bluetooth name. Spaces are replaced with underscores
 */
void L2HAL_HC06_SetName(L2HAL_HC06_ContextStruct* context, const char* name);

/**
 * Set PIN code
 */
void L2HAL_HC06_SetPIN(L2HAL_HC06_ContextStruct* context, const char pin[L2HAL_HC06_PIN_CODE_LENGTH]);


#endif /* L2HAL_DRIVERS_BLUETOOTH_HC06_INCLUDE_L2HAL_HC06_H_ */
