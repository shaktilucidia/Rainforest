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

#include "../include/l2hal_gc9a01.h"
#include "../include/l2hal_gc9a01_private.h"
#include "../../../../include/l2hal_errors.h"

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
)
{
	context->SPIHandle = spiHandle;

	context->ResetPort = resetPort;
	context->ResetPin = resetPin;

	context->DataCommandPort = dataCommandPort;
	context->DataCommandPin = dataCommandPin;

	context->ChipSelectPort = chipSelectPort;
	context->ChipSelectPin = chipSelectPin;

	/* Setting up cache */
	context->PixelsCacheY = 0;
	context->PixelsCacheX = 0;
	memset(context->PixelsCache, 0x00, L2HAL_GC9A01_CACHE_SIZE);

	context->IsDataTransferInProgress = true;

	context->FramebufferDriverContext = framebufferDriverContext;
	context->FramebufferMemoryWriteFunctionPtr = framebufferMemoryWriteFunctionPtr;
	context->FramebufferMemoryReadFunctionPtr = framebufferMemoryReadFunctionPtr;
	context->FramebufferBaseAddress = framebufferBaseAddress;

	context->PreviousFrameBufferBaseAddress = previousFrameBufferBaseAddress;

	context->CrcContext = crcContext;

	L2HAL_GC9A01_ResetDisplay(context);

	/* Initialization sequence */
	L2HAL_GC9A01_WriteCommand(context, 0xEF);

	L2HAL_GC9A01_WriteCommand(context, 0xEB);
	L2HAL_GC9A01_WriteByteData(context, 0x14);

	L2HAL_GC9A01_WriteCommand(context, 0xFE);
	L2HAL_GC9A01_WriteCommand(context, 0xEF);

	L2HAL_GC9A01_WriteCommand(context, 0xEB);
	L2HAL_GC9A01_WriteByteData(context, 0x14);

	L2HAL_GC9A01_WriteCommand(context, 0x84);
	L2HAL_GC9A01_WriteByteData(context, 0x40);

	L2HAL_GC9A01_WriteCommand(context, 0x85);
	L2HAL_GC9A01_WriteByteData(context, 0xFF);

	L2HAL_GC9A01_WriteCommand(context, 0x86);
	L2HAL_GC9A01_WriteByteData(context, 0xFF);

	L2HAL_GC9A01_WriteCommand(context, 0x87);
	L2HAL_GC9A01_WriteByteData(context, 0xFF);

	L2HAL_GC9A01_WriteCommand(context, 0x88);
	L2HAL_GC9A01_WriteByteData(context, 0x0A);

	L2HAL_GC9A01_WriteCommand(context, 0x89);
	L2HAL_GC9A01_WriteByteData(context, 0x21);

	L2HAL_GC9A01_WriteCommand(context, 0x8A);
	L2HAL_GC9A01_WriteByteData(context, 0x00);

	L2HAL_GC9A01_WriteCommand(context, 0x8B);
	L2HAL_GC9A01_WriteByteData(context, 0x80);

	L2HAL_GC9A01_WriteCommand(context, 0x8C);
	L2HAL_GC9A01_WriteByteData(context, 0x01);

	L2HAL_GC9A01_WriteCommand(context, 0x8D);
	L2HAL_GC9A01_WriteByteData(context, 0x01);

	L2HAL_GC9A01_WriteCommand(context, 0x8E);
	L2HAL_GC9A01_WriteByteData(context, 0xFF);

	L2HAL_GC9A01_WriteCommand(context, 0x8F);
	L2HAL_GC9A01_WriteByteData(context, 0xFF);


	L2HAL_GC9A01_WriteCommand(context, 0xB6);
	L2HAL_GC9A01_WriteByteData(context, 0x00);
	L2HAL_GC9A01_WriteByteData(context, 0x00);

	/* Orientation */
	L2HAL_GC9A01_WriteCommand(context, 0x36); /* Memory access control */

	/* Parameters for orientation command: MY MX MV ML BGR MH 0 0 */
	switch(orientation)
	{
		case ROTATION_0:
			L2HAL_GC9A01_WriteByteData(context, 0b00011000);
			break;

		case ROTATION_90:
			L2HAL_GC9A01_WriteByteData(context, 0b00101000);
			break;

		case ROTATION_180:
			L2HAL_GC9A01_WriteByteData(context, 0b01001000);
			break;

		case ROTATION_270:
			L2HAL_GC9A01_WriteByteData(context, 0b01101000);
			break;

		default:
			L2HAL_Error(Generic);
	}

	/* End of orientation */

	L2HAL_GC9A01_WriteCommand(context, 0x3A); /* Set color mode */
	L2HAL_GC9A01_WriteByteData(context, 0b01100110); /* 18 bits per pixel, 3 bytes exchange - RGB */

	L2HAL_GC9A01_WriteCommand(context, 0x90);
	L2HAL_GC9A01_WriteByteData(context, 0x08);
	L2HAL_GC9A01_WriteByteData(context, 0x08);
	L2HAL_GC9A01_WriteByteData(context, 0x08);
	L2HAL_GC9A01_WriteByteData(context, 0x08);

	L2HAL_GC9A01_WriteCommand(context, 0xBD);
	L2HAL_GC9A01_WriteByteData(context, 0x06);

	L2HAL_GC9A01_WriteCommand(context, 0xBC);
	L2HAL_GC9A01_WriteByteData(context, 0x00);

	L2HAL_GC9A01_WriteCommand(context, 0xFF);
	L2HAL_GC9A01_WriteByteData(context, 0x60);
	L2HAL_GC9A01_WriteByteData(context, 0x01);
	L2HAL_GC9A01_WriteByteData(context, 0x04);

	L2HAL_GC9A01_WriteCommand(context, 0xC3);
	L2HAL_GC9A01_WriteByteData(context, 0x13);
	L2HAL_GC9A01_WriteCommand(context, 0xC4);
	L2HAL_GC9A01_WriteByteData(context, 0x13);

	L2HAL_GC9A01_WriteCommand(context, 0xC9);
	L2HAL_GC9A01_WriteByteData(context, 0x22);

	L2HAL_GC9A01_WriteCommand(context, 0xBE);
	L2HAL_GC9A01_WriteByteData(context, 0x11);

	L2HAL_GC9A01_WriteCommand(context, 0xE1);
	L2HAL_GC9A01_WriteByteData(context, 0x10);
	L2HAL_GC9A01_WriteByteData(context, 0x0E);

	L2HAL_GC9A01_WriteCommand(context, 0xDF);
	L2HAL_GC9A01_WriteByteData(context, 0x21);
	L2HAL_GC9A01_WriteByteData(context, 0x0c);
	L2HAL_GC9A01_WriteByteData(context, 0x02);

	L2HAL_GC9A01_WriteCommand(context, 0xF0);
	L2HAL_GC9A01_WriteByteData(context, 0x45);
	L2HAL_GC9A01_WriteByteData(context, 0x09);
	L2HAL_GC9A01_WriteByteData(context, 0x08);
	L2HAL_GC9A01_WriteByteData(context, 0x08);
	L2HAL_GC9A01_WriteByteData(context, 0x26);
	L2HAL_GC9A01_WriteByteData(context, 0x2A);

	L2HAL_GC9A01_WriteCommand(context, 0xF1);
	L2HAL_GC9A01_WriteByteData(context, 0x43);
	L2HAL_GC9A01_WriteByteData(context, 0x70);
	L2HAL_GC9A01_WriteByteData(context, 0x72);
	L2HAL_GC9A01_WriteByteData(context, 0x36);
	L2HAL_GC9A01_WriteByteData(context, 0x37);
	L2HAL_GC9A01_WriteByteData(context, 0x6F);

	L2HAL_GC9A01_WriteCommand(context, 0xF2);
	L2HAL_GC9A01_WriteByteData(context, 0x45);
	L2HAL_GC9A01_WriteByteData(context, 0x09);
	L2HAL_GC9A01_WriteByteData(context, 0x08);
	L2HAL_GC9A01_WriteByteData(context, 0x08);
	L2HAL_GC9A01_WriteByteData(context, 0x26);
	L2HAL_GC9A01_WriteByteData(context, 0x2A);

	L2HAL_GC9A01_WriteCommand(context, 0xF3);
	L2HAL_GC9A01_WriteByteData(context, 0x43);
	L2HAL_GC9A01_WriteByteData(context, 0x70);
	L2HAL_GC9A01_WriteByteData(context, 0x72);
	L2HAL_GC9A01_WriteByteData(context, 0x36);
	L2HAL_GC9A01_WriteByteData(context, 0x37);
	L2HAL_GC9A01_WriteByteData(context, 0x6F);

	L2HAL_GC9A01_WriteCommand(context, 0xED);
	L2HAL_GC9A01_WriteByteData(context, 0x1B);
	L2HAL_GC9A01_WriteByteData(context, 0x0B);

	L2HAL_GC9A01_WriteCommand(context, 0xAE);
	L2HAL_GC9A01_WriteByteData(context, 0x77);

	L2HAL_GC9A01_WriteCommand(context, 0xCD);
	L2HAL_GC9A01_WriteByteData(context, 0x63);

	L2HAL_GC9A01_WriteCommand(context, 0x70);
	L2HAL_GC9A01_WriteByteData(context, 0x07);
	L2HAL_GC9A01_WriteByteData(context, 0x07);
	L2HAL_GC9A01_WriteByteData(context, 0x04);
	L2HAL_GC9A01_WriteByteData(context, 0x0E);
	L2HAL_GC9A01_WriteByteData(context, 0x0F);
	L2HAL_GC9A01_WriteByteData(context, 0x09);
	L2HAL_GC9A01_WriteByteData(context, 0x07);
	L2HAL_GC9A01_WriteByteData(context, 0x08);
	L2HAL_GC9A01_WriteByteData(context, 0x03);

	L2HAL_GC9A01_WriteCommand(context, 0xE8);
	L2HAL_GC9A01_WriteByteData(context, 0x34);

	L2HAL_GC9A01_WriteCommand(context, 0x62);
	L2HAL_GC9A01_WriteByteData(context, 0x18);
	L2HAL_GC9A01_WriteByteData(context, 0x0D);
	L2HAL_GC9A01_WriteByteData(context, 0x71);
	L2HAL_GC9A01_WriteByteData(context, 0xED);
	L2HAL_GC9A01_WriteByteData(context, 0x70);
	L2HAL_GC9A01_WriteByteData(context, 0x70);
	L2HAL_GC9A01_WriteByteData(context, 0x18);
	L2HAL_GC9A01_WriteByteData(context, 0x0F);
	L2HAL_GC9A01_WriteByteData(context, 0x71);
	L2HAL_GC9A01_WriteByteData(context, 0xEF);
	L2HAL_GC9A01_WriteByteData(context, 0x70);
	L2HAL_GC9A01_WriteByteData(context, 0x70);

	L2HAL_GC9A01_WriteCommand(context, 0x63);
	L2HAL_GC9A01_WriteByteData(context, 0x18);
	L2HAL_GC9A01_WriteByteData(context, 0x11);
	L2HAL_GC9A01_WriteByteData(context, 0x71);
	L2HAL_GC9A01_WriteByteData(context, 0xF1);
	L2HAL_GC9A01_WriteByteData(context, 0x70);
	L2HAL_GC9A01_WriteByteData(context, 0x70);
	L2HAL_GC9A01_WriteByteData(context, 0x18);
	L2HAL_GC9A01_WriteByteData(context, 0x13);
	L2HAL_GC9A01_WriteByteData(context, 0x71);
	L2HAL_GC9A01_WriteByteData(context, 0xF3);
	L2HAL_GC9A01_WriteByteData(context, 0x70);
	L2HAL_GC9A01_WriteByteData(context, 0x70);

	L2HAL_GC9A01_WriteCommand(context, 0x64);
	L2HAL_GC9A01_WriteByteData(context, 0x28);
	L2HAL_GC9A01_WriteByteData(context, 0x29);
	L2HAL_GC9A01_WriteByteData(context, 0xF1);
	L2HAL_GC9A01_WriteByteData(context, 0x01);
	L2HAL_GC9A01_WriteByteData(context, 0xF1);
	L2HAL_GC9A01_WriteByteData(context, 0x00);
	L2HAL_GC9A01_WriteByteData(context, 0x07);

	L2HAL_GC9A01_WriteCommand(context, 0x66);
	L2HAL_GC9A01_WriteByteData(context, 0x3C);
	L2HAL_GC9A01_WriteByteData(context, 0x00);
	L2HAL_GC9A01_WriteByteData(context, 0xCD);
	L2HAL_GC9A01_WriteByteData(context, 0x67);
	L2HAL_GC9A01_WriteByteData(context, 0x45);
	L2HAL_GC9A01_WriteByteData(context, 0x45);
	L2HAL_GC9A01_WriteByteData(context, 0x10);
	L2HAL_GC9A01_WriteByteData(context, 0x00);
	L2HAL_GC9A01_WriteByteData(context, 0x00);
	L2HAL_GC9A01_WriteByteData(context, 0x00);

	L2HAL_GC9A01_WriteCommand(context, 0x67);
	L2HAL_GC9A01_WriteByteData(context, 0x00);
	L2HAL_GC9A01_WriteByteData(context, 0x3C);
	L2HAL_GC9A01_WriteByteData(context, 0x00);
	L2HAL_GC9A01_WriteByteData(context, 0x00);
	L2HAL_GC9A01_WriteByteData(context, 0x00);
	L2HAL_GC9A01_WriteByteData(context, 0x01);
	L2HAL_GC9A01_WriteByteData(context, 0x54);
	L2HAL_GC9A01_WriteByteData(context, 0x10);
	L2HAL_GC9A01_WriteByteData(context, 0x32);
	L2HAL_GC9A01_WriteByteData(context, 0x98);

	L2HAL_GC9A01_WriteCommand(context, 0x74);
	L2HAL_GC9A01_WriteByteData(context, 0x10);
	L2HAL_GC9A01_WriteByteData(context, 0x85);
	L2HAL_GC9A01_WriteByteData(context, 0x80);
	L2HAL_GC9A01_WriteByteData(context, 0x00);
	L2HAL_GC9A01_WriteByteData(context, 0x00);
	L2HAL_GC9A01_WriteByteData(context, 0x4E);
	L2HAL_GC9A01_WriteByteData(context, 0x00);

	L2HAL_GC9A01_WriteCommand(context, 0x98);
	L2HAL_GC9A01_WriteByteData(context, 0x3e);
	L2HAL_GC9A01_WriteByteData(context, 0x07);

	L2HAL_GC9A01_WriteCommand(context, 0x35);
	L2HAL_GC9A01_WriteCommand(context, 0x21);

	L2HAL_GC9A01_WriteCommand(context, 0x11);
	HAL_Delay(L2HAL_GC9A01_DISPLAY_SLEEP_OUT_TIME);

	L2HAL_GC9A01_WriteCommand(context, 0x29);
	HAL_Delay(L2HAL_GC9A01_DISPLAY_AFTER_ON_TIME);

	/* Active color is black */
	FMGL_API_ColorStruct blackColor;
	blackColor.R = 0x00;
	blackColor.G = 0x00;
	blackColor.B = 0x00;
	L2HAL_GC9A01_SetActiveColor(context, blackColor);

	/* Filling display with black */
	L2HAL_GC9A01_ClearDisplay(context);
}

void L2HAL_GC9A01_SelectChip(L2HAL_GC9A01_ContextStruct *context, bool isSelected)
{
	if (isSelected)
	{
		HAL_GPIO_WritePin(context->ChipSelectPort, context->ChipSelectPin, GPIO_PIN_RESET); /* Active low*/
	}
	else
	{
		HAL_GPIO_WritePin(context->ChipSelectPort, context->ChipSelectPin, GPIO_PIN_SET);
	}
}

void L2HAL_GC9A01_ResetDisplay(L2HAL_GC9A01_ContextStruct *context)
{
	L2HAL_GC9A01_SelectChip(context, false);
	HAL_Delay(L2HAL_GC9A01_CS_TO_RESET);

	HAL_GPIO_WritePin(context->ResetPort, context->ResetPin, GPIO_PIN_RESET);
	HAL_Delay(L2HAL_GC9A01_RESET_DURATION);

	HAL_GPIO_WritePin(context->ResetPort, context->ResetPin, GPIO_PIN_SET);
	HAL_Delay(L2HAL_GC9A01_DISPLAY_BOOT_TIME);
}

void L2HAL_GC9A01_WriteCommand(L2HAL_GC9A01_ContextStruct *context, uint8_t command)
{
	context->IsDataTransferInProgress = true;

	HAL_GPIO_WritePin(context->DataCommandPort, context->DataCommandPin, GPIO_PIN_RESET); /* 0 - Command */

	L2HAL_GC9A01_SelectChip(context, true);

	if (HAL_SPI_Transmit_DMA(context->SPIHandle, &command, 1) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_GC9A01_WaitForDataTransferCompletion(context);

	L2HAL_GC9A01_SelectChip(context, false);
}

void L2HAL_GC9A01_WriteData(L2HAL_GC9A01_ContextStruct *context, uint8_t *data, uint16_t dataSize)
{
	context->IsDataTransferInProgress = true;

	HAL_GPIO_WritePin(context->DataCommandPort, context->DataCommandPin, GPIO_PIN_SET); /* 1 - Data */

	L2HAL_GC9A01_SelectChip(context, true);

	if (HAL_SPI_Transmit_DMA(context->SPIHandle, data, dataSize) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_GC9A01_WaitForDataTransferCompletion(context);

	L2HAL_GC9A01_SelectChip(context, false);
}

void L2HAL_GC9A01_WriteByteData(L2HAL_GC9A01_ContextStruct *context, uint8_t byteData)
{
	L2HAL_GC9A01_WriteData(context, &byteData, 1);
}

void L2HAL_GC9A01_SetColumnsRange(L2HAL_GC9A01_ContextStruct *context, uint16_t colStart, uint16_t colEnd)
{
    uint8_t data[4];

    L2HAL_GC9A01_WriteCommand(context, 0x2A); /* Column address */
    data[0] = (colStart >> 8) & 0xFFU;
    data[1] = colStart & 0xFFU;

    data[2] = (colEnd >> 8) & 0xFFU;
    data[3] = colEnd & 0xFFU;

    L2HAL_GC9A01_WriteData(context, data, 4);
}

void L2HAL_GC9A01_SetRowsRange(L2HAL_GC9A01_ContextStruct *context, uint16_t rowStart, uint16_t rowEnd)
{
	uint8_t data[4];

	L2HAL_GC9A01_WriteCommand(context, 0x2B); /* Row address */
	data[0] = (rowStart >> 8) & 0xFFU;
	data[1] = rowStart & 0xFFU;

	data[2] = (rowEnd >> 8) & 0xFFU;
	data[3] = rowEnd & 0xFFU;

	L2HAL_GC9A01_WriteData(context, data, 4);
}

uint16_t L2HAL_GC9A01_GetWidth(void)
{
	return L2HAL_GC9A01_DISPLAY_WIDTH;
}

uint16_t L2HAL_GC9A01_GetHeight(void)
{
	return L2HAL_GC9A01_DISPLAY_HEIGHT;
}

void L2HAL_GC9A01_ClearDisplay(L2HAL_GC9A01_ContextStruct *context)
{
	L2HAL_GC9A01_ClearFramebuffer(context);
	L2HAL_GC9A01_ForcePushFramebuffer(context);
}

void L2HAL_GC9A01_DrawPixel(L2HAL_GC9A01_ContextStruct* context, uint16_t x, uint16_t y)
{
	if (L2HAL_GC9A01_IsPixelsCacheHit(context, x, y))
	{
		/* Cache hit */
		L2HAL_GC9A01_WritePixelsCache(context, x, y, context->ActiveColor);
		return;
	}

	/* Cache miss, pushing cache to external memory */
	L2HAL_GC9A01_WriteCacheToFramebuffer(context);

	/* Moving cache to new position */
	context->PixelsCacheX = x;
	context->PixelsCacheY = y;
	L2HAL_GC9A01_ReadCacheFromFramebuffer(context);

	/* Writing new pixel to cache */
	L2HAL_GC9A01_WritePixelsCache(context, x, y, context->ActiveColor);
}

bool L2HAL_GC9A01_IsPixelsCacheHit(L2HAL_GC9A01_ContextStruct *context, uint16_t x, uint16_t y)
{
	return
		x >= context->PixelsCacheX
		&&
		x < context->PixelsCacheX + L2HAL_GC9A01_CACHE_WIDTH
		&&
		y >= context->PixelsCacheY
		&&
		y < context->PixelsCacheY + L2HAL_GC9A01_CACHE_HEIGHT;
}

FMGL_API_ColorStruct L2HAL_GC9A01_ReadPixelsCache(L2HAL_GC9A01_ContextStruct *context, uint16_t x, uint16_t y)
{
	uint16_t cacheLine = y - context->PixelsCacheY;
	uint16_t cacheIndex = (cacheLine * L2HAL_GC9A01_CACHE_WIDTH + x - context->PixelsCacheX) * 3;

	FMGL_API_ColorStruct result;
	result.R = context->PixelsCache[cacheIndex + 0]; // We have RGB sequence
	result.G = context->PixelsCache[cacheIndex + 1];
	result.B = context->PixelsCache[cacheIndex + 2];

	return result;
}

void L2HAL_GC9A01_WritePixelsCache(L2HAL_GC9A01_ContextStruct *context, uint16_t x, uint16_t y, FMGL_API_ColorStruct color)
{
	uint16_t cacheLine = y - context->PixelsCacheY;
	uint16_t cacheIndex = (cacheLine * L2HAL_GC9A01_CACHE_WIDTH + x - context->PixelsCacheX) * 3;

	context->PixelsCache[cacheIndex + 0] = color.R;
	context->PixelsCache[cacheIndex + 1] = color.G;
	context->PixelsCache[cacheIndex + 2] = color.B;
}

void L2HAL_GC9A01_SetActiveColor(L2HAL_GC9A01_ContextStruct* context, FMGL_API_ColorStruct color)
{
	context->ActiveColor = color;
}

FMGL_API_ColorStruct L2HAL_GC9A01_GetPixel(L2HAL_GC9A01_ContextStruct* context, uint16_t x, uint16_t y)
{
	if (L2HAL_GC9A01_IsPixelsCacheHit(context, x, y))
	{
		/* Cache hit */
		return L2HAL_GC9A01_ReadPixelsCache(context, x, y);
	}
	else
	{
		/* Cache miss, we need to download cache from external memory */
		L2HAL_GC9A01_ReadCacheFromFramebuffer(context);

		return L2HAL_GC9A01_ReadPixelsCache(context, x, y);
	}
}

void L2HAL_GC9A01_PushFramebuffer(L2HAL_GC9A01_ContextStruct* context)
{
	L2HAL_GC9A01_WriteCacheToFramebuffer(context);

	uint8_t lineBuffer[L2HAL_GC9A01_DISPLAY_LINE_SIZE];
	uint32_t previousFrameLineBuffer[L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_BLOCKS_COUNT];

	for (uint16_t y = 0; y < L2HAL_GC9A01_DISPLAY_HEIGHT; y ++)
	{
		context->FramebufferMemoryReadFunctionPtr(context->FramebufferDriverContext, context->FramebufferBaseAddress + y * L2HAL_GC9A01_DISPLAY_LINE_SIZE, L2HAL_GC9A01_DISPLAY_LINE_SIZE, lineBuffer);
		context->FramebufferMemoryReadFunctionPtr(context->FramebufferDriverContext, context->PreviousFrameBufferBaseAddress + y * L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_SIZE, L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_SIZE, (uint8_t*)previousFrameLineBuffer);

		for (uint16_t blockNumber = 0; blockNumber < L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_BLOCKS_COUNT; blockNumber ++)
		{
			/* Calculating CRC for pixels block came from framebuffer */
			uint32_t framebufferBlockCrc = L2HAL_CRC_Calculate(context->CrcContext, &lineBuffer[blockNumber * L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH * 3], L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH * 3);

			if (framebufferBlockCrc != previousFrameLineBuffer[blockNumber])
			{
				uint16_t blockFirstPixel = blockNumber * L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH;
				uint8_t pixelsCountToTransmit = L2HAL_GC9A01_DISPLAY_WIDTH - blockFirstPixel;
				if (pixelsCountToTransmit > L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH)
				{
					pixelsCountToTransmit = L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH;
				}

				/* Transmitting */
				L2HAL_GC9A01_SetColumnsRange(context, blockFirstPixel, blockFirstPixel + pixelsCountToTransmit - 1);
				L2HAL_GC9A01_SetRowsRange(context, y, y);

				L2HAL_GC9A01_WriteCommand(context, 0x2C);
				L2HAL_GC9A01_WriteData(context, &lineBuffer[blockFirstPixel * 3], pixelsCountToTransmit * 3);
			}

			/* Refreshing previous frame buffer */
			previousFrameLineBuffer[blockNumber] = framebufferBlockCrc;
		}

		context->FramebufferMemoryWriteFunctionPtr(context->FramebufferDriverContext, context->PreviousFrameBufferBaseAddress + y * L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_SIZE, L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_SIZE, (uint8_t*)previousFrameLineBuffer);

//		/* Searching for dirty (e.g. changed) pixels */
//		uint16_t x = 0;
//		while (x < L2HAL_GC9A01_DISPLAY_WIDTH)
//		{
//			uint16_t currentPixelBaseIndex = x * 3;
//
//			if
//			(
//				previousFrameLineBuffer[currentPixelBaseIndex] != lineBuffer[currentPixelBaseIndex]
//				||
//				previousFrameLineBuffer[currentPixelBaseIndex + 1] != lineBuffer[currentPixelBaseIndex + 1]
//				||
//				previousFrameLineBuffer[currentPixelBaseIndex + 2] != lineBuffer[currentPixelBaseIndex + 2]
//			)
//			{
//				uint8_t pixelsCountToTransmit = L2HAL_GC9A01_DISPLAY_WIDTH - x;
//				if (pixelsCountToTransmit > L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH)
//				{
//					pixelsCountToTransmit = L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH;
//				}
//
//				/* Transmitting */
//				L2HAL_GC9A01_SetColumnsRange(context, x, x + pixelsCountToTransmit - 1);
//				L2HAL_GC9A01_SetRowsRange(context, y, y);
//
//				L2HAL_GC9A01_WriteCommand(context, 0x2C);
//				L2HAL_GC9A01_WriteData(context, &lineBuffer[x * 3], pixelsCountToTransmit * 3);
//
//
//				x += pixelsCountToTransmit;
//			}
//			else
//			{
//				x += 1;
//			}
//		}
//
//		/* Refreshing previous frame buffer */
//		context->FramebufferMemoryWriteFunctionPtr(context->FramebufferDriverContext, context->PreviousFrameBufferBaseAddress + y * L2HAL_GC9A01_DISPLAY_LINE_SIZE, L2HAL_GC9A01_DISPLAY_LINE_SIZE, lineBuffer);
	}
}

//void L2HAL_GC9A01_PushFramebuffer(L2HAL_GC9A01_ContextStruct* context)
//{
//	L2HAL_GC9A01_WriteCacheToFramebuffer(context);
//
//	uint8_t lineBuffer[L2HAL_GC9A01_DISPLAY_LINE_SIZE];
//
//	for (uint16_t y = 0; y < L2HAL_GC9A01_DISPLAY_HEIGHT; y ++)
//	{
//		context->FramebufferMemoryReadFunctionPtr(context->FramebufferDriverContext, context->FramebufferBaseAddress + y * L2HAL_GC9A01_DISPLAY_LINE_SIZE, L2HAL_GC9A01_DISPLAY_LINE_SIZE, lineBuffer);
//
//		/* Transmitting */
//		L2HAL_GC9A01_SetColumnsRange(context, 0, L2HAL_GC9A01_DISPLAY_WIDTH - 1);
//		L2HAL_GC9A01_SetRowsRange(context, y, y);
//
//		L2HAL_GC9A01_WriteCommand(context, 0x2C);
//		L2HAL_GC9A01_WriteData(context, lineBuffer, L2HAL_GC9A01_DISPLAY_LINE_SIZE);
//	}
//}

void L2HAL_GC9A01_ForcePushFramebuffer(L2HAL_GC9A01_ContextStruct* context)
{
	L2HAL_GC9A01_WriteCacheToFramebuffer(context);

	uint8_t lineBuffer[L2HAL_GC9A01_DISPLAY_LINE_SIZE];
	uint32_t previousFrameLineBuffer[L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_BLOCKS_COUNT];

	for (uint16_t y = 0; y < L2HAL_GC9A01_DISPLAY_HEIGHT; y ++)
	{
		context->FramebufferMemoryReadFunctionPtr(context->FramebufferDriverContext, context->FramebufferBaseAddress + y * L2HAL_GC9A01_DISPLAY_LINE_SIZE, L2HAL_GC9A01_DISPLAY_LINE_SIZE, lineBuffer);

		/* Transmitting */
		L2HAL_GC9A01_SetColumnsRange(context, 0, L2HAL_GC9A01_DISPLAY_WIDTH - 1);
		L2HAL_GC9A01_SetRowsRange(context, y, y);

		L2HAL_GC9A01_WriteCommand(context, 0x2C);
		L2HAL_GC9A01_WriteData(context, lineBuffer, L2HAL_GC9A01_DISPLAY_LINE_SIZE);

		/* Refreshing previous frame buffer */
		for (uint16_t blockNumber = 0; blockNumber < L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_BLOCKS_COUNT; blockNumber ++)
		{
			previousFrameLineBuffer[blockNumber] = L2HAL_CRC_Calculate(context->CrcContext, &lineBuffer[blockNumber * L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH * 3], L2HAL_GC9A01_DIRTY_PIXELS_TRANSMISSION_LENGTH * 3);
		}
		context->FramebufferMemoryWriteFunctionPtr(context->FramebufferDriverContext, context->PreviousFrameBufferBaseAddress + y * L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_SIZE, L2HAL_GC9A01_DIRTY_PIXELS_BUFFER_LINE_SIZE, (uint8_t*)previousFrameLineBuffer);
	}
}

void L2HAL_GC9A01_WaitForDataTransferCompletion(L2HAL_GC9A01_ContextStruct *context)
{
	while (context->IsDataTransferInProgress) {} /* First wait for DMA completion */
	while (HAL_SPI_GetState(context->SPIHandle) != HAL_SPI_STATE_READY) { } /* Then wait for SPI ready*/
}

void L2HAL_GC9A01_MarkDataTransferAsCompleted(L2HAL_GC9A01_ContextStruct *context)
{
	context->IsDataTransferInProgress = false;
}

void L2HAL_GC9A01_WriteCacheToFramebuffer(L2HAL_GC9A01_ContextStruct *context)
{
	for(uint16_t cacheY = 0; cacheY < L2HAL_GC9A01_CACHE_HEIGHT; cacheY++)
	{
		uint8_t* cacheLineBaseAddress = &context->PixelsCache[cacheY * L2HAL_GC9A01_CACHE_LINE_SIZE];
		uint32_t framebufferWriteBaseAddress = context->FramebufferBaseAddress + 3 * ((context->PixelsCacheY + cacheY) * L2HAL_GC9A01_DISPLAY_WIDTH + context->PixelsCacheX);

		context->FramebufferMemoryWriteFunctionPtr(context->FramebufferDriverContext, framebufferWriteBaseAddress, L2HAL_GC9A01_CACHE_LINE_SIZE, cacheLineBaseAddress);
	}
}

void L2HAL_GC9A01_ReadCacheFromFramebuffer(L2HAL_GC9A01_ContextStruct *context)
{
	for(uint16_t cacheY = 0; cacheY < L2HAL_GC9A01_CACHE_HEIGHT; cacheY++)
	{
		uint8_t* cacheLineBaseAddress = &context->PixelsCache[cacheY * L2HAL_GC9A01_CACHE_LINE_SIZE];
		uint32_t framebufferWriteBaseAddress = context->FramebufferBaseAddress + 3 * ((context->PixelsCacheY + cacheY) * L2HAL_GC9A01_DISPLAY_WIDTH + context->PixelsCacheX);

		context->FramebufferMemoryReadFunctionPtr(context->FramebufferDriverContext, framebufferWriteBaseAddress, L2HAL_GC9A01_CACHE_LINE_SIZE, cacheLineBaseAddress);
	}
}

void L2HAL_GC9A01_ClearFramebuffer(L2HAL_GC9A01_ContextStruct* context)
{
	uint8_t lineBuffer[L2HAL_GC9A01_DISPLAY_LINE_SIZE];
	memset(lineBuffer, 0x00, L2HAL_GC9A01_DISPLAY_LINE_SIZE);

	for (uint16_t y = 0; y < L2HAL_GC9A01_DISPLAY_HEIGHT; y++)
	{
		context->FramebufferMemoryWriteFunctionPtr(context->FramebufferDriverContext, context->FramebufferBaseAddress + y * L2HAL_GC9A01_DISPLAY_LINE_SIZE, L2HAL_GC9A01_DISPLAY_LINE_SIZE, lineBuffer);
	}
}

void L2HAL_GC9A01_SetFramebufferBaseAddress(L2HAL_GC9A01_ContextStruct* context, uint32_t baseAddress)
{
	L2HAL_GC9A01_WriteCacheToFramebuffer(context);

	context->FramebufferBaseAddress = baseAddress;

	context->PixelsCacheX = 0;
	context->PixelsCacheY = 0;
	L2HAL_GC9A01_ReadCacheFromFramebuffer(context);
}
