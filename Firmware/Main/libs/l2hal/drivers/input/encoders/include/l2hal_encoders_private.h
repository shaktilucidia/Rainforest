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
 * @brief Incremental encoder level 2 HAL driver (private stuff).
 */

#ifndef L2HAL_DRIVERS_INPUT_ENCODERS_INCLUDE_L2HAL_ENCODERS_PRIVATE_H_
#define L2HAL_DRIVERS_INPUT_ENCODERS_INCLUDE_L2HAL_ENCODERS_PRIVATE_H_

#include "../include/l2hal_encoders.h"


/**
 * Driver attaches this function as callback handler to buttons, which are parts of encoder. See l2hal_buttons.h for callback parameters.
 */
void L2HAL_Encoders_ButtonsCallback(void* button, GPIO_PinState newPinState, uint16_t* portsData);



#endif /* L2HAL_DRIVERS_INPUT_ENCODERS_INCLUDE_L2HAL_ENCODERS_PRIVATE_H_ */
