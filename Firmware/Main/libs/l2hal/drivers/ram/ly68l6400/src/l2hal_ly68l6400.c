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

#include "../include/l2hal_ly68l6400_private.h"

void L2HAL_LY68L6400_Init
(
	L2HAL_LY68L6400_ContextStruct* context,
	SPI_HandleTypeDef *spiHandle,

	GPIO_TypeDef* chipSelectPort,
	uint16_t chipSelectPin
)
{
	context->SPIHandle = spiHandle;

	context->ChipSelectPort = chipSelectPort;
	context->ChipSelectPin = chipSelectPin;

	HAL_Delay(100);

	uint8_t commandBuffer[4];

	/* Reset enable */
	commandBuffer[0] = 0x66;
	L2HAL_LY68L6400_SelectChip(context, true);
	L2HAL_LY68L6400_WriteData(context, commandBuffer, 1);
	L2HAL_LY68L6400_SelectChip(context, false);

	/* Reset */
	commandBuffer[0] = 0x99;
	L2HAL_LY68L6400_SelectChip(context, true);
	L2HAL_LY68L6400_WriteData(context, commandBuffer, 1);
	L2HAL_LY68L6400_SelectChip(context, false);

	HAL_Delay(100);

	/* Chip ID - is chip on bus? */
	uint8_t readIdBuffer[2];

	commandBuffer[0] = 0x9F;
	commandBuffer[1] = 0x00;
	commandBuffer[2] = 0x00;
	commandBuffer[3] = 0x00;
	L2HAL_LY68L6400_SelectChip(context, true);
	L2HAL_LY68L6400_WriteData(context, commandBuffer, 4);
	L2HAL_LY68L6400_ReadData(context, readIdBuffer, 2);
	L2HAL_LY68L6400_SelectChip(context, false);

	if (readIdBuffer[0] != 0x0D || readIdBuffer[1] != 0x5D)
	{
		// Chip is not on bus or failed
		L2HAL_Error(Generic);
	}
}

void L2HAL_LY68L6400_SelectChip(L2HAL_LY68L6400_ContextStruct *context, bool isSelected)
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

void L2HAL_LY68L6400_WriteData(L2HAL_LY68L6400_ContextStruct *context, uint8_t* data, uint16_t dataSize)
{
	context->IsDataTransferInProgress = true;

	if (HAL_SPI_Transmit_DMA(context->SPIHandle, data, dataSize) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_LY68L6400_WaitForDataTransferCompletion(context);
}

void L2HAL_LY68L6400_ReadData(L2HAL_LY68L6400_ContextStruct *context, uint8_t* dataBuffer, uint16_t readSize)
{
	uint8_t* transmitData = malloc(readSize);
	memset(transmitData, 0xFF, readSize);

	context->IsDataTransferInProgress = true;
	if (HAL_SPI_TransmitReceive_DMA(context->SPIHandle, transmitData, dataBuffer, readSize) != HAL_OK)
	{
		free(transmitData);
		L2HAL_Error(Generic);
	}

	L2HAL_LY68L6400_WaitForDataTransferCompletion(context);

	free(transmitData);
}

void L2HAL_LY68L6400_WaitForDataTransferCompletion(L2HAL_LY68L6400_ContextStruct *context)
{
	while (context->IsDataTransferInProgress) {} /* First wait for DMA completion */
	while (HAL_SPI_GetState(context->SPIHandle) != HAL_SPI_STATE_READY) { } /* Then wait for SPI ready*/
}

void L2HAL_LY68L6400_MarkDataTransferAsCompleted(L2HAL_LY68L6400_ContextStruct *context)
{
	context->IsDataTransferInProgress = false;
}

void L2HAL_LY68L6400_MemoryReadInternal(L2HAL_LY68L6400_ContextStruct *context, uint32_t startAddress, uint8_t size, uint8_t* buffer)
{
	if (size > L2HAL_LY68L6400_MAX_READ_BYTES)
	{
		L2HAL_Error(Generic);
	}

	if (startAddress + (uint32_t)size > L2HAL_LY68L6400_CAPACITY)
	{
		L2HAL_Error(Generic);
	}

	uint8_t commandBuffer[5];

	commandBuffer[0] = 0x0B; /* Fast read */
	commandBuffer[1] = (startAddress & 0xFF0000) >> 16; // Address MSB
	commandBuffer[2] = (startAddress & 0xFF00) >> 8;
	commandBuffer[3] = startAddress & 0xFF; // Address LSB
	commandBuffer[4] = 0x00; // Wait cycle

	L2HAL_LY68L6400_SelectChip(context, true);
	L2HAL_LY68L6400_WriteData(context, commandBuffer, 5);
	L2HAL_LY68L6400_ReadData(context, buffer, size);
	L2HAL_LY68L6400_SelectChip(context, false);
}

void L2HAL_LY68L6400_MemoryWriteInternal(L2HAL_LY68L6400_ContextStruct *context, uint32_t startAddress, uint8_t size, uint8_t* buffer)
{
	if (size > L2HAL_LY68L6400_MAX_READ_BYTES)
	{
		L2HAL_Error(Generic);
	}

	if (startAddress + (uint32_t)size > L2HAL_LY68L6400_CAPACITY)
	{
		L2HAL_Error(Generic);
	}

	uint8_t commandBuffer[4];

	commandBuffer[0] = 0x02; /* Write */
	commandBuffer[1] = (startAddress & 0xFF0000) >> 16; // Address MSB
	commandBuffer[2] = (startAddress & 0xFF00) >> 8;
	commandBuffer[3] = startAddress & 0xFF; // Address LSB

	L2HAL_LY68L6400_SelectChip(context, true);
	L2HAL_LY68L6400_WriteData(context, commandBuffer, 4); // Sending command
	L2HAL_LY68L6400_WriteData(context, buffer, size); // Writing data
	L2HAL_LY68L6400_SelectChip(context, false);
}

void L2HAL_LY68L6400_MemoryRead(L2HAL_LY68L6400_ContextStruct *context, uint32_t startAddress, uint32_t size, uint8_t* buffer)
{
	if (startAddress + (uint32_t)size > L2HAL_LY68L6400_CAPACITY)
	{
		L2HAL_Error(Generic);
	}

	uint32_t bufferStartAddress = 0;
	uint32_t packetStartAddress = startAddress;
	uint32_t remaining = size;

	while (remaining > 0)
	{
		uint8_t toRead;
		if (remaining > L2HAL_LY68L6400_MAX_READ_BYTES)
		{
			toRead = L2HAL_LY68L6400_MAX_READ_BYTES;
		}
		else
		{
			toRead = (uint8_t)remaining;
		}

		L2HAL_LY68L6400_MemoryReadInternal(context, packetStartAddress, toRead, &buffer[bufferStartAddress]);

		bufferStartAddress += toRead;
		packetStartAddress += toRead;
		remaining -= toRead;
	}
}

void L2HAL_LY68L6400_MemoryWrite(L2HAL_LY68L6400_ContextStruct *context, uint32_t startAddress, uint32_t size, uint8_t* buffer)
{
	if (startAddress + (uint32_t)size > L2HAL_LY68L6400_CAPACITY)
	{
		L2HAL_Error(Generic);
	}

	uint32_t bufferStartAddress = 0;
	uint32_t packetStartAddress = startAddress;
	uint32_t remaining = size;

	while (remaining > 0)
	{
		uint8_t toWrite;
		if (remaining > L2HAL_LY68L6400_MAX_READ_BYTES)
		{
			toWrite = L2HAL_LY68L6400_MAX_READ_BYTES;
		}
		else
		{
			toWrite = (uint8_t)remaining;
		}

		L2HAL_LY68L6400_MemoryWriteInternal(context, packetStartAddress, toWrite, &buffer[bufferStartAddress]);

		bufferStartAddress += toWrite;
		packetStartAddress += toWrite;
		remaining -= toWrite;
	}
}

