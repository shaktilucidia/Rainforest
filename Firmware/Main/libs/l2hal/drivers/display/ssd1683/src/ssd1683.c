/*
 * ssd1683.c
 *
 *  Created on: Apr 11, 2025
 *      Author: shakti
 */

#include "../include/ssd1683.h"
#include "../include/ssd1683_private.h"

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
)
{
	context->SPIHandle = spiHandle;

	context->BusyPort = busyPort;
	context->BusyPin = busyPin;

	context->ResetPort = resetPort;
	context->ResetPin = resetPin;

	context->DataCommandPort = dataCommandPort;
	context->DataCommandPin = dataCommandPin;

	context->ChipSelectPort = chipSelectPort;
	context->ChipSelectPin = chipSelectPin;

	context->IsDataTransferInProgress = true;

	/* Auto full refresh */
	context->IsAutoFullRefresh = isAutoFullRefresh;
	context->AutoFullRefreshFramesCount = autoFullRefreshFramesCount;
	context->FramesTillFullRefresh = autoFullRefreshFramesCount - 1;

	L2HAL_SSD1683_ResetDisplay(context);

	L2HAL_SSD1683_WaitForReadiness(context);

	L2HAL_SSD1683_WriteCommand(context, 0x12); /* Software reset */
	HAL_Delay(L2HAL_SSD1683_DISPLAY_BOOT_TIME);
	L2HAL_SSD1683_WaitForReadiness(context);

	L2HAL_SSD1683_WriteCommand(context, 0x01); /* Set MUX as 300 */
	L2HAL_SSD1683_WriteDataByte(context, 0x2B);
	L2HAL_SSD1683_WriteDataByte(context, 0x01);
	L2HAL_SSD1683_WriteDataByte(context, 0x00);


	L2HAL_SSD1683_WriteCommand(context, 0x3C); /* Border waveform */
	L2HAL_SSD1683_WriteDataByte(context, 0x01);

	L2HAL_SSD1683_WriteCommand(context, 0x18); /* Read built-in temperature sensor */
	L2HAL_SSD1683_WriteDataByte(context, 0x80);


	L2HAL_SSD1683_WriteCommand(context, 0x11); /* Data entry mode */
	L2HAL_SSD1683_WriteDataByte(context, 0x03); /* X-mode */

	/* Power on */
	L2HAL_SSD1683_PowerOn(context);

	/* Initial position */
	L2HAL_SSD1683_SetRange(context, 0, 0, L2HAL_SSD1683_DISPLAY_WIDTH, L2HAL_SSD1683_DISPLAY_HEIGHT);
	L2HAL_SSD1683_SetPosition(context, 0, 0);

	/* Cleanup */
	L2HAL_SSD1683_ClearFramebuffer(context, initColor);
	L2HAL_SSD1683_PushFramebufferFull(context);
}

void L2HAL_SSD1683_ResetDisplay(L2HAL_SSD1683_ContextStruct *context)
{
	L2HAL_SSD1683_SelectChip(context, false);
	HAL_Delay(L2HAL_SSD1683_CS_TO_RESET);

	HAL_GPIO_WritePin(context->ResetPort, context->ResetPin, GPIO_PIN_RESET);
	HAL_Delay(L2HAL_SSD1683_RESET_DURATION);

	HAL_GPIO_WritePin(context->ResetPort, context->ResetPin, GPIO_PIN_SET);
	HAL_Delay(L2HAL_SSD1683_DISPLAY_BOOT_TIME);
}

void L2HAL_SSD1683_SelectChip(L2HAL_SSD1683_ContextStruct *context, bool isSelected)
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

void L2HAL_SSD1683_WaitForReadiness(L2HAL_SSD1683_ContextStruct *context)
{
	while (GPIO_PIN_SET == HAL_GPIO_ReadPin(context->BusyPort, context->BusyPin)) {}
}

/**
 * Hang till the DMA transfer completed
 */
void L2HAL_SSD1683_WaitForDataTransferCompletion(L2HAL_SSD1683_ContextStruct *context)
{
	while (context->IsDataTransferInProgress) {} /* First wait for DMA completion */
	while (HAL_SPI_GetState(context->SPIHandle) != HAL_SPI_STATE_READY) { } /* Then wait for SPI ready*/
}

/**
 * Mark transfer as completed (call it from DMA IRQ)
 */
void L2HAL_SSD1683_MarkDataTransferAsCompleted(L2HAL_SSD1683_ContextStruct *context)
{
	context->IsDataTransferInProgress = false;
}

/**
 * Write command to display
 */
void L2HAL_SSD1683_WriteCommand(L2HAL_SSD1683_ContextStruct *context, uint8_t command)
{
	context->IsDataTransferInProgress = true;

	HAL_GPIO_WritePin(context->DataCommandPort, context->DataCommandPin, GPIO_PIN_RESET); /* 0 - Command */

	L2HAL_SSD1683_SelectChip(context, true);

	if (HAL_SPI_Transmit_DMA(context->SPIHandle, &command, 1) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_SSD1683_WaitForDataTransferCompletion(context);

	L2HAL_SSD1683_SelectChip(context, false);
}

/**
 * Write data to display
 */
void L2HAL_SSD1683_WriteData(L2HAL_SSD1683_ContextStruct *context, uint8_t *data, uint16_t dataSize)
{
	context->IsDataTransferInProgress = true;

	HAL_GPIO_WritePin(context->DataCommandPort, context->DataCommandPin, GPIO_PIN_SET); /* 1 - Data */

	L2HAL_SSD1683_SelectChip(context, true);

	if (HAL_SPI_Transmit_DMA(context->SPIHandle, data, dataSize) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_SSD1683_WaitForDataTransferCompletion(context);

	L2HAL_SSD1683_SelectChip(context, false);
}

/**
 * Write 1 byte of data
 */
void L2HAL_SSD1683_WriteDataByte(L2HAL_SSD1683_ContextStruct *context, uint8_t data)
{
	L2HAL_SSD1683_WriteData(context, &data, 1);
}

/**
 * Set output position
 */
void L2HAL_SSD1683_SetPosition(L2HAL_SSD1683_ContextStruct *context, uint16_t x, uint16_t y)
{
	L2HAL_SSD1683_WriteCommand(context, 0x4E);
	L2HAL_SSD1683_WriteDataByte(context, x >> 3);

	L2HAL_SSD1683_WriteCommand(context, 0x4F);
	L2HAL_SSD1683_WriteDataByte(context, y & 0xFF);
	L2HAL_SSD1683_WriteDataByte(context, (y >> 8) & 0x01);
}

void L2HAL_SSD1683_SetRange(L2HAL_SSD1683_ContextStruct *context, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	L2HAL_SSD1683_WriteCommand(context, 0x11); // set ram entry mode
	L2HAL_SSD1683_WriteDataByte(context, 0x03);    // x increase, y increase : normal mode

	L2HAL_SSD1683_WriteCommand(context, 0x44);
	L2HAL_SSD1683_WriteDataByte(context, x / 8);
	L2HAL_SSD1683_WriteDataByte(context, (x + width - 1) / 8);

	L2HAL_SSD1683_WriteCommand(context, 0x45);
	L2HAL_SSD1683_WriteDataByte(context, y % 256);
	L2HAL_SSD1683_WriteDataByte(context, y / 256);
	L2HAL_SSD1683_WriteDataByte(context, (y + height - 1) % 256);
	L2HAL_SSD1683_WriteDataByte(context, (y + height - 1) / 256);

	L2HAL_SSD1683_WriteCommand(context, 0x4E);
	L2HAL_SSD1683_WriteDataByte(context, x / 8);

	L2HAL_SSD1683_WriteCommand(context, 0x4F);
	L2HAL_SSD1683_WriteDataByte(context, y % 256);
	L2HAL_SSD1683_WriteDataByte(context, y / 256);
}

void L2HAL_SSD1683_Update(L2HAL_SSD1683_ContextStruct *context)
{
	L2HAL_SSD1683_WriteCommand(context, 0x22);
	L2HAL_SSD1683_WriteDataByte(context, 0xF7);

	L2HAL_SSD1683_WriteCommand(context, 0x20);

	L2HAL_SSD1683_WaitForReadiness(context);
}

/**
 * Update display partially (not spatial, but "weak update")
 */
void L2HAL_SSD1683_PartialUpdate(L2HAL_SSD1683_ContextStruct *context)
{
	L2HAL_SSD1683_WriteCommand(context, 0x22);
	L2HAL_SSD1683_WriteDataByte(context, 0xFF);

	L2HAL_SSD1683_WriteCommand(context, 0x20);

	L2HAL_SSD1683_WaitForReadiness(context);
}

/**
 * Get display width
 */
uint16_t L2HAL_SSD1683_GetWidth(void)
{
	return L2HAL_SSD1683_DISPLAY_WIDTH;
}

/**
 * Get display height
 */
uint16_t L2HAL_SSD1683_GetHeight(void)
{
	return L2HAL_SSD1683_DISPLAY_HEIGHT;
}

/**
 * Set color what will be used for drawing.
 */
void L2HAL_SSD1683_SetActiveColor(L2HAL_SSD1683_ContextStruct* context, FMGL_API_ColorStruct color)
{
	context->BinarizedActiveColor = L2HAL_SSD1683_BinarizeColor(color);
}

/**
 * If color is not fully black will return true
 */
uint8_t L2HAL_SSD1683_BinarizeColor(FMGL_API_ColorStruct color)
{
	return (0x00 != (color.R | color.G | color.B)) ? 0xFF : 0x00;
}

/**
 * Fill framebuffer with active color
 */
void L2HAL_SSD1683_ClearFramebuffer(L2HAL_SSD1683_ContextStruct* context, FMGL_API_ColorStruct clearColor)
{
	memset(context->Framebuffer, L2HAL_SSD1683_BinarizeColor(clearColor), L2HAL_SSD1683_FRAMEBUFFER_SIZE);
}

/**
 * Push framebuffer to display
 */
void L2HAL_SSD1683_PushFramebufferFull(L2HAL_SSD1683_ContextStruct* context)
{
	L2HAL_SSD1683_PushFramebufferInternal(context, 0x26);
	L2HAL_SSD1683_PushFramebufferInternal(context, 0x24);

	L2HAL_SSD1683_Update(context);
}

/**
 * Push framebuffer to display (partial update)
 */
void L2HAL_SSD1683_PushFramebufferPartial(L2HAL_SSD1683_ContextStruct* context)
{
	if (context->IsAutoFullRefresh)
	{
		if (context->FramesTillFullRefresh > 0)
		{
			context->FramesTillFullRefresh --;

			L2HAL_SSD1683_PushFramebufferInternal(context, 0x24);
			L2HAL_SSD1683_PartialUpdate(context);
		}
		else
		{
			context->FramesTillFullRefresh = context->AutoFullRefreshFramesCount - 1;

			L2HAL_SSD1683_PushFramebufferFull(context);
		}
	}
	else
	{
		L2HAL_SSD1683_PushFramebufferInternal(context, 0x24);
		L2HAL_SSD1683_PartialUpdate(context);
	}
}

/**
* Push framebuffer with given command (internal use only)
*/
void L2HAL_SSD1683_PushFramebufferInternal(L2HAL_SSD1683_ContextStruct* context, uint8_t command)
{
	L2HAL_SSD1683_SetRange(context, 0, 0, L2HAL_SSD1683_DISPLAY_WIDTH, L2HAL_SSD1683_DISPLAY_HEIGHT);
	L2HAL_SSD1683_WriteCommand(context, command);
	L2HAL_SSD1683_WriteData(context, context->Framebuffer, L2HAL_SSD1683_DISPLAY_LINE_SIZE * L2HAL_SSD1683_DISPLAY_HEIGHT);
}

/**
 * Draw pixel
 */
void L2HAL_SSD1683_DrawPixel(L2HAL_SSD1683_ContextStruct* context, uint16_t x, uint16_t y)
{
	uint32_t index = y * L2HAL_SSD1683_DISPLAY_LINE_SIZE + (x >> 3);

	uint8_t bitNumber = 7 - (x % 8);

	uint8_t mask = (1 << bitNumber);
	uint8_t antimask = ~(1 << bitNumber);

	context->Framebuffer[index] = (context->Framebuffer[index] & antimask) | (mask & context->BinarizedActiveColor);
}

/**
 * Get pixel color
 */
FMGL_API_ColorStruct L2HAL_SSD1683_GetPixel(L2HAL_SSD1683_ContextStruct* context, uint16_t x, uint16_t y)
{
	uint32_t index = (y * L2HAL_SSD1683_DISPLAY_LINE_SIZE + (x >> 3));

	uint8_t bitNumber = x % 8;

	return (0 != (context->Framebuffer[index] & (1 << bitNumber))) ? WhiteColor : BlackColor;

}

/**
 * Save framebuffer to external memory
 */
void L2HAL_SSD1683_SaveFramebuffer
(
	L2HAL_SSD1683_ContextStruct * context,
	void* RAMContext,
	uint32_t saveAddress,
	void (*FramebufferMemoryWriteFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*)
)
{
	FramebufferMemoryWriteFunctionPtr(RAMContext, saveAddress, L2HAL_SSD1683_FRAMEBUFFER_SIZE, context->Framebuffer);
}

/**
 * Load framebuffer from external memory
 */
void L2HAL_SSD1683_LoadFramebuffer
(
	L2HAL_SSD1683_ContextStruct *context,
	void* RAMContext,
	uint32_t loadAddress,
	void (*FramebufferMemoryReadFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*)
)
{
	FramebufferMemoryReadFunctionPtr(RAMContext, loadAddress, L2HAL_SSD1683_FRAMEBUFFER_SIZE, context->Framebuffer);
}

/**
 * Power display on
 */
void L2HAL_SSD1683_PowerOn(L2HAL_SSD1683_ContextStruct *context)
{
	L2HAL_SSD1683_WriteCommand(context, 0x22);
	L2HAL_SSD1683_WriteDataByte(context, 0xE0);
	L2HAL_SSD1683_WriteCommand(context, 0x20);

	L2HAL_SSD1683_WaitForReadiness(context);
}

/**
 * Power display off. Use L2HAL_SSD1683_Init() to power it on back
 */
void L2HAL_SSD1683_PowerOff(L2HAL_SSD1683_ContextStruct *context)
{
	L2HAL_SSD1683_WriteCommand(context, 0x22);
	L2HAL_SSD1683_WriteDataByte(context, 0x83);
	L2HAL_SSD1683_WriteDataByte(context, 0x20);

	L2HAL_SSD1683_WaitForReadiness(context);
}

/**
 * Power display off and enter deepsleep mode. Use L2HAL_SSD1683_Init() to power it on back
 */
void L2HAL_SSD1683_EnterDeepSleep(L2HAL_SSD1683_ContextStruct *context, bool isRetainRam)
{
	L2HAL_SSD1683_PowerOff(context);

	L2HAL_SSD1683_WriteCommand(context, 0x10);
	L2HAL_SSD1683_WriteDataByte(context, isRetainRam ? 0x01 : 0x03);
}
