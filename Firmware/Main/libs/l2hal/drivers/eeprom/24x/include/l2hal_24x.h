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
 * @brief 24x EEPROMs level 2 HAL driver.
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
 * 2) Define EEPROM context (if you have many EEPROMS you need, of course, many contexts):
 *    L2HAL_24x_ContextStruct EEPROMContext
 *
 * 3) Attach driver to EEPROM:
 *    EEPROMContext = L2HAL_24x_DetectEepromAtAddress(&I2C_Other, 0xA0, true, 64);
 *
 *    Don't forget to check for IsFound flag in context. If it's false, then driver was unable to find EEPROM at given bus address.
 *
 * 4) Write:
 *    L2HAL_24x_WriteData(&EEPROMContext, 15, writeBuffer, 100);
 *
 * 5) Read:
 *    L2HAL_24x_ReadData(&EEPROMContext, 0x00, eepromBuffer, 128);
 */

#ifndef L2HAL_DRIVERS_EEPROM_24X_INCLUDE_L2HAL_24X_H_
#define L2HAL_DRIVERS_EEPROM_24X_INCLUDE_L2HAL_24X_H_

#include <stdint.h>
#include <stdbool.h>
#include "../../../../mcu_dependent/l2hal_mcu.h"
#include "../../../../include/l2hal_aux.h"
#include "../../../../include/l2hal_errors.h"

/**
 * Bus timeout for read and EEPROM detection, also for single byte write
 */
#define L2HAL_24X_BUS_TIMEOUT 100

/**
 * Wait this time between two writes
 */
#define L2HAL_24X_INTERWRITE_PAUSE 5


/**
 * 24x EEPROM context
 */
typedef struct
{
	/**
	 * True, if EEPROM is found on bus
	 */
	bool IsFound;

	/**
	 * Pointer to I2C Handle
	 */
	I2C_HandleTypeDef* I2CHandle;

	/**
	 * EEPROM's address on bus
	 */
	uint8_t BusAddress;

	/**
	 * EEPROM address size
	 */
	uint16_t MemAddressSize;

	/**
	 * Write no more than this amount bytes at once
	 */
	uint16_t WritePageSize;
}
L2HAL_24x_ContextStruct;

/**
 * Attempts to detect EEPROM at given bus and address.
 * @param i2CHandle Pointer to I2C bus handle
 * @param busAddress Search for EEPROM at this I2C bus address
 * @param isLongAddress Set to true if EEPROM uses more than 8-bit internal address
 * @param writePageSize Write no more than this amount of bytes at once
 */
L2HAL_24x_ContextStruct L2HAL_24x_DetectEepromAtAddress(I2C_HandleTypeDef* i2CHandle,
		uint8_t busAddress,
		bool isLongAddress,
		uint16_t writePageSize);

/**
 * Read data from EEPROM into given destination.
 * @param context Pointer to driver context
 * @param eepromAddress Read data starting from this EEPROM address
 * @param destination Put read data here
 * @param size How much bytes to read
 */
void L2HAL_24x_ReadData(L2HAL_24x_ContextStruct* context, uint16_t eepromAddress, uint8_t* destination, uint16_t size);

/**
 * Write data from source into EEPROM. Writes byte-by-byte, slow but no problems with pages
 * @param context Pointer to driver context
 * @param eepromAddress Write data starting from this EEPROM address
 * @param destination Get data to write from here
 * @param size How much bytes to write
 */
void L2HAL_24x_WriteData(L2HAL_24x_ContextStruct* context, uint16_t eepromAddress, uint8_t* source, uint16_t size);


/*************************
* Private stuff is below *
**************************/

/**
 * Determines page number by EEPROM address
 * @param context Pointer to driver context
 * @param eepromAddress EEPROM address
 * @return Page number
 */
uint16_t L2HAL_24x_GetPageNumber(L2HAL_24x_ContextStruct* context, uint16_t eepromAddress);

/**
 * Returns address of page's first byte
 * @param context Pointer to driver context
 * @param pageNumber Page number
 * @return Page's first byte EEPROM address
 */
uint16_t L2HAL_24x_GetPageStartAddress(L2HAL_24x_ContextStruct* context, uint16_t pageNumber);

/**
 * Returns address of page's last byte
 * @param context Pointer to driver context
 * @param pageNumber Page number
 * @return Page's last byte EEPROM address
 */
uint16_t L2HAL_24x_GetPageEndAddress(L2HAL_24x_ContextStruct* context, uint16_t pageNumber);

/**
 * Writes data within one page, causing L2HAL_Error(Generic) if interpage write going to occur.
 * @param context Pointer to driver context
 * @param eepromAddress EEPROM address of first byte, pointed by dataPtr
 * @param dataPtr Pointer to first byte of data
 * @param size Amount of data to write
 */
void L2HAL_24x_PageWrite(L2HAL_24x_ContextStruct* context, uint16_t eepromAddress, uint8_t* dataPtr, uint16_t size);

#endif /* L2HAL_DRIVERS_EEPROM_24X_INCLUDE_L2HAL_24X_H_ */
