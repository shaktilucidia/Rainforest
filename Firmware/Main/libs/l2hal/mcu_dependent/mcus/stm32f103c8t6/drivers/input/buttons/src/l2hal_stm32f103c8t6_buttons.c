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


#include "../include/l2hal_stm32f103c8t6_buttons.h"
#include "../../../../../../../include/l2hal_errors.h"


void L2HAL_Buttons_DefaultPinInitializer(GPIO_TypeDef* port, uint32_t pins)
{
	/* Clocking port in */
	if (GPIOA == port)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}
	else if (GPIOB == port)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();
	}
	else if (GPIOC == port)
	{
		__HAL_RCC_GPIOC_CLK_ENABLE();
	}
	else if (GPIOD == port)
	{
		__HAL_RCC_GPIOD_CLK_ENABLE();
	}
	else
	{
		 /* Non-existing port */
		L2HAL_Error(WrongArgument);
	}


	/* Initializing pin */
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode = L2HAL_BUTTONS_DEFAULT_PIN_MODE;
	GPIO_InitStruct.Pull = L2HAL_BUTTONS_DEFAULT_PIN_PULL_MODE;

	GPIO_InitStruct.Pin = pins;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}
