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
 * @brief SSD1327 OLED display controller level 2 HAL driver.
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
 *            L2HAL_SSD1327_InterruptTransferCompleted(&L2HAL_SSD1327_Context);
 *        }
 *    }
 *
 * 3) Create instance of L2HAL_SSD1327_ContextStruct.
 *    Sample code:
 *
 *	  L2HAL_SSD1327_ContextStruct L2HAL_SSD1327_Context;
 *
 * 4) Call L2HAL_SSD1327_DetectDisplay() to attempt to detect display automatically at it's usual addresses or L2HAL_SSD1327_DetectDisplayAtAddress() to
 *    attempt to detect display at user-provided address.
 *
 *	  L2HAL_SSD1327_Context = L2HAL_SSD1327_DetectDisplay(&I2CHandle);
 *
 * 5) Initialize display:
 *    L2HAL_SSD1327_InitDisplay(&L2HAL_SSD1327_Context);
 *
 * 6) Draw on display by hands or use my FMGL to draw on it.
 *
 * 7) Push local framebuffer (stored in MCU RAM) to display by calling L2HAL_SSD1327_PushFramebuffer().
 */

#ifndef L2HAL_DRIVERS_DISPLAY_SSD1327_INCLUDE_L2HAL_SSD1327_H_
#define L2HAL_DRIVERS_DISPLAY_SSD1327_INCLUDE_L2HAL_SSD1327_H_

#include <stdint.h>
#include "../../../../mcu_dependent/l2hal_mcu.h"
#include "../../../../include/l2hal_aux.h"
#include "../include/l2hal_ssd1327_commands.h"
#include "../../../../fmgl/include/fmgl.h"

/**
 * If defined, then monochrome (instead of grayscale) mode will be used to save memory
 */
#define L2HAL_SSD1327_MONOCHROME_MODE

/**
 * Display dimensions.
 */
#define L2HAL_SSD1327_DISPLAY_WIDTH 128U
#define L2HAL_SSD1327_DISPLAY_HEIGHT 128U

/**
 * Framebuffer size.
 */
#ifdef L2HAL_SSD1327_MONOCHROME_MODE
	#define L2HAL_SSD1327_FRAMEBUFFER_SIZE L2HAL_SSD1327_DISPLAY_WIDTH * L2HAL_SSD1327_DISPLAY_HEIGHT / 8U
#else
	#define L2HAL_SSD1327_FRAMEBUFFER_SIZE L2HAL_SSD1327_DISPLAY_WIDTH * L2HAL_SSD1327_DISPLAY_HEIGHT / 2U
#endif

/**
 * One line of display (size in bytes)
 */
#define L2HAL_SSD1327_DECOMPRESSED_LINE_SIZE 64U

#define L2HAL_SSD1327_COMPRESSED_LINE_SIZE 16U

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
	 * void L2HAL_SSD1327_PushFramebuffer() for details.
	 */
	uint8_t Framebuffer[L2HAL_SSD1327_FRAMEBUFFER_SIZE];

#ifdef L2HAL_SSD1327_MONOCHROME_MODE
	/**
	 * Line buffer (only for monochrome mode)
	 */
	uint8_t lineBuffer[L2HAL_SSD1327_DECOMPRESSED_LINE_SIZE];
#endif

	/**
	 * If true then data transfer in progress.
	 * Have to be volatile, because changed in interrupt handler.
	 */
	volatile bool IsDataTransferInProgress;

	/**
	 * Active color [0-15]
	 */
	uint8_t ActiveColor;

} L2HAL_SSD1327_ContextStruct;

/**
 * Display modes
 */
typedef enum
{
	Normal,
	AllPixelsOn,
	AllPixelsOff,
	Inverse
} L2HAL_SSD1327_DisplayMode;

/**
 * Attempts to detect display on bus at given address. If detected, then generates display context.
 * @param i2cBusHandle Pointer to I2C bus handler.
 * @param address Bus addres to search at.
 */
L2HAL_SSD1327_ContextStruct L2HAL_SSD1327_DetectDisplayAtAddress(I2C_HandleTypeDef* i2cBusHandle, uint8_t address);

/**
 * Attempts to detect display on bus. If detected, then generates display context.
 * @param i2cBusHandle Pointer to I2C bus handler.
 */
L2HAL_SSD1327_ContextStruct L2HAL_SSD1327_DetectDisplay(I2C_HandleTypeDef* i2cBusHandle);

/**
 * Initialize and turn display on.
 */
void L2HAL_SSD1327_InitDisplay(L2HAL_SSD1327_ContextStruct* context);

/**
 * Turn display on.
 * @param context Driver context.
 */
void L2HAL_SSD1327_TurnOn(L2HAL_SSD1327_ContextStruct* context);

/**
 * Turn display off.
 * @param context Driver context.
 */
void L2HAL_SSD1327_TurnOff(L2HAL_SSD1327_ContextStruct* context);

/**
 * Set display brightness.
 * @param context Driver context pointer
 * @param brightness Brightness from 0 to 255.
 */
void L2HAL_SSD1327_SetBrightness(L2HAL_SSD1327_ContextStruct* context, uint8_t brightness);

/**
 * Set display column start and end addresses.
 * @param context Driver context pointer.
 * @param startAddress Column start address (max 0x3F)
 * @param endAddress Column end address (max 0x3F)
 */
void L2HAL_SSD1327_SetColumnAddress(L2HAL_SSD1327_ContextStruct* context, uint8_t startAddress, uint8_t endAddress);

/**
 * Loads default grayscale linear LUT into display.
 * @param context Driver context pointer.
 */
void L2HAL_SSD1327_LoadDefaultGrayscaleTable(L2HAL_SSD1327_ContextStruct* context);

/**
 * Set phases lenghts (see datasheet). Allowed values are 1-15
 * @param context Pointer to driver context
 */
void L2HAL_SSD1327_SetPhasesLenghts(L2HAL_SSD1327_ContextStruct* context, uint8_t phase1, uint8_t phase2);

/**
 * Set divide ratio (D) and Fosc (see datasheet).
 * @param context Driver context pointer
 * @param divideRatio Divide ratio, valid values 1-16
 * @param fOsc Oscillator frequency, valid values 0-15
 */
void L2HAL_SSD1327_SetDClockFosc(L2HAL_SSD1327_ContextStruct* context, uint8_t divideRatio, uint8_t fOsc);

/**
 * Select what regulator to use (see datasheet).
 * @param context Driver context pointer
 * @param enableExternalRegulator If true, then external Vdd regulator will be used
 */
void L2HAL_SSD1327_SetRegulator(L2HAL_SSD1327_ContextStruct* context, bool enableExternalRegulator);

/**
 * Set precharge voltage (see datasheet).
 * @param context Driver context pointer
 * @param voltage Precharge voltage, max is 0x08
 */
void L2HAL_SSD1327_SetPrechargeVoltage(L2HAL_SSD1327_ContextStruct* context, uint8_t voltage);

/**
 * Set various functions
 * @param context Driver context pointer
 * @param enableSecondPrecharge Enable second precharge
 * @param enableExternalVSL Enable external VSL
 */
void L2HAL_SSD1327_SetFunctionB(L2HAL_SSD1327_ContextStruct* context, bool enableSecondPrecharge, bool enableExternalVSL);

/**
 * Set second precharge period
 * @param context Driver context pointer
 * @param period Second precharge period, [0-15]
 */
void L2HAL_SSD1327_SetSecondPrechargePeriod(L2HAL_SSD1327_ContextStruct* context, uint8_t period);

/**
 * Set COM deselect voltage level.
 * @param context Driver context pointer
 * @param Vcomh Vcomh voltage [0-7]
 */
void L2HAL_SSD1327_SetVCOMH(L2HAL_SSD1327_ContextStruct* context, uint8_t vcomh);

/**
 * Set MUX ratio
 * @param context Driver context pointer
 * @param muxRatio MUX ratio [16-128]
 */
void L2HAL_SSD1327_SetMUXRatio(L2HAL_SSD1327_ContextStruct* context, uint8_t muxRatio);

/**
 * Set display mode
 * @param context Driver context pointer
 * @param mode Display mode.
 */
void L2HAL_SSD1327_SetDisplayMode(L2HAL_SSD1327_ContextStruct* context, L2HAL_SSD1327_DisplayMode mode);

/**
 * Set display start line
 * @param context Driver context pointer
 * @param startLine Display start line [0-127]
 */
void L2HAL_SSD1327_SetStartLine(L2HAL_SSD1327_ContextStruct* context, uint8_t startLine);

/**
 * Set vertical offset
 * @param context Driver context pointer
 * @param verticalOffset Vertical offset [0-127]
 */
void L2HAL_SSD1327_SetVerticalOffset(L2HAL_SSD1327_ContextStruct* context, uint8_t verticalOffset);

/**
 * Set row start and end addresses.
 * @param context Driver context pointer
 * @param rowStartAddress Row start address [0-127]
 * @param rowEndAddress Row end address [0-127]
 */
void L2HAL_SSD1327_SetRowAddresses(L2HAL_SSD1327_ContextStruct* context, uint8_t rowStartAddress, uint8_t rowEndAddress);

/**
 * Setup various remaps (see datasheet for details).
 * @param context Driver context pointer
 * @param enableColumnAddressRemap Enable column address remap
 * @param enableNibbleRemap Enable libble remap
 * @param enableVerticalAddressIncrement Vertical address increment if true, horizontal if false
 * @param enableComRemap Enable COM remap
 * @param enableComSplitOddEven Enable COM split odd even
 */
void L2HAL_SSD1327_SetRemap(L2HAL_SSD1327_ContextStruct* context,
		bool enableColumnAddressRemap,
		bool enableNibbleRemap,
		bool enableVerticalAddressIncrement,
		bool enableComRemap,
		bool enableComSplitOddEven);

/**
 * Setup custom grayscale table.
 * @param context Driver context pointer
 * @param table Table for 15 grayscale levels - GS1-GS15 (GS0 is always 0). It must be 0<GS1<GS2<GS3...GS15<64
 */
void L2HAL_SSD1327_SetGrayscaleTable(L2HAL_SSD1327_ContextStruct* context, uint8_t table[15]);

/**
 * Returns display width.
 * @return Display width.
 */
uint16_t L2HAL_SSD1327_GetWidth(void);

/**
 * Returns display height.
 * @return Display height.
 */
uint16_t L2HAL_SSD1327_GetHeight(void);

/**
 * Set color what will be used for drawing.
 * @param context Pointer to driver context.
 * @param color Set this color as active.
 */
void L2HAL_SSD1327_SetActiveColor(L2HAL_SSD1327_ContextStruct* context, FMGL_API_ColorStruct color);

/**
 * Draws pixel with active color. Do nothing if coordinates are incorrect. DOESN'T PUSH
 * FRAMEBUFFER.
 *
 * Function will wait if data transfer is ongoing, because framebuffer can't be changed this time.
 * @param context Pointer to driver context.
 * @param x Pixel X coordinate.
 * @param y Pixel Y coordinate.
 */
void L2HAL_SSD1327_DrawPixel(L2HAL_SSD1327_ContextStruct* context, uint16_t x, uint16_t y);

/**
 * Get color of pixel with given coordinates. Return off pixel color if coordinates are incorrect.
 * @param context Pointer to driver context.
 * @param x Pixel X coordinate.
 * @param y Pixel Y coordinate.
 * @return Color of given pixel or off pixel color if coordinates are incorrect.
 */
FMGL_API_ColorStruct L2HAL_SSD1327_GetPixel(L2HAL_SSD1327_ContextStruct* context, uint16_t x, uint16_t y);

/**
 * Push framebuffer to display.
 * @param context Driver context pointer
 */
void L2HAL_SSD1327_PushFramebuffer(L2HAL_SSD1327_ContextStruct* context);

/**
 * Call this function from I2C interrupt transfer completed handler, i.e. from void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
 * for correct I2C bus.
 * @param context Pointer to driver context.
 */
void L2HAL_SSD1327_InterruptTransferCompleted(L2HAL_SSD1327_ContextStruct* context);

#endif /* L2HAL_DRIVERS_DISPLAY_SSD1327_INCLUDE_L2HAL_SSD1327_H_ */
