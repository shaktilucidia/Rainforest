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

#ifndef L2HAL_DRIVERS_DISPLAY_GC9A01_INCLUDE_L2HAL_GC9A01_H_
#define L2HAL_DRIVERS_DISPLAY_GC9A01_INCLUDE_L2HAL_GC9A01_H_

#include "../../../../mcu_dependent/l2hal_mcu.h"
#include "../../../internal/crc/include/l2hal_crc.h"
#include <stdbool.h>
#include "../../../../fmgl/include/fmgl.h"

/**
 * Display sizes
 */
#define L2HAL_GC9A01_DISPLAY_WIDTH 240
#define L2HAL_GC9A01_DISPLAY_HEIGHT 240

#define L2HAL_GC9A01_DISPLAY_LINE_SIZE (L2HAL_GC9A01_DISPLAY_WIDTH * 3)
#define L2HAL_GC9A01_FRAMEBUFFER_SIZE (L2HAL_GC9A01_DISPLAY_LINE_SIZE * L2HAL_GC9A01_DISPLAY_HEIGHT)

/**
 * Sizes of local pixels cache
 */
#define L2HAL_GC9A01_CACHE_WIDTH 16
#define L2HAL_GC9A01_CACHE_HEIGHT 16
#define L2HAL_GC9A01_CACHE_LINE_SIZE (L2HAL_GC9A01_CACHE_WIDTH * 3)
#define L2HAL_GC9A01_CACHE_SIZE (L2HAL_GC9A01_CACHE_WIDTH * L2HAL_GC9A01_CACHE_HEIGHT * 3)

/**
 * How much pixels to transmit (in line) if we found dirty one
 * L2HAL_GC9A01_DISPLAY_WIDTH must be a multiple of this
 */
#define L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH 40

/**
 * Dirty pixels buffer blocks count. Block have length of L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH pixels
 */
#define L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_BLOCKS_COUNT (L2HAL_GC9A01_DISPLAY_WIDTH / L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH)

/**
 * Dirty pixels buffer line size
 */
#define L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_SIZE ((L2HAL_GC9A01_DISPLAY_WIDTH / L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH) * 4)

/**
 * Dirty pixels buffer total size
 */
#define L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_SIZE (L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_SIZE * L2HAL_GC9A01_DISPLAY_HEIGHT)

enum L2HAL_GC9A01_Orientation
{
	ROTATION_0,  /**< ROTATION_0 */
	ROTATION_90, /**< ROTATION_90 */
	ROTATION_180,/**< ROTATION_180 */
	ROTATION_270 /**< ROTATION_270 */
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
	 * Local (i.e. not in external memory) pixels cache
	 */
	uint8_t PixelsCache[L2HAL_GC9A01_CACHE_SIZE];

	/**
	 * Current location of pixels cache
	 */
	uint16_t PixelsCacheX;
	uint16_t PixelsCacheY;

	/**
	 * If true, then data transfer in progress and we must wait for next one
	 */
	volatile bool IsDataTransferInProgress;

	/**
	 * Framebuffer driver context pointer (usually it will be RAM-driver context)
	 */
	void* FramebufferDriverContext;

	/**
	 * Pointer to function, writing to framebuffer.
	 * 1st param - pointer to framebuffer driver context (NOT to display driver context)
	 * 2nd param - write start address
	 * 3rd param - write length
	 * 4rd param - pointer to buffer with data to write
	 */
	void (*FramebufferMemoryWriteFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*);

	/**
	 * Pointer to function, reading from framebuffer.
	 * 1st param - pointer to framebuffer driver context (NOT to display driver context)
	 * 2nd param - read start address
	 * 3rd param - read length
	 * 4rd param - pointer to buffer where data will be readed
	 */
	void (*FramebufferMemoryReadFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*);

	/**
	 * Framebuffer base address
	 */
	uint32_t FramebufferBaseAddress;

	/**
	 * Previous frame buffer (to calculate dirty pixels map)
	 */
	uint32_t PreviousFrameBufferBaseAddress;

	/**
	 * CRC calculation unit context
	 */
	L2HAL_CRCContextStruct* CrcContext;
}
L2HAL_GC9A01_ContextStruct;


/**
 * Initialize GC9A01-based display
 */
void L2HAL_GC9A01_Init
(
	L2HAL_GC9A01_ContextStruct* context,
	SPI_HandleTypeDef *spiHandle,

	GPIO_TypeDef* resetPort,
	uint16_t resetPin,

	GPIO_TypeDef* dataCommandPort,
	uint16_t dataCommandPin,

	GPIO_TypeDef* chipSelectPort,
	uint16_t chipSelectPin,

	enum L2HAL_GC9A01_Orientation orientation,

	void* framebufferDriverContext,

	void (*framebufferMemoryWriteFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*),

	void (*framebufferMemoryReadFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*),

	uint32_t framebufferBaseAddress,

	uint32_t previousFrameBufferBaseAddress,

	L2HAL_CRCContextStruct* crcContext
);


/**
 * Set columns range for subsequent memory writes
 */
void L2HAL_GC9A01_SetColumnsRange(L2HAL_GC9A01_ContextStruct *context, uint16_t colStart, uint16_t colEnd);

/**
 * Set rows range for subsequent memory writes
 */
void L2HAL_GC9A01_SetRowsRange(L2HAL_GC9A01_ContextStruct *context, uint16_t rowStart, uint16_t rowEnd);

/**
 * Get display width
 */
uint16_t L2HAL_GC9A01_GetWidth(void);

/**
 * Get display height
 */
uint16_t L2HAL_GC9A01_GetHeight(void);

/**
 * Fill display with active color
 */
void L2HAL_GC9A01_ClearDisplay(L2HAL_GC9A01_ContextStruct *context);

/**
 * Draw pixel
 */
void L2HAL_GC9A01_DrawPixel(L2HAL_GC9A01_ContextStruct* context, uint16_t x, uint16_t y);

/**
 * Get pixel color
 */
FMGL_API_ColorStruct L2HAL_GC9A01_GetPixel(L2HAL_GC9A01_ContextStruct* context, uint16_t x, uint16_t y);

/**
 * Set color what will be used for drawing.
 */
void L2HAL_GC9A01_SetActiveColor(L2HAL_GC9A01_ContextStruct* context, FMGL_API_ColorStruct color);

/**
 * Push framebuffer to display
 */
void L2HAL_GC9A01_PushFramebuffer(L2HAL_GC9A01_ContextStruct* context);

/**
 * Fill framebuffer with black color
 */
void L2HAL_GC9A01_ClearFramebuffer(L2HAL_GC9A01_ContextStruct* context);

/**
 * Set framebuffer base address (in external RAM). Allows to have multiple framebuffers
 */
void L2HAL_GC9A01_SetFramebufferBaseAddress(L2HAL_GC9A01_ContextStruct* context, uint32_t baseAddress);

/**
 * Call it from SPI DMA TX completion interrupt
 */
void L2HAL_GC9A01_MarkDataTransferAsCompleted(L2HAL_GC9A01_ContextStruct *context);

#endif /* L2HAL_DRIVERS_DISPLAY_GC9A01_INCLUDE_L2HAL_GC9A01_H_ */
