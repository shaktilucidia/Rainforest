/*
	This file is part of Shakti Lucidia's FerFox.

	FerFox is free software: you can redistribute it and/or modify
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

	Repository: https://github.com/shaktilucidia/FerFox

	-------------------------------------------------------------------------
 */

/**
 * @file
 * @brief SSD1306 OLED display controller level 2 HAL driver.
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
 * 2) Add code, calling L2HAL_SSD1306_InterruptTransferCompleted() from I2C transfer complete interrupt handler (for correct bus, of course).
 *    Sample code:
 *
 *    void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
 *    {
 *        if (hi2c->Instance == I2C1)
 *        {
 *            L2HAL_SSD1306_InterruptTransferCompleted(&L2HAL_SSD1306_Context);
 *        }
 *    }
 *
 * 3) Create instance of L2HAL_SSD1306_ContextStruct and set it's I2CHandler pointer to I2C bus handle from step 1.
 *    Sample code:
 *
 *    L2HAL_SSD1306_Context.I2CHandle = &I2CHandle;
 *
 * 4) Call L2HAL_SSD1306_DetectDisplay() to attempt to detect display automatically at it's usual addresses or L2HAL_SSD1306_DetectDisplayAtAddress() to
 *    attempt to detect display at user-provided address.
 *
 * 5) Probably you'll want to turn display on via L2HAL_SSD1306_TurnDisplayOn() command.
 *
 * 6) Draw on display by hands or use my FMGL to draw on it.
 *
 * 7) Push local framebuffer (stored in MCU RAM) to display by calling L2HAL_SSD1306_PushFramebuffer().
 */

#ifndef L2HAL_INCLUDE_SSD1306HAL_L2HAL_SSD1306_H_
#define L2HAL_INCLUDE_SSD1306HAL_L2HAL_SSD1306_H_

#include <stdint.h>
#include "../../../../mcu_dependent/l2hal_mcu.h"
#include "../../../../include/l2hal_aux.h"
#include "../include/l2hal_ssd1306_commands.h"
#include "../../../../fmgl/include/fmgl.h"

/**
 * Display dimensions.
 */
#define L2HAL_SSD1306_DISPLAY_WIDTH 128
#define L2HAL_SSD1306_DISPLAY_HEIGHT 64

/**
 * Framebuffer size.
 */
#define L2HAL_SSD1306_FRAMEBUFFER_SIZE L2HAL_SSD1306_DISPLAY_WIDTH * L2HAL_SSD1306_DISPLAY_HEIGHT / 8

/**
 * Display context, I2C connection, device address etc are stored here.
 */
typedef struct
{
	/**
	 * True if display found on bus, false otherwise.
	 */
	bool IsFound;

	/**
	 * Talk with display via this I2C instance.
	 */
	I2C_HandleTypeDef* I2CHandle;

	/**
	 * Display bus address.
	 */
	uint8_t BusAddress;

	/**
	 * Command code is stored here during time of command transfer. !! DO NOT CHANGE DURING DATA TRANSFER !!
	 */
	uint8_t CommandCode;

	/** Framebuffer. !! CONTENT MUST NOT BE CHANGED DURING DATA TRANSFER !! See
	 * void L2HAL_SSD1306_PushFramebuffer() for details.
	 */
	uint8_t Framebuffer[L2HAL_SSD1306_FRAMEBUFFER_SIZE];

	/**
	 * If true then data transfer in progress.
	 * Have to be volatile, because changed in interrupt handler.
	 */
	volatile bool IsDataTransferInProgress;

	/**
	 * Active color.
	 */
	uint8_t ActiveColor;

} L2HAL_SSD1306_ContextStruct;


/**
 * Attempts to detect display on bus. If detected, then IsFound and BussAddress will be set in context.
 * @param context Pointer to driver context.
 */
void L2HAL_SSD1306_DetectDisplay(L2HAL_SSD1306_ContextStruct* context);

/**
 * Attempts to detect display on bus at given address. If detected, then IsFound and BussAddress will be set in context.
 * @param context Pointer to driver context.
 * @param address Attempt to detect display at given address.
 */
void L2HAL_SSD1306_DetectDisplayAtAddress(L2HAL_SSD1306_ContextStruct* context, uint8_t address);

/**
 * Initializes display and turning it on. Doesn't affect framebuffer contents.
 * @param context Pointer to driver context.
 */
void L2HAL_SSD1306_TurnDisplayOn(L2HAL_SSD1306_ContextStruct* context);

/**
 * Turn display off (screen is off, Vcc step-up is off).
 * @param context Pointer to driver context.
 */
void L2HAL_SSD1306_TurnDisplayOff(L2HAL_SSD1306_ContextStruct* context);

/**
 * Set brightness. 0x00 is minimal, 0xFF is maximal.
 * @param context Pointer to driver context.
 * @param brightness Brightness level.
 */
void L2HAL_SSD1306_SetBrightness(L2HAL_SSD1306_ContextStruct* context, uint8_t brightness);

/**
 * Returns display width.
 * @return Display width.
 */
uint16_t L2HAL_SSD1306_GetWidth(void);

/**
 * Returns display height.
 * @return Display height.
 */
uint16_t L2HAL_SSD1306_GetHeight(void);

/**
 * Set color what will be used for drawing.
 * @param context Pointer to driver context.
 * @param color Set this color as active.
 */
void L2HAL_SSD1306_SetActiveColor(L2HAL_SSD1306_ContextStruct* context, FMGL_API_ColorStruct color);

/**
 * Draws pixel with active color. Do nothing if coordinates are incorrect. DOESN'T PUSH
 * FRAMEBUFFER.
 *
 * Function will wait if data transfer is ongoing, because framebuffer can't be changed this time.
 * @param context Pointer to driver context.
 * @param x Pixel X coordinate.
 * @param y Pixel Y coordinate.
 */
void L2HAL_SSD1306_DrawPixel(L2HAL_SSD1306_ContextStruct* context, uint16_t x, uint16_t y);

/**
 * Get color of pixel with given coordinates. Return off pixel color if coordinates are incorrect.
 * @param context Pointer to driver context.
 * @param x Pixel X coordinate.
 * @param y Pixel Y coordinate.
 * @return Color of given pixel or off pixel color if coordinates are incorrect.
 */
FMGL_API_ColorStruct L2HAL_SSD1306_GetPixel(L2HAL_SSD1306_ContextStruct* context, uint16_t x, uint16_t y);

/**
 * Pushes framebuffer to display, if push already initiated waits for completion.
 * @param context Pointer to driver context.
 */
void L2HAL_SSD1306_PushFramebuffer(L2HAL_SSD1306_ContextStruct* context);

/**
 * Call this function from I2C interrupt transfer completed handler, i.e. from void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
 * for correct I2C bus.
 * @param context Pointer to driver context.
 */
void L2HAL_SSD1306_InterruptTransferCompleted(L2HAL_SSD1306_ContextStruct* context);

#endif /* L2HAL_INCLUDE_SSD1306HAL_L2HAL_SSD1306_H_ */
