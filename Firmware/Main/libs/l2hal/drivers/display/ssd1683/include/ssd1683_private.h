/*
 * ssd1683_private.h
 *
 *  Created on: Apr 11, 2025
 *      Author: shakti
 */

#ifndef DRIVERS_DISPLAY_SSD1683_INCLUDE_SSD1683_PRIVATE_H_
#define DRIVERS_DISPLAY_SSD1683_INCLUDE_SSD1683_PRIVATE_H_

#include "ssd1683.h"

/**
 * Interval between CS and RESET signals during display reset
 */
#define L2HAL_SSD1683_CS_TO_RESET 5

/**
 * Reset duration
 */
#define L2HAL_SSD1683_RESET_DURATION 50

/**
 * Wait this time after reset before starting to send commands
 */
#define L2HAL_SSD1683_DISPLAY_BOOT_TIME 120

/* Predefined white color */
FMGL_API_ColorStruct WhiteColor =
{
	.R = FMGL_API_MAX_CHANNEL_BRIGHTNESS,
	.G = FMGL_API_MAX_CHANNEL_BRIGHTNESS,
	.B = FMGL_API_MAX_CHANNEL_BRIGHTNESS
};

/* Predefined black color */
FMGL_API_ColorStruct BlackColor =
{
	.R = 0,
	.G = 0,
	.B = 0
};

/* Predefined red color */
FMGL_API_ColorStruct RedColor =
{
	.R = FMGL_API_MAX_CHANNEL_BRIGHTNESS,
	.G = 0,
	.B = 0
};

/**
 * Send reset signal to display
 */
void L2HAL_SSD1683_ResetDisplay(L2HAL_SSD1683_ContextStruct *context);

/**
 * Select / deselect chip
 */
void L2HAL_SSD1683_SelectChip(L2HAL_SSD1683_ContextStruct *context, bool isSelected);

/**
 * Wait until display is ready
 */
void L2HAL_SSD1683_WaitForReadiness(L2HAL_SSD1683_ContextStruct *context);

/**
 * Reset display
 */
void L2HAL_SSD1683_ResetDisplay(L2HAL_SSD1683_ContextStruct *context);

/**
 * Hang till the DMA transfer completed
 */
void L2HAL_SSD1683_WaitForDataTransferCompletion(L2HAL_SSD1683_ContextStruct *context);

/**
 * Write command to display
 */
void L2HAL_SSD1683_WriteCommand(L2HAL_SSD1683_ContextStruct *context, uint8_t command);

/**
 * Write 1 byte of data
 */
void L2HAL_SSD1683_WriteDataByte(L2HAL_SSD1683_ContextStruct *context, uint8_t data);

/**
 * Write data to display
 */
void L2HAL_SSD1683_WriteData(L2HAL_SSD1683_ContextStruct *context, uint8_t *data, uint16_t dataSize);

/**
 * Power display on
 */
void L2HAL_SSD1683_PowerOn(L2HAL_SSD1683_ContextStruct *context);

/**
 * Set output position
 */
void L2HAL_SSD1683_SetPosition(L2HAL_SSD1683_ContextStruct *context, uint16_t x, uint16_t y);

/**
 * Set addresses range
 */
void L2HAL_SSD1683_SetRange(L2HAL_SSD1683_ContextStruct *context, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/**
 * Update display
 */
void L2HAL_SSD1683_Update(L2HAL_SSD1683_ContextStruct *context);

/**
 * Update display partially (not spatial, but "weak update")
 */
void L2HAL_SSD1683_PartialUpdate(L2HAL_SSD1683_ContextStruct *context);

/**
 * Put this value into black framebuffer
 */
uint8_t L2HAL_SSD1683_GetBlackColorPattern(FMGL_API_ColorStruct color);

/**
 * Put this value into red framebuffer
 */
uint8_t L2HAL_SSD1683_GetRedColorPattern(FMGL_API_ColorStruct color);

/**
 * Push all necessary framebuffers to device
 */
void L2HAL_SSD1683_PushFramebuffersInternal(L2HAL_SSD1683_ContextStruct* context);

/**
 * Push red framebuffer
 */
void L2HAL_SSD1683_PushFramebufferInternalR(L2HAL_SSD1683_ContextStruct* context);

/**
 * Push B/W framebuffer
 */
void L2HAL_SSD1683_PushFramebufferInternalB(L2HAL_SSD1683_ContextStruct* context);

/**
 * Power display on
 */
void L2HAL_SSD1683_PowerOn(L2HAL_SSD1683_ContextStruct *context);

#endif /* DRIVERS_DISPLAY_SSD1683_INCLUDE_SSD1683_PRIVATE_H_ */
