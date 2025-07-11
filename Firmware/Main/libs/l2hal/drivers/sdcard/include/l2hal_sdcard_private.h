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

#ifndef L2HAL_DRIVERS_SDCARD_INCLUDE_L2HAL_SDCARD_PRIVATE_H_
#define L2HAL_DRIVERS_SDCARD_INCLUDE_L2HAL_SDCARD_PRIVATE_H_

#include "../../../mcu_dependent/l2hal_mcu.h"
#include "../include/l2hal_sdcard.h"
#include <stdbool.h>

/* Data tokens */
#define L2HAL_SDCARD_DATA_TOKEN_CMD9  0xFE
#define L2HAL_SDCARD_DATA_TOKEN_CMD17 0xFE
#define L2HAL_SDCARD_DATA_TOKEN_CMD18 0xFE
#define L2HAL_SDCARD_DATA_TOKEN_CMD24 0xFE
#define L2HAL_SDCARD_DATA_TOKEN_CMD25 0xFC

/**
 * Select / deselect sdcard
 */
void L2HAL_SDCard_Select(L2HAL_SDCard_ContextStruct *context, bool isSelected);


/**
 * Send data to SD-card WITHOUT control of CS
 * @param context SD-card context
 * @param data Data to send
 * @param dataSize Data size
 */
void L2HAL_SDCard_WriteDataNoCSControl(L2HAL_SDCard_ContextStruct *context, uint8_t *data, uint16_t dataSize);


/**
 * Read data from SD-card into buffer
 * @param context SD-card context
 * @param buffer Data will be read here
 * @param readSize How much data to read
 */
void L2HAL_SDCard_ReadData(L2HAL_SDCard_ContextStruct *context, uint8_t *buffer, uint16_t readSize);

/**
 * Wait for BUSY flag cleared
 * @param context SD-card context
 */
void L2HAL_SDCard_WaitForBusyCleared(L2HAL_SDCard_ContextStruct *context);


/**
 * Read SD-card R1 response
 * @param context SD-card context
 * @param response R1 data
 * @return Is successful
 */
bool L2HAL_SDCard_ReadR1(L2HAL_SDCard_ContextStruct *context, uint8_t* response);

/**
 * Wait for given token
 * @param context SD-card context
 * @param token Token to wait for
 */
void L2HAL_SDCard_WaitForToken(L2HAL_SDCard_ContextStruct *context, uint8_t token);

/**
 * Wait for DMA transfer completion
 * @param context SD-card context
 */
void L2HAL_SDCard_WaitForDataTransferCompletion(L2HAL_SDCard_ContextStruct *context);

#endif /* L2HAL_DRIVERS_SDCARD_INCLUDE_L2HAL_SDCARD_PRIVATE_H_ */
