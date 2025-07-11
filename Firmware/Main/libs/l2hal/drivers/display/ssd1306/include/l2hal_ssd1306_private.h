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
 * @brief SSD1306 OLED display controller level 2 HAL driver - private stuff.
 */

#ifndef L2HAL_DRIVERS_DISPLAY_SSD1306_INCLUDE_L2HAL_SSD1306_PRIVATE_H_
#define L2HAL_DRIVERS_DISPLAY_SSD1306_INCLUDE_L2HAL_SSD1306_PRIVATE_H_

#include "../include/l2hal_ssd1306.h"

/**
 * Display I/O timeout in milliseconds.
 */
#define L2HAL_SSD1306_IO_TIMEOUT 100U

/**
 * Display can have one of this two addresses. Addresses already shifted.
 */
#define L2HAL_SSD1306_POSSIBLE_ADDRESS1 0x78U
#define L2HAL_SSD1306_POSSIBLE_ADDRESS2 0x7AU

/**
 * Possible values of control byte (sent after device address).
 */
#define L2HAL_SSD1306_CONTROL_BYTE_COMMAND 0b00000000 /* Next byte is command */
#define L2HAL_SSD1306_CONTROL_BYTE_DATA 0b01000000 /* Next byte is data */

/**
 * Default brightness.
 */
#define L2HAL_SSD1306_DEFAULT_BRIGHTNESS 0xFF

/**
 * Page (horizontal stripe) height.
 */
#define L2HAL_SSD1306_PAGE_HEIGHT 8

/**
 * Pixel states.
 */
#define L2HAL_SSD1306_PIXEL_OFF 0x00
#define L2HAL_SSD1306_PIXEL_ON 0xFF

/**
 * If L2HAL_SSD1306_GetBrightness() for giver color is higher or equal to this value,
 * then pixel will be lit.
 */
#define L2HAL_SSD1306_BRIGHTNESS_THRESHOLD 127U

/**
 * Factors for brightness calculation, normalized to 0xFF.
 * Br = 0.2126 * R + 0.7152 * G + 0.0722 * B
 */
#define L2HAL_SSD1306_BRIGHTNESS_DIV_FACTOR 255U
#define L2HAL_SSD1306_BRIGHTNESS_MUL_R_FACTOR 54U
#define L2HAL_SSD1306_BRIGHTNESS_MUL_G_FACTOR 182U
#define L2HAL_SSD1306_BRIGHTNESS_MUL_B_FACTOR 18U

/**
 * Maximal possible brightness.
 */
#define L2HAL_SSD1306_MAX_BRIGHTNESS FMGL_API_MAX_CHANNEL_BRIGHTNESS

/**
 * Return this color if pixel is lit.
 */
#define L2HAL_SSD1306_PIXEL_ON_R FMGL_API_MAX_CHANNEL_BRIGHTNESS
#define L2HAL_SSD1306_PIXEL_ON_G FMGL_API_MAX_CHANNEL_BRIGHTNESS
#define L2HAL_SSD1306_PIXEL_ON_B FMGL_API_MAX_CHANNEL_BRIGHTNESS

/**
 * Return this color if pixel is off.
 */
#define L2HAL_SSD1306_PIXEL_OFF_R 0
#define L2HAL_SSD1306_PIXEL_OFF_G 0
#define L2HAL_SSD1306_PIXEL_OFF_B 0


/**
 * Call this function to check if data transfer undergoing. If so, function will hang until it completed.
 * @param context Pointer to driver context.
 */
void L2HAL_SSD1306_WaitForTransferCompletion(L2HAL_SSD1306_ContextStruct* context);

/**
 * Send command to display.
 * @param context Pointer to driver context.
 * @param command Command to send.
 */
void L2HAL_SSD1306_WriteCommand(L2HAL_SSD1306_ContextStruct* context, uint8_t command);

/**
 * Send data to videomemory.
 * @param context Pointer to driver context.
 * @param data Pointer to data, data must not change during transfer (until L2HAL_SSD1306_WaitForTransferCompletion() is called).
 */
void L2HAL_SSD1306_WriteData(L2HAL_SSD1306_ContextStruct* context, uint8_t* data, uint16_t length);

/**
 * Causes L2HAL error if display not found.
 * @param context Pointer to driver context.
 */
void L2HAL_SSD1306_CheckIfFound(L2HAL_SSD1306_ContextStruct* context);

/**
 * Getting x and y position and checking them against display size. If incorrect returns false.
 * If correct - putting framebuffer array index into index and bit mask (all bits is 0 except one, pointing to requested
 * pixel) for pixel in byte.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param index Pointer to variable, where framebuffer array index will be put.
 * @param mask Pointer to variable, where bitmask, corresponing to given pixel, will be put.
 * @return True if coordinates are correct, false otherwise.
 */
bool L2HAL_SSD1306_GetFramebufferAddress(uint16_t x, uint16_t y, uint16_t* index, uint8_t* mask);

/**
 * Returns brightness [0-L2HAL_SSD1306_MAX_BRIGHTNESS] of given color.
 * @param color Color, whose brightness have to be calculated.
 * @return Brightness [0-L2HAL_SSD1306_MAX_BRIGHTNESS] of given color.
 */
uint16_t L2HAL_SSD1306_GetBrightness(FMGL_API_ColorStruct color);

#endif /* L2HAL_DRIVERS_DISPLAY_SSD1306_INCLUDE_L2HAL_SSD1306_PRIVATE_H_ */
