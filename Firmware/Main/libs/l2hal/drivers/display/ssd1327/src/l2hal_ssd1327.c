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

#include "../include/l2hal_ssd1327_private.h"
#include "../../../../include/l2hal_errors.h"

L2HAL_SSD1327_ContextStruct L2HAL_SSD1327_DetectDisplayAtAddress(I2C_HandleTypeDef* i2cBusHandle, uint8_t address)
{
	L2HAL_SSD1327_ContextStruct context;
	context.IsFound = false;

	if (HAL_OK == HAL_I2C_IsDeviceReady(i2cBusHandle, address, 1, L2HAL_SSD1327_IO_TIMEOUT))
	{
		context.I2CHandle = i2cBusHandle;
		context.BusAddress = address;
		context.IsFound = true;
		context.IsDataTransferInProgress = false;
	}

	return context;
}

L2HAL_SSD1327_ContextStruct L2HAL_SSD1327_DetectDisplay(I2C_HandleTypeDef* i2cBusHandle)
{

	L2HAL_SSD1327_ContextStruct context = L2HAL_SSD1327_DetectDisplayAtAddress(i2cBusHandle, L2HAL_SSD1327_POSSIBLE_ADDRESS1);

	if (true == context.IsFound)
	{
		return context;
	}

	return L2HAL_SSD1327_DetectDisplayAtAddress(i2cBusHandle, L2HAL_SSD1327_POSSIBLE_ADDRESS2);
}

void L2HAL_SSD1327_InitDisplay(L2HAL_SSD1327_ContextStruct* context)
{
	L2HAL_SSD1327_CheckIfFound(context);

	/* To avoid blinking during setup */
	L2HAL_SSD1327_TurnOff(context);

	/* Hardware stuff */
	L2HAL_SSD1327_SetPhasesLenghts(context, 0x01, 0x01);
	L2HAL_SSD1327_SetDClockFosc(context, 1, 0);
	L2HAL_SSD1327_SetRegulator(context, true);
	L2HAL_SSD1327_SetPrechargeVoltage(context, 0x08);
	L2HAL_SSD1327_SetFunctionB(context, true, false);
	L2HAL_SSD1327_SetSecondPrechargePeriod(context, 0x04);
	L2HAL_SSD1327_SetVCOMH(context, 0x07);
	L2HAL_SSD1327_SetMUXRatio(context, 128);


	/* Full brightness */
	L2HAL_SSD1327_SetBrightness(context, 0xFFU);

	/* Start line (topmost) */
	L2HAL_SSD1327_SetStartLine(context, 0x00);

	/* Vertical offset (0 pixels) */
	L2HAL_SSD1327_SetVerticalOffset(context, 0x00);

	/* Column start/end addresses */
	L2HAL_SSD1327_SetColumnAddress(context, 0x00U, 0x3FU);

	/* Row start/end address */
	L2HAL_SSD1327_SetRowAddresses(context, 0x00U, 0x7FU);

	/* Default grayscale table */
	L2HAL_SSD1327_LoadDefaultGrayscaleTable(context);

	/* Remap */
	L2HAL_SSD1327_SetRemap(context,
			true, /* enableColumnAddressRemap */
			false, /* enableNibbleRemap */
			false, /* enableVerticalAddressIncrement */
			true, /* enableComRemap */
			true /* enableComSplitOddEven */);

	/* Normal mode */
	L2HAL_SSD1327_SetDisplayMode(context, Normal);

	/* Clearing display */
	memset(context->Framebuffer, 0x00U, L2HAL_SSD1327_FRAMEBUFFER_SIZE);
	L2HAL_SSD1327_PushFramebuffer(context);

	/* Finally turning on */
	L2HAL_SSD1327_TurnOn(context);
}

void L2HAL_SSD1327_CheckIfFound(L2HAL_SSD1327_ContextStruct* context)
{
	if (true != context->IsFound)
	{
		L2HAL_Error(Generic);
	}
}

void L2HAL_SSD1327_WaitForTransferCompletion(L2HAL_SSD1327_ContextStruct* context)
{
	while(context->IsDataTransferInProgress) {}
}

void L2HAL_SSD1327_WriteCommand(L2HAL_SSD1327_ContextStruct* context, uint8_t command)
{
	L2HAL_SSD1327_CheckIfFound(context);

	L2HAL_SSD1327_WaitForTransferCompletion(context); /* We can't send command when transfer is in progress */

	context->CommandCode = command;

	/* Transfer started */
	context->IsDataTransferInProgress = true;

	/* Using HAL_I2C_Mem_Write to prepend control byte without allocating buffer for it. */
	if (HAL_OK != HAL_I2C_Mem_Write_IT(context->I2CHandle, context->BusAddress, L2HAL_SSD1327_CONTROL_BYTE_COMMAND, 1, &(context->CommandCode), 1))
	{
		context->IsDataTransferInProgress = false; /* Transfer didn't start */
		L2HAL_Error(Generic);
	}
}

void L2HAL_SSD1327_WriteData(L2HAL_SSD1327_ContextStruct* context, uint8_t* data, uint16_t length)
{
	L2HAL_SSD1327_CheckIfFound(context);

	L2HAL_SSD1327_WaitForTransferCompletion(context); /* We can't send command when transfer is in progress */

	/* Transfer started */
	context->IsDataTransferInProgress = true;

	/* Using HAL_I2C_Mem_Write to prepend control byte without allocating buffer for it */
	if (HAL_OK != HAL_I2C_Mem_Write_IT(context->I2CHandle, context->BusAddress, L2HAL_SSD1327_CONTROL_BYTE_DATA, 1, data, length))
	{
		context->IsDataTransferInProgress = false; /* Transfer didn't start */
		L2HAL_Error(Generic);
	}
}

void L2HAL_SSD1327_TurnOn(L2HAL_SSD1327_ContextStruct* context)
{
	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_ON);
}

void L2HAL_SSD1327_TurnOff(L2HAL_SSD1327_ContextStruct* context)
{
	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_OFF);
}

void L2HAL_SSD1327_SetBrightness(L2HAL_SSD1327_ContextStruct* context, uint8_t brightness)
{
	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_CONTRAST);
	L2HAL_SSD1327_WriteCommand(context, brightness);
}

void L2HAL_SSD1327_SetColumnAddress(L2HAL_SSD1327_ContextStruct* context, uint8_t startAddress, uint8_t endAddress)
{
	if (startAddress > L2HAL_SSD1327_MAX_COLUMN_ADDRESS || endAddress > L2HAL_SSD1327_MAX_COLUMN_ADDRESS)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_COLUMN_ADDRESS);
	L2HAL_SSD1327_WriteCommand(context, startAddress);
	L2HAL_SSD1327_WriteCommand(context, endAddress);
}

void L2HAL_SSD1327_LoadDefaultGrayscaleTable(L2HAL_SSD1327_ContextStruct* context)
{
	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_DEFAULT_GRAYSCALE_TABLE);
}

void L2HAL_SSD1327_SetPhasesLenghts(L2HAL_SSD1327_ContextStruct* context, uint8_t phase1, uint8_t phase2)
{
	if (phase1 < L2HAL_SSD1327_MIN_PHASELENGTH || phase2 < L2HAL_SSD1327_MIN_PHASELENGTH
			|| phase1 > L2HAL_SSD1327_MAX_PHASELENGTH || phase2 > L2HAL_SSD1327_MAX_PHASELENGTH)
	{
		L2HAL_Error(Generic);
	}

	phase1 = 0x0F & phase1;
	phase2 = (0x0F & phase2) << 4;

	uint8_t phase = phase2 | phase1;
	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_PHASE_LENGTH);
	L2HAL_SSD1327_WriteCommand(context, phase);

}

void L2HAL_SSD1327_SetDClockFosc(L2HAL_SSD1327_ContextStruct* context, uint8_t divideRatio, uint8_t fOsc)
{
	if (divideRatio < L2HAL_SSD1327_MIN_D || divideRatio > L2HAL_SSD1327_MAX_D || fOsc > L2HAL_SSD1327_MAX_FOSC)
	{
		L2HAL_Error(Generic);
	}

	divideRatio --;
	fOsc = fOsc << 4;

	uint8_t data = fOsc + divideRatio;
	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_FCD_OF);
	L2HAL_SSD1327_WriteCommand(context, data);
}

void L2HAL_SSD1327_SetRegulator(L2HAL_SSD1327_ContextStruct* context, bool enableExternalRegulator)
{
	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_FUNCTION_A);

	if (enableExternalRegulator)
	{
		L2HAL_SSD1327_WriteCommand(context, 0x01);
	}
	else
	{
		L2HAL_SSD1327_WriteCommand(context, 0x00);
	}
}

void L2HAL_SSD1327_SetPrechargeVoltage(L2HAL_SSD1327_ContextStruct* context, uint8_t voltage)
{
	if (voltage > L2HAL_SSD1327_MAX_PRECHARGE_VOLTAGE)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_PRECHARGE_VOLTAGE);
	L2HAL_SSD1327_WriteCommand(context, voltage);
}

void L2HAL_SSD1327_SetFunctionB(L2HAL_SSD1327_ContextStruct* context, bool enableSecondPrecharge, bool enableExternalVSL)
{
	uint8_t secondByte = L2HAL_SSD1327_SET_FUNCTION_B_TEMPLATE;

	if (enableSecondPrecharge)
	{
		secondByte |= L2HAL_SSD1327_ENABLE_SECOND_PRECHARGE;
	}

	if (enableExternalVSL)
	{
		secondByte |= L2HAL_SSD1327_ENABLE_EXTERNAL_VSL;
	}

	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_FUNCTION_B);
	L2HAL_SSD1327_WriteCommand(context, secondByte);
}

void L2HAL_SSD1327_SetSecondPrechargePeriod(L2HAL_SSD1327_ContextStruct* context, uint8_t period)
{
	if (period < L2HAL_SSD1327_MIN_SECOND_PRECHARGE_PERIOD || period > L2HAL_SSD1327_MAX_SECOND_PRECHARGE_PERIOD)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_SECOND_PRECHARGE_PERIOD);
	L2HAL_SSD1327_WriteCommand(context, period);
}

void L2HAL_SSD1327_SetVCOMH(L2HAL_SSD1327_ContextStruct* context, uint8_t vcomh)
{
	if (vcomh > L2HAL_SSD1327_VCOMH_MAX)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_VCOMH);
	L2HAL_SSD1327_WriteCommand(context, vcomh);
}

void L2HAL_SSD1327_SetMUXRatio(L2HAL_SSD1327_ContextStruct* context, uint8_t muxRatio)
{
	if (muxRatio < L2HAL_SSD1327_MIN_MUXRATIO || muxRatio > L2HAL_SSD1327_MAX_MUXRATIO)
	{
		L2HAL_Error(Generic);
	}

	uint8_t muxRatioData = muxRatio - 1U;

	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_MUX_RATIO);
	L2HAL_SSD1327_WriteCommand(context, muxRatioData);
}

void L2HAL_SSD1327_SetDisplayMode(L2HAL_SSD1327_ContextStruct* context, L2HAL_SSD1327_DisplayMode mode)
{
	uint8_t command;
	switch (mode)
	{
		case Normal:
			command = L2HAL_SSD1327_COMMAND_SET_NORMAL_DISPLAY;
		break;
		case AllPixelsOn:
			command = L2HAL_SSD1327_COMMAND_SET_ALL_ON;
		break;
		case AllPixelsOff:
			command = L2HAL_SSD1327_COMMAND_SET_ALL_OFF;
		break;
		case Inverse:
			command = L2HAL_SSD1327_COMMAND_SET_INVERSE;
		break;
		default:
			L2HAL_Error(Generic);
	}

	L2HAL_SSD1327_WriteCommand(context, command);
}

void L2HAL_SSD1327_SetStartLine(L2HAL_SSD1327_ContextStruct* context, uint8_t startLine)
{
	if (startLine > L2HAL_SSD1327_MAX_START_LINE)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_DISPLAY_START_LINE);
	L2HAL_SSD1327_WriteCommand(context, startLine);
}

void L2HAL_SSD1327_SetVerticalOffset(L2HAL_SSD1327_ContextStruct* context, uint8_t verticalOffset)
{
	if (verticalOffset > L2HAL_SSD1327_MAX_VERTICAL_OFFSET)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_VERTICAL_OFFSET);
	L2HAL_SSD1327_WriteCommand(context, verticalOffset);
}

void L2HAL_SSD1327_SetRowAddresses(L2HAL_SSD1327_ContextStruct* context, uint8_t rowStartAddress, uint8_t rowEndAddress)
{
	if (rowStartAddress > L2HAL_SSD1327_MAX_ROW_ADDRESS || rowEndAddress > L2HAL_SSD1327_MAX_ROW_ADDRESS)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_ROW_ADDRESS);
	L2HAL_SSD1327_WriteCommand(context, rowStartAddress);
	L2HAL_SSD1327_WriteCommand(context, rowEndAddress);
}

void L2HAL_SSD1327_SetRemap(L2HAL_SSD1327_ContextStruct* context,
		bool enableColumnAddressRemap,
		bool enableNibbleRemap,
		bool enableVerticalAddressIncrement,
		bool enableComRemap,
		bool enableComSplitOddEven)
{
	uint8_t data = 0x00;

	if (enableColumnAddressRemap)
	{
		data |= L2HAL_SSD1327_ENABLE_COLUMN_ADDRESS_REMAP;
	}

	if (enableNibbleRemap)
	{
		data |= L2HAL_SSD1327_ENABLE_NIBBLE_REMAP;
	}

	if (enableVerticalAddressIncrement)
	{
		data |= L2HAL_SSD1327_ENABLE_VERTICAL_ADDRESS_INCREMENT;
	}

	if (enableComRemap)
	{
		data |= L2HAL_SSD1327_ENABLE_COM_REMAP;
	}

	if (enableComSplitOddEven)
	{
		data |= L2HAL_SSD1327_ENABLE_COM_SPLIT_ODD_EVEN;
	}

	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_REMAP);
	L2HAL_SSD1327_WriteCommand(context, data);
}

void L2HAL_SSD1327_SetGrayscaleTable(L2HAL_SSD1327_ContextStruct* context, uint8_t table[15])
{
	if (table[14] > L2HAL_SSD1327_MAX_GRAYSCALE_LEVEL)
	{
		L2HAL_Error(Generic);
	}

	for (uint8_t i = 14; i > 0; i--)
	{
		if (table[i] <= table[i - 1])
		{
			L2HAL_Error(Generic);
		}
	}

	L2HAL_SSD1327_WriteCommand(context, L2HAL_SSD1327_COMMAND_SET_GRAYSCALE_TABLE);
	for (uint8_t i = 0; i < 15; i++)
	{
		L2HAL_SSD1327_WriteCommand(context, table[i]);
	}
}

#ifdef L2HAL_SSD1327_MONOCHROME_MODE
	void L2HAL_SSD1327_PushFramebuffer(L2HAL_SSD1327_ContextStruct* context)
	{
		L2HAL_SSD1327_SetColumnAddress(context, 0x00U, 0x3FU);
		HAL_Delay(L2HAL_SSD1327_COLUMN_ADDRESS_TO_ROW_ADDRESS_DELAY);

		L2HAL_SSD1327_SetRowAddresses(context, 0x00U, 0x7FU);
		HAL_Delay(L2HAL_SSD1327_ROW_ADDRESS_TO_DATA_DELAY);

		uint16_t framebufferIndex;
		uint8_t framebufferBitmask;

		uint8_t linebufferIndex;
		bool isMostSignificantNibble;

		for (uint8_t y = 0; y < L2HAL_SSD1327_DISPLAY_HEIGHT; y++)
		{
			memset(context->lineBuffer, 0x00, L2HAL_SSD1327_DECOMPRESSED_LINE_SIZE);

			for (uint8_t x = 0; x < L2HAL_SSD1327_DISPLAY_WIDTH; x++)
			{
				L2HAL_SSD1327_GetPixelAddressMonochrome(x, y, &framebufferIndex, &framebufferBitmask);

				uint8_t isLit;
				if ((context->Framebuffer[framebufferIndex] & framebufferBitmask) != 0)
				{
					isLit = 0xFFU;
				}
				else
				{
					isLit = 0x00U;
				}

				linebufferIndex = x / 2U; // Two pixels per byte
				isMostSignificantNibble = (x % 2U == 0);

				if (isMostSignificantNibble)
				{
					context->lineBuffer[linebufferIndex] |= 0xF0U & isLit;
				}
				else
				{
					context->lineBuffer[linebufferIndex] |= 0x0FU & isLit;
				}
			}

			L2HAL_SSD1327_WriteData(context, context->lineBuffer, L2HAL_SSD1327_DECOMPRESSED_LINE_SIZE);
			HAL_Delay(L2HAL_SSD1327_INTERLINE_DELAY);
		}
	}
#else
	void L2HAL_SSD1327_PushFramebuffer(L2HAL_SSD1327_ContextStruct* context)
	{
		L2HAL_SSD1327_SetColumnAddress(context, 0x00U, 0x3FU);
		L2HAL_SSD1327_SetRowAddresses(context, 0x00U, 0x7FU);
		L2HAL_SSD1327_WriteData(context, context->Framebuffer, L2HAL_SSD1327_FRAMEBUFFER_SIZE);
	}
#endif


bool L2HAL_SSD1327_GetPixelAddress(uint16_t x, uint16_t y, uint16_t* index, bool* isMostSignificantNibble)
{
	if (x > L2HAL_SSD1327_DISPLAY_WIDTH || y > L2HAL_SSD1327_DISPLAY_HEIGHT)
	{
		return false;
	}

	uint16_t pixelNumber = y * L2HAL_SSD1327_DISPLAY_WIDTH + x;

	*index = pixelNumber / 2U;
	*isMostSignificantNibble = (pixelNumber % 2U == 0);

	return true;
}

bool L2HAL_SSD1327_GetPixelAddressMonochrome(uint16_t x, uint16_t y, uint16_t* index, uint8_t* mask)
{
	if (x > L2HAL_SSD1327_DISPLAY_WIDTH || y > L2HAL_SSD1327_DISPLAY_HEIGHT)
	{
		return false;
	}

	*index = y * L2HAL_SSD1327_COMPRESSED_LINE_SIZE + x / L2HAL_SSD1327_PAGE_WIDTH;
	*mask = 1 << (x % L2HAL_SSD1327_PAGE_WIDTH);

	return true;
}

void L2HAL_SSD1327_SetActiveColor(L2HAL_SSD1327_ContextStruct* context, FMGL_API_ColorStruct color)
{
	context->ActiveColor = ((uint32_t)color.R * L2HAL_SSD1327_BRIGHTNESS_MUL_R_FACTOR
				+ (uint32_t)color.G * L2HAL_SSD1327_BRIGHTNESS_MUL_G_FACTOR
				+ (uint32_t)color.B * L2HAL_SSD1327_BRIGHTNESS_MUL_B_FACTOR) / L2HAL_SSD1327_BRIGHTNESS_DIV_FACTOR;

	if (context->ActiveColor > L2HAL_SSD1327_MAX_BRIGHTNESS)
	{
		context->ActiveColor = L2HAL_SSD1327_MAX_BRIGHTNESS;
	}
}

#ifdef L2HAL_SSD1327_MONOCHROME_MODE
	void L2HAL_SSD1327_DrawPixel(L2HAL_SSD1327_ContextStruct* context, uint16_t x, uint16_t y)
	{
		uint16_t index;
		uint8_t bitmask;

		if (!L2HAL_SSD1327_GetPixelAddressMonochrome(x, y, &index, &bitmask))
		{
			return;
		}

		uint8_t targetColor;
		if (context->ActiveColor > 0)
		{
			// Lit
			context->Framebuffer[index] |= bitmask;
		}
		else
		{
			// Shut
			context->Framebuffer[index] &= ~bitmask;
		}
	}
#else
	void L2HAL_SSD1327_DrawPixel(L2HAL_SSD1327_ContextStruct* context, uint16_t x, uint16_t y)
	{
		uint16_t index;
		bool isMostSignificantNibble;

		if (!L2HAL_SSD1327_GetPixelAddress(x, y, &index, &isMostSignificantNibble))
		{
			return;
		}

		if (isMostSignificantNibble)
		{
			context->Framebuffer[index] &= 0x0FU;
			context->Framebuffer[index] |= (context->ActiveColor & 0x0FU) << 4;
		}
		else
		{
			context->Framebuffer[index] &= 0xF0U;
			context->Framebuffer[index] |= (context->ActiveColor & 0x0FU);
		}
	}
#endif


#ifdef L2HAL_SSD1327_MONOCHROME_MODE
	FMGL_API_ColorStruct L2HAL_SSD1327_GetPixel(L2HAL_SSD1327_ContextStruct* context, uint16_t x, uint16_t y)
	{
		FMGL_API_ColorStruct result;
		result.R = 0;
		result.G = 0;
		result.B = 0;

		uint16_t index;
		uint8_t bitmask;

		if (!L2HAL_SSD1327_GetPixelAddressMonochrome(x, y, &index, &bitmask))
		{
			return result;
		}

		uint8_t brightness = context->Framebuffer[index] & bitmask;

		if (brightness != 0)
		{
			result.R = FMGL_API_MAX_CHANNEL_BRIGHTNESS;
			result.G = FMGL_API_MAX_CHANNEL_BRIGHTNESS;
			result.B = FMGL_API_MAX_CHANNEL_BRIGHTNESS;
		}

		return result;
	}
#else
	FMGL_API_ColorStruct L2HAL_SSD1327_GetPixel(L2HAL_SSD1327_ContextStruct* context, uint16_t x, uint16_t y)
	{
		FMGL_API_ColorStruct result;
		result.R = 0;
		result.G = 0;
		result.B = 0;

		uint16_t index;
		bool isMostSignificantNibble;

		if (!L2HAL_SSD1327_GetPixelAddress(x, y, &index, &isMostSignificantNibble))
		{
			return result;
		}

		uint8_t brightness;
		if (isMostSignificantNibble)
		{
			brightness = (context->Framebuffer[index] & 0xF0U) >> 4;
		}
		else
		{
			brightness = context->Framebuffer[index] & 0x0FU;
		}

		result.R = brightness * L2HAL_SSD1327_GET_PIXEL_MULTIPLIER;
		result.G = result.R;
		result.B = result.R;

		return result;
	}
#endif



uint16_t L2HAL_SSD1327_GetWidth(void)
{
	return L2HAL_SSD1327_DISPLAY_WIDTH;
}

uint16_t L2HAL_SSD1327_GetHeight(void)
{
	return L2HAL_SSD1327_DISPLAY_HEIGHT;
}

void L2HAL_SSD1327_InterruptTransferCompleted(L2HAL_SSD1327_ContextStruct* context)
{
	if (context->IsDataTransferInProgress)
	{
		/* Data transfer completed */
		context->IsDataTransferInProgress = false;
	}
}
