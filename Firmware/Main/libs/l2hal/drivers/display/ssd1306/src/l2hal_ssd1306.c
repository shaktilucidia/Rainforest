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

#include "../include/l2hal_ssd1306_private.h"
#include "../../../../include/l2hal_errors.h"

void L2HAL_SSD1306_DetectDisplay(L2HAL_SSD1306_ContextStruct* context)
{
	uint8_t addrs[] = { L2HAL_SSD1306_POSSIBLE_ADDRESS1, L2HAL_SSD1306_POSSIBLE_ADDRESS2 };

	for (uint8_t i = 0; i < sizeof(addrs) / sizeof(addrs[0]); i++)
	{
		if (HAL_OK == HAL_I2C_IsDeviceReady(context->I2CHandle, addrs[i], 1, L2HAL_SSD1306_IO_TIMEOUT))
		{
			/* We found it */
			context->BusAddress = addrs[i];
			context->IsFound = true;
			context->IsDataTransferInProgress = false;

			return;
		}
	}

	/* Not found */
	context->IsFound = false;
}

void L2HAL_SSD1306_DetectDisplayAtAddress(L2HAL_SSD1306_ContextStruct* context, uint8_t address)
{
	if (HAL_OK == HAL_I2C_IsDeviceReady(context->I2CHandle, address, 1, L2HAL_SSD1306_IO_TIMEOUT))
	{
		context->BusAddress = address;
		context->IsFound = true;
		context->IsDataTransferInProgress = false;

		return;
	}

	context->IsFound = false;
}

void L2HAL_SSD1306_CheckIfFound(L2HAL_SSD1306_ContextStruct* context)
{
	if (true != context->IsFound)
	{
		L2HAL_Error(Generic);
	}
}


void L2HAL_SSD1306_WaitForTransferCompletion(L2HAL_SSD1306_ContextStruct* context)
{
	while(context->IsDataTransferInProgress) {}
}

void L2HAL_SSD1306_InterruptTransferCompleted(L2HAL_SSD1306_ContextStruct* context)
{
	if (context->IsDataTransferInProgress)
	{
		/* Data transfer completed */
		context->IsDataTransferInProgress = false;
	}
}


void L2HAL_SSD1306_WriteCommand(L2HAL_SSD1306_ContextStruct* context, uint8_t command)
{
	L2HAL_SSD1306_CheckIfFound(context);

	L2HAL_SSD1306_WaitForTransferCompletion(context); /* We can't send command when transfer is in progress */

	context->CommandCode = command;

	/* Transfer started */
	context->IsDataTransferInProgress = true;

	/* Using HAL_I2C_Mem_Write to prepend control byte without allocating buffer for it. */
	if (HAL_OK != HAL_I2C_Mem_Write_IT(context->I2CHandle, context->BusAddress, L2HAL_SSD1306_CONTROL_BYTE_COMMAND, 1, &context->CommandCode, 1))
	{
		context->IsDataTransferInProgress = false; /* Transfer didn't start */
		L2HAL_Error(Generic);
	}
}

void L2HAL_SSD1306_WriteData(L2HAL_SSD1306_ContextStruct* context, uint8_t* data, uint16_t length)
{
	L2HAL_SSD1306_CheckIfFound(context);

	L2HAL_SSD1306_WaitForTransferCompletion(context);

	/* Transfer started */
	context->IsDataTransferInProgress = true;

	/* Using HAL_I2C_Mem_Write to prepend control byte without allocating buffer for it */
	if (HAL_OK != HAL_I2C_Mem_Write_IT(context->I2CHandle, context->BusAddress, L2HAL_SSD1306_CONTROL_BYTE_DATA, 1, data, length))
	{
		context->IsDataTransferInProgress = false; /* Transfer didn't start */
		L2HAL_Error(Generic);
	}
}

void L2HAL_SSD1306_TurnDisplayOn(L2HAL_SSD1306_ContextStruct* context)
{
	/* Turning display off to avoid blinking during setup process. */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_OFF);

	/* Preparing power and clocks*/

	/* Setting maximal display frequency */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_DISPLAY_FREQUENCY);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_DISPLAY_FREQUENCY_MAX_DIVIDE_1);

	/* Setting precharge periods */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_PRECHARGE_PERIODS);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_PRECHARGE_PERIODS_DEFAULT);

	/* Set Vcomh deselect level */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_VCOMH_LEVEL);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_VCOMH_LEVEL_065);

	/* Setting up brightness */
	L2HAL_SSD1306_SetBrightness(context, L2HAL_SSD1306_DEFAULT_BRIGHTNESS);

	/* Turning on Vcc */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_VCC_MODE);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_VCC_MODE_ON);

	/* Setting hardware layout */

	/* Segments remap (horizontal flip) */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_SEGMENT_REMAP_REMAPPED);

	/* Vertical flip mode */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_COM_OUTPUT_SCAN_DIRECTION_REMAPPED);

	/* Set multiplex ratio */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_MULTIPLEX_RATIO);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_MULTIPLEX_RATIO_64);

	/* COM pins hardware configuration */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_COM_PINS_HW_CONFIG);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_COM_PINS_HW_CONFIG_ALT_NO_REMAP);

	/* Setting VRAM mode */

	/* Setting horizontal memory addressing mode to be able to write whole framebuffer at once */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_MEMORY_ADDRESSING_MODE);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_ADDRESSING_MODE_HORIZONAL);

	/* Setting leftmost and rightmost columns */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_COLUMN_ADDRESS);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_COLUMN_ADDRESS_LEFTMOST);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_COLUMN_ADDRESS_RIGHTMOST);

	/* Page start and end addresses (topmost and bottommost) */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_PAGE_ADDRESS);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_PAGE_ADDRESS_TOPMOST);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_PAGE_ADDRESS_BOTTOMMOST);

	/* Start line */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_START_LINE_0);

	/* Set display offset */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_DISPLAY_OFFSET);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_DISPLAY_OFFSET_0);

	/* Pixels brightness inversion */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_PIXELS_NORMAL);

	/* Data from VRAM are being displayed */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_VRAM_IGNORE_NO);

	/* We are ready, turning display on. */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_ON);
}

void L2HAL_SSD1306_TurnDisplayOff(L2HAL_SSD1306_ContextStruct* context)
{
	/* Turning display off */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_OFF);

	/* Turning off Vcc */
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_VCC_MODE);
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_VCC_MODE_OFF);
}

void L2HAL_SSD1306_SetBrightness(L2HAL_SSD1306_ContextStruct* context, uint8_t brightness)
{
	L2HAL_SSD1306_WriteCommand(context, L2HAL_SSD1306_COMMAND_SET_BRIGHTNESS_LEVEL);
	L2HAL_SSD1306_WriteCommand(context, brightness);
}

uint16_t L2HAL_SSD1306_GetWidth(void)
{
	return L2HAL_SSD1306_DISPLAY_WIDTH;
}
uint16_t L2HAL_SSD1306_GetHeight(void)
{
	return L2HAL_SSD1306_DISPLAY_HEIGHT;
}

void L2HAL_SSD1306_PushFramebuffer(L2HAL_SSD1306_ContextStruct* context)
{
	L2HAL_SSD1306_WriteData(context, context->Framebuffer, L2HAL_SSD1306_FRAMEBUFFER_SIZE);
}

bool L2HAL_SSD1306_GetFramebufferAddress(uint16_t x, uint16_t y, uint16_t* index, uint8_t* mask)
{
	if (L2HAL_SSD1306_DISPLAY_WIDTH <= x || L2HAL_SSD1306_DISPLAY_HEIGHT <= y)
	{
		return false;
	}

	*index = (y / L2HAL_SSD1306_PAGE_HEIGHT) * L2HAL_SSD1306_DISPLAY_WIDTH + x;
	*mask = 1 << (y % L2HAL_SSD1306_PAGE_HEIGHT);

	return true;
}

void L2HAL_SSD1306_SetActiveColor(L2HAL_SSD1306_ContextStruct* context, FMGL_API_ColorStruct color)
{
	uint16_t brightness = L2HAL_SSD1306_GetBrightness(color);

	if (L2HAL_SSD1306_BRIGHTNESS_THRESHOLD > brightness)
	{
		context->ActiveColor = L2HAL_SSD1306_PIXEL_OFF;
	}
	else
	{
		context->ActiveColor = L2HAL_SSD1306_PIXEL_ON;
	}
}

void L2HAL_SSD1306_DrawPixel(L2HAL_SSD1306_ContextStruct* context, uint16_t x, uint16_t y)
{
	L2HAL_SSD1306_WaitForTransferCompletion(context); /* We can't change framebuffer during transfer. */

	uint16_t index;
	uint8_t mask;

	if (false == L2HAL_SSD1306_GetFramebufferAddress(x, y, &index, &mask))
	{
		return;
	}

	uint8_t antimask = ~mask;

	context->Framebuffer[index] = (context->Framebuffer[index] & antimask) | (mask & context->ActiveColor);
}

uint16_t L2HAL_SSD1306_GetBrightness(FMGL_API_ColorStruct color)
{
	uint16_t brightness = ((uint32_t)color.R * L2HAL_SSD1306_BRIGHTNESS_MUL_R_FACTOR
			+ (uint32_t)color.G * L2HAL_SSD1306_BRIGHTNESS_MUL_G_FACTOR
			+ (uint32_t)color.B * L2HAL_SSD1306_BRIGHTNESS_MUL_B_FACTOR) / L2HAL_SSD1306_BRIGHTNESS_DIV_FACTOR;

	if (brightness > L2HAL_SSD1306_MAX_BRIGHTNESS)
	{
		brightness = L2HAL_SSD1306_MAX_BRIGHTNESS;
	}

	return brightness;
}

FMGL_API_ColorStruct L2HAL_SSD1306_GetPixel(L2HAL_SSD1306_ContextStruct* context, uint16_t x, uint16_t y)
{
	uint16_t index;
	uint8_t mask;
	FMGL_API_ColorStruct result;

	// Off color
	result.R = L2HAL_SSD1306_PIXEL_OFF_R;
	result.G = L2HAL_SSD1306_PIXEL_OFF_G;
	result.B = L2HAL_SSD1306_PIXEL_OFF_B;

	if (false == L2HAL_SSD1306_GetFramebufferAddress(x, y, &index, &mask))
	{
		return result; // Incorrect pixels are always off
	}

	if (0 != (context->Framebuffer[index] & mask))
	{
		result.R = L2HAL_SSD1306_PIXEL_ON_R;
		result.G = L2HAL_SSD1306_PIXEL_ON_G;
		result.B = L2HAL_SSD1306_PIXEL_ON_B;
	}

	return result;
}

