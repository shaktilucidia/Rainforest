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

#ifndef L2HAL_DRIVERS_DISPLAY_GC9A01_LOCAL_FRAMEBUFFER_INCLUDE_L2HAL_GC9A01_LFB_LFB_H_
#define L2HAL_DRIVERS_DISPLAY_GC9A01_LOCAL_FRAMEBUFFER_INCLUDE_L2HAL_GC9A01_LFB_LFB_H_

#include <stdbool.h>
#include "../../../../fmgl/include/fmgl.h"
#include "../../../../mcu_dependent/l2hal_mcu.h"

/**
 * Display sizes
 */
#define L2HAL_GC9A01_LFB_DISPLAY_WIDTH 240
#define L2HAL_GC9A01_LFB_DISPLAY_HEIGHT 240

#define L2HAL_GC9A01_LFB_DISPLAY_LINE_SIZE (L2HAL_GC9A01_LFB_DISPLAY_WIDTH * 3)
#define L2HAL_GC9A01_LFB_FRAMEBUFFER_SIZE (L2HAL_GC9A01_LFB_DISPLAY_LINE_SIZE * L2HAL_GC9A01_LFB_DISPLAY_HEIGHT)

/**
 * Dirty lines buffer sizes (in RAM bytes)
 */
#define L2HAL_GC9A01_LFB_DIRTY_LINES_BUFFER_SIZE (L2HAL_GC9A01_LFB_DISPLAY_HEIGHT / 8) /* 8 bits */

enum L2HAL_GC9A01_LFB_Orientation
{
	L2HAL_GC9A01_LFB_ROTATION_0,  /**< ROTATION_0 */  /**< L2HAL_GC9A01_LFB_ROTATION_0 */
	L2HAL_GC9A01_LFB_ROTATION_90, /**< ROTATION_90 */ /**< L2HAL_GC9A01_LFB_ROTATION_90 */
	L2HAL_GC9A01_LFB_ROTATION_180,/**< ROTATION_180 *//**< L2HAL_GC9A01_LFB_ROTATION_180 */
	L2HAL_GC9A01_LFB_ROTATION_270 /**< ROTATION_270 *//**< L2HAL_GC9A01_LFB_ROTATION_270 */
};


/**
 * Display context, SPI connection and various stuff is stored here
 */
typedef struct
{
	/**
	 * SPI bus handle
	 */
	SPI_HandleTypeDef* SPIHandle;

	/**
	 * Reset pin
	 */
	GPIO_TypeDef* ResetPort;
	uint16_t ResetPin;

	/**
	 * Data / command pin
	 */
	GPIO_TypeDef* DataCommandPort;
	uint16_t DataCommandPin;

	/**
	 * Chip select pin
	 */
	GPIO_TypeDef* ChipSelectPort;
	uint16_t ChipSelectPin;

	/**
	 * Current drawing color
	 */
	FMGL_API_ColorStruct ActiveColor;

	/**
	 * Framebuffer
	 */
	uint8_t Framebuffer[L2HAL_GC9A01_LFB_FRAMEBUFFER_SIZE];

	/**
	 * Dirty lines buffer
	 */
	uint8_t DirtyLinesBuffer[L2HAL_GC9A01_LFB_DIRTY_LINES_BUFFER_SIZE];

	/**
	 * If true, then data transfer in progress and we must wait for next one
	 */
	volatile bool IsDataTransferInProgress;
}
L2HAL_GC9A01_LFB_ContextStruct;


/**
 * Initialize GC9A01-based display
 */
void L2HAL_GC9A01_LFB_Init
(
	L2HAL_GC9A01_LFB_ContextStruct* context,
	SPI_HandleTypeDef *spiHandle,

	GPIO_TypeDef* resetPort,
	uint16_t resetPin,

	GPIO_TypeDef* dataCommandPort,
	uint16_t dataCommandPin,

	GPIO_TypeDef* chipSelectPort,
	uint16_t chipSelectPin,

	enum L2HAL_GC9A01_LFB_Orientation orientation
);


/**
 * Set columns range for subsequent memory writes
 */
void L2HAL_GC9A01_LFB_SetColumnsRange(L2HAL_GC9A01_LFB_ContextStruct *context, uint16_t colStart, uint16_t colEnd);

/**
 * Set rows range for subsequent memory writes
 */
void L2HAL_GC9A01_LFB_SetRowsRange(L2HAL_GC9A01_LFB_ContextStruct *context, uint16_t rowStart, uint16_t rowEnd);

/**
 * Get display width
 */
uint16_t L2HAL_GC9A01_LFB_GetWidth(void);

/**
 * Get display height
 */
uint16_t L2HAL_GC9A01_LFB_GetHeight(void);

/**
 * Fill display with active color
 */
void L2HAL_GC9A01_LFB_ClearDisplay(L2HAL_GC9A01_LFB_ContextStruct *context);

/**
 * Draw pixel
 */
void L2HAL_GC9A01_LFB_DrawPixel(void* context, uint16_t x, uint16_t y);

/**
 * Get pixel color
 */
FMGL_API_ColorStruct L2HAL_GC9A01_LFB_GetPixel(void* context, uint16_t x, uint16_t y);

/**
 * Set color what will be used for drawing.
 */
void L2HAL_GC9A01_LFB_SetActiveColor(void* context, FMGL_API_ColorStruct color);

/**
 * Push framebuffer to display
 */
void L2HAL_GC9A01_LFB_PushFramebuffer(void* context);

/**
 * Fill framebuffer with black color
 */
void L2HAL_GC9A01_LFB_ClearFramebuffer(L2HAL_GC9A01_LFB_ContextStruct* context);

/**
 * Call it from SPI DMA TX completion interrupt
 */
void L2HAL_GC9A01_LFB_MarkDataTransferAsCompleted(L2HAL_GC9A01_LFB_ContextStruct *context);

/**
 * Mark framebuffer as dirty
 * Call this after framebuffer external modification
 */
void L2HAL_GC9A01_LFB_MarkFramebufferAsDirty(L2HAL_GC9A01_LFB_ContextStruct* context);

#endif /* L2HAL_DRIVERS_DISPLAY_GC9A01_LOCAL_FRAMEBUFFER_INCLUDE_L2HAL_GC9A01_LFB_LFB_H_ */
