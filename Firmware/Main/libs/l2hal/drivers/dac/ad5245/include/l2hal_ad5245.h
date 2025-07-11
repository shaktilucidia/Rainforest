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
 * @brief AD5245 digital potentiometer level 2 HAL driver.
 * How to use this driver:
 *
 * 1) Set-up I2C using STM32 HAL.
 *    Sample code:
 *
 *    void L2HAL_SetupI2C(void)
 *    {
 *        // Clocking I2C
 *        __HAL_RCC_I2C1_CLK_ENABLE();
 *
 *        // Setting up I2C
 *        I2CHandle.Instance = I2C1;
 *        I2CHandle.Init.ClockSpeed = 400000; // 400 KHz
 *        I2CHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
 *        I2CHandle.Init.OwnAddress1 = 0x00;
 *        I2CHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
 *        I2CHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
 *        I2CHandle.Init.OwnAddress2 = 0x00;
 *        I2CHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
 *        I2CHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
 *
 *        if(HAL_I2C_Init(&I2CHandle) != HAL_OK)
 *        {
 *            // Initialization Error
 *            L2HAL_Error(L2HAL_ERROR_GENERIC);
 *        }
 *    }
 *
 * 2) Define D-POT context (if you have many potentiometers you need, of course, many contexts):
 *    L2HAL_AD5245_ContextStruct U80mRegulatorContext;
 *
 * 3) Attach driver to D-POT:
 *    U80mRegulatorContext = L2HAL_AD5245_DetectDACAtAddress(&I2C_Other, ox58);
 *
 *    Don't forget to check for IsFound flag in context. If it's false, then driver was unable to find D-POT at given bus address.
 *
 * 4) Write value:
 *    L2HAL_AD5245_SetValue(&U80mRegulatorContext, 0x80);
 */

#ifndef L2HAL_DRIVERS_DAC_AD5245_INCLUDE_L2HAL_AD5245_H_
#define L2HAL_DRIVERS_DAC_AD5245_INCLUDE_L2HAL_AD5245_H_

#include <stdbool.h>
#include <stdint.h>
#include "../../../../mcu_dependent/l2hal_mcu.h"
#include "../../../../include/l2hal_aux.h"
#include "../../../../include/l2hal_errors.h"

/**
 * IIC bus timeout for device operations
 */
#define L2HAL_AD5245_BUS_TIMEOUT 200U

/**
 * AD5245 context
 */
typedef struct
{
	/**
	 * True, if DAC is found on bus
	 */
	bool IsFound;

	/**
	 * Pointer to I2C Handle
	 */
	I2C_HandleTypeDef* I2CHandle;

	/**
	 * DAC's address on bus
	 */
	uint8_t BusAddress;
}
L2HAL_AD5245_ContextStruct;

/**
 * Attempts to detect DAC at given bus and address.
 * @param i2CHandle Pointer to I2C bus handle
 * @param busAddress Search for DAC at this I2C bus address
 */
L2HAL_AD5245_ContextStruct L2HAL_AD5245_DetectDACAtAddress(I2C_HandleTypeDef* i2CHandle,
		uint8_t busAddress);

/**
 * Sets DAC value.
 * @param context Pointer to device context
 * @param value Value to set
 */
void L2HAL_AD5245_SetValue(L2HAL_AD5245_ContextStruct* context, uint8_t value);

#endif /* L2HAL_DRIVERS_DAC_AD5245_INCLUDE_L2HAL_AD5245_H_ */
