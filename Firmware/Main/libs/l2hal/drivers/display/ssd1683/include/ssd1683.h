/*
 * ssd1683.h
 *
 *  Created on: Apr 11, 2025
 *      Author: shakti
 */

#ifndef DRIVERS_DISPLAY_SSD1683_INCLUDE_SSD1683_H_
#define DRIVERS_DISPLAY_SSD1683_INCLUDE_SSD1683_H_

#include <stdbool.h>
#include "../../../../mcu_dependent/l2hal_mcu.h"
#include "../../../../fmgl/include/fmgl.h"

/**
 * Display sizes
 */
#define L2HAL_SSD1683_DISPLAY_WIDTH 400
#define L2HAL_SSD1683_DISPLAY_HEIGHT 300

#define L2HAL_SSD1683_DISPLAY_LINE_SIZE (L2HAL_SSD1683_DISPLAY_WIDTH / 8)
#define L2HAL_SSD1683_FRAMEBUFFER_SIZE (L2HAL_SSD1683_DISPLAY_LINE_SIZE * L2HAL_SSD1683_DISPLAY_HEIGHT)

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
	 * Busy pin
	 */
	GPIO_TypeDef* BusyPort;
	uint16_t BusyPin;

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
	 * Current drawing color. 0x00 - black, 0xFF - white
	 */
	uint8_t BinarizedActiveColor;

	/**
	 * If true, then data transfer in progress and we must wait for next one
	 */
	volatile bool IsDataTransferInProgress;

	/**
	 * Local framebuffer
	 */
	uint8_t Framebuffer[L2HAL_SSD1683_FRAMEBUFFER_SIZE];

	/**
	 * If set to true, then driver will perform full refreshs each AutoFullRefreshFramesCount
	 * Applied for L2HAL_SSD1683_PushFramebufferPartial()
	 */
	bool IsAutoFullRefresh;

	/**
	 * Do full refresh every this frames count if IsAutoFullRefresh == true
	 */
	uint8_t AutoFullRefreshFramesCount;

	/**
	 * Do full refresh when this variable == 0
	 */
	uint8_t FramesTillFullRefresh;
}
L2HAL_SSD1683_ContextStruct;

/**
 * Initialize SSD1683-based display
 */
void L2HAL_SSD1683_Init
(
	L2HAL_SSD1683_ContextStruct* context,
	SPI_HandleTypeDef *spiHandle,

	GPIO_TypeDef* busyPort,
	uint16_t busyPin,

	GPIO_TypeDef* resetPort,
	uint16_t resetPin,

	GPIO_TypeDef* dataCommandPort,
	uint16_t dataCommandPin,

	GPIO_TypeDef* chipSelectPort,
	uint16_t chipSelectPin,

	FMGL_API_ColorStruct initColor,

	bool isAutoFullRefresh,

	uint8_t autoFullRefreshFramesCount
);

/**
 * Get display width
 */
uint16_t L2HAL_SSD1683_GetWidth(void);

/**
 * Get display height
 */
uint16_t L2HAL_SSD1683_GetHeight(void);

/**
 * Set color what will be used for drawing.
 */
void L2HAL_SSD1683_SetActiveColor(L2HAL_SSD1683_ContextStruct* context, FMGL_API_ColorStruct color);

/**
 * Draw pixel
 */
void L2HAL_SSD1683_DrawPixel(L2HAL_SSD1683_ContextStruct* context, uint16_t x, uint16_t y);

/**
 * Get pixel color
 */
FMGL_API_ColorStruct L2HAL_SSD1683_GetPixel(L2HAL_SSD1683_ContextStruct* context, uint16_t x, uint16_t y);

/**
 * Fill framebuffer with active color
 */
void L2HAL_SSD1683_ClearFramebuffer(L2HAL_SSD1683_ContextStruct* context, FMGL_API_ColorStruct clearColor);

/**
 * Push framebuffer to display (full update)
 */
void L2HAL_SSD1683_PushFramebufferFull(L2HAL_SSD1683_ContextStruct* context);

/**
 * Push framebuffer to display (partial update)
 */
void L2HAL_SSD1683_PushFramebufferPartial(L2HAL_SSD1683_ContextStruct* context);

/**
 * Mark transfer as completed (call it from DMA IRQ)
 */
void L2HAL_SSD1683_MarkDataTransferAsCompleted(L2HAL_SSD1683_ContextStruct *context);

/**
 * Save framebuffer to external memory
 */
void L2HAL_SSD1683_SaveFramebuffer
(
	L2HAL_SSD1683_ContextStruct *context,
	void* RAMContext,
	uint32_t saveAddress,
	void (*FramebufferMemoryWriteFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*)
);

/**
 * Load framebuffer from external memory
 */
void L2HAL_SSD1683_LoadFramebuffer
(
	L2HAL_SSD1683_ContextStruct *context,
	void* RAMContext,
	uint32_t loadAddress,
	void (*FramebufferMemoryReadFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*)
);

/**
 * Power display off. Use L2HAL_SSD1683_Init() to power it on back
 */
void L2HAL_SSD1683_PowerOff(L2HAL_SSD1683_ContextStruct *context);

/**
 * Power display off and enter deepsleep mode. Use L2HAL_SSD1683_Init() to power it on back
 */
void L2HAL_SSD1683_EnterDeepSleep(L2HAL_SSD1683_ContextStruct *context, bool isRetainRam);

#endif /* DRIVERS_DISPLAY_SSD1683_INCLUDE_SSD1683_H_ */
