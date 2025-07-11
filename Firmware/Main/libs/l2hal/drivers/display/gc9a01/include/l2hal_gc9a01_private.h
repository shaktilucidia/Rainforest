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

#ifndef L2HAL_DRIVERS_DISPLAY_GC9A01_INCLUDE_L2HAL_GC9A01_PRIVATE_H_
#define L2HAL_DRIVERS_DISPLAY_GC9A01_INCLUDE_L2HAL_GC9A01_PRIVATE_H_

#include "../../../../mcu_dependent/l2hal_mcu.h"
#include <stdint.h>

/**
 * Interval between CS and RESET signals during display reset
 */
#define L2HAL_GC9A01_CS_TO_RESET 5

/**
 * Reset signal duration
 */
#define L2HAL_GC9A01_RESET_DURATION 10


/**
 * Wait this time after reset before starting to send commands
 */
#define L2HAL_GC9A01_DISPLAY_BOOT_TIME 120

/**
 * Wait this time after exiting from sleepmode
 */
#define L2HAL_GC9A01_DISPLAY_SLEEP_OUT_TIME 120

/**
 * Wait this time after issuing "Display ON" command
 */
#define L2HAL_GC9A01_DISPLAY_AFTER_ON_TIME 20

/**
 * Select / deselect chip
 */
void L2HAL_GC9A01_SelectChip(L2HAL_GC9A01_ContextStruct *context, bool isSelected);

/**
 * Send reset signal to display
 */
void L2HAL_GC9A01_ResetDisplay(L2HAL_GC9A01_ContextStruct *context);

/**
 * Write command to display
 */
void L2HAL_GC9A01_WriteCommand(L2HAL_GC9A01_ContextStruct *context, uint8_t command);

/**
 * Write data to display
 */
void L2HAL_GC9A01_WriteData(L2HAL_GC9A01_ContextStruct *context, uint8_t *data, uint16_t dataSize);

/**
 * Special case - send one byte of data to display (useful for initialization)
 */
void L2HAL_GC9A01_WriteByteData(L2HAL_GC9A01_ContextStruct *context, uint8_t byteData);

/**
 * Returns true if pixel with given coordinates exists in cache
 */
bool L2HAL_GC9A01_IsPixelsCacheHit(L2HAL_GC9A01_ContextStruct *context, uint16_t x, uint16_t y);

/**
 * Read pixel from pixels cache. To speed up don't do any checks, so call with pixel outside of cache will lead to
 * unexpected behaviour.
 */
FMGL_API_ColorStruct L2HAL_GC9A01_ReadPixelsCache(L2HAL_GC9A01_ContextStruct *context, uint16_t x, uint16_t y);

/**
 * Write pixel to pixels cache. To speed up don't do any checks, so call with pixel outside of cache will lead to
 * MEMORY CORRUPTION.
 */
void L2HAL_GC9A01_WritePixelsCache(L2HAL_GC9A01_ContextStruct *context, uint16_t x, uint16_t y, FMGL_API_ColorStruct color);

/**
 * Write cache to framebuffer
 */
void L2HAL_GC9A01_WriteCacheToFramebuffer(L2HAL_GC9A01_ContextStruct *context);

/**
 * Read cache from framebuffer
 */
void L2HAL_GC9A01_ReadCacheFromFramebuffer(L2HAL_GC9A01_ContextStruct *context);

/**
 * Forced framebuffer push (without checking for dirty pixels)
 */
void L2HAL_GC9A01_ForcePushFramebuffer(L2HAL_GC9A01_ContextStruct* context);

/**
 * Hang till the DMA transfer completed
 */
void L2HAL_GC9A01_WaitForDataTransferCompletion(L2HAL_GC9A01_ContextStruct *context);

#endif /* L2HAL_DRIVERS_DISPLAY_GC9A01_INCLUDE_L2HAL_GC9A01_PRIVATE_H_ */
