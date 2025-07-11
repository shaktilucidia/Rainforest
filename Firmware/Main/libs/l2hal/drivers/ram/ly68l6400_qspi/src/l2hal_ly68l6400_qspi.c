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

#include "../include/l2hal_ly68l6400_qspi_private.h"
#include "../../../../include/l2hal_errors.h"

void L2HAL_LY68L6400_QSPI_Init
(
	L2HAL_LY68L6400_QSPI_ContextStruct* context,

	QSPI_HandleTypeDef *qspiHandle,

	GPIO_TypeDef* chipSelectPort,
	uint16_t chipSelectPin,

	GPIO_TypeDef* clockPort,
	uint16_t clockPin,

	GPIO_TypeDef* mosiPort,
	uint16_t mosiPin,

	GPIO_TypeDef* misoPort,
	uint16_t misoPin
)
{
	/* Context setup */
	context->QSPIHandle = qspiHandle;

	context->ChipSelectPort = chipSelectPort;
	context->ChipSelectPin = chipSelectPin;

	context->ClockPort = clockPort;
	context->ClockPin = clockPin;

	context->MosiPort = mosiPort;
	context->MosiPin = mosiPin;

	context->MisoPort = misoPort;
	context->MisoPin = misoPin;

	HAL_Delay(100);

	/* After previous reset chip might be in QSPI mode, we have to pull it out from it */
	HAL_QSPI_DeInit(context->QSPIHandle);
	L2HAL_LY68L6400_QSPI_InitQspi(context);
	L2HAL_LY68L6400_QSPI_ExitQspiMode(context);

	/* Now chip must be in SPI mode, deinitializing QSPI */
	HAL_QSPI_DeInit(context->QSPIHandle);

	/* Starting in soft SPI mode to check chip state and switch it to QSPI mode */
	L2HAL_LY68L6400_QSPI_SoftSpi_Init(context);

	/* Reset enable */
	L2HAL_LY68L6400_QSPI_SoftSpi_SelectChip(context, true);
	L2HAL_LY68L6400_QSPI_SoftSpi_SwapBytes(context, 0x66);
	L2HAL_LY68L6400_QSPI_SoftSpi_SelectChip(context, false);

	/* Reset */
	L2HAL_LY68L6400_QSPI_SoftSpi_SelectChip(context, true);
	L2HAL_LY68L6400_QSPI_SoftSpi_SwapBytes(context, 0x99);
	L2HAL_LY68L6400_QSPI_SoftSpi_SelectChip(context, false);

	/* Chip ID - is chip on bus? */
	uint8_t readIdBuffer[2];

	L2HAL_LY68L6400_QSPI_SoftSpi_SelectChip(context, true);
	L2HAL_LY68L6400_QSPI_SoftSpi_SwapBytes(context, 0x9F);
	L2HAL_LY68L6400_QSPI_SoftSpi_SwapBytes(context, 0x00);
	L2HAL_LY68L6400_QSPI_SoftSpi_SwapBytes(context, 0x00);
	L2HAL_LY68L6400_QSPI_SoftSpi_SwapBytes(context, 0x00);

	readIdBuffer[0] = L2HAL_LY68L6400_QSPI_SoftSpi_SwapBytes(context, 0x00);
	readIdBuffer[1] = L2HAL_LY68L6400_QSPI_SoftSpi_SwapBytes(context, 0x00);

	L2HAL_LY68L6400_QSPI_SoftSpi_SelectChip(context, false);

	if (readIdBuffer[0] != 0x0D || readIdBuffer[1] != 0x5D)
	{
		// Chip is not on bus or failed
		L2HAL_Error(Generic);
	}

	/* Going to QSPI mode */
	L2HAL_LY68L6400_QSPI_SoftSpi_SelectChip(context, true);
	L2HAL_LY68L6400_QSPI_SoftSpi_SwapBytes(context, 0x35);
	L2HAL_LY68L6400_QSPI_SoftSpi_SelectChip(context, false);

	HAL_Delay(100);

	L2HAL_LY68L6400_QSPI_InitQspi(context);
}

void L2HAL_LY68L6400_QSPI_SoftSpi_Init(L2HAL_LY68L6400_QSPI_ContextStruct *context)
{
	L2HAL_MCU_ClockPortIn(context->ChipSelectPort);
	L2HAL_MCU_ClockPortIn(context->ClockPort);
	L2HAL_MCU_ClockPortIn(context->MosiPort);
	L2HAL_MCU_ClockPortIn(context->MisoPort);

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* CS -> OUT, initially HIGH */
	GPIO_InitStruct.Pin = context->ChipSelectPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(context->ChipSelectPort, &GPIO_InitStruct);
	HAL_GPIO_WritePin(context->ChipSelectPort, context->ChipSelectPin, GPIO_PIN_SET);

	/* Clock -> OUT, initially LOW */
	GPIO_InitStruct.Pin = context->ClockPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(context->ClockPort, &GPIO_InitStruct);
	HAL_GPIO_WritePin(context->ClockPort, context->ClockPin, GPIO_PIN_RESET);

	/* MOSI -> OUT, initially LOW */
	GPIO_InitStruct.Pin = context->MosiPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(context->MosiPort, &GPIO_InitStruct);
	HAL_GPIO_WritePin(context->MosiPort, context->MosiPin, GPIO_PIN_RESET);

	/* MISO -> IN */
	GPIO_InitStruct.Pin = context->MisoPin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(context->MisoPort, &GPIO_InitStruct);
}

void L2HAL_LY68L6400_QSPI_SoftSpi_SelectChip(L2HAL_LY68L6400_QSPI_ContextStruct *context, bool isSelected)
{
	if (isSelected)
	{
		HAL_GPIO_WritePin(context->ChipSelectPort, context->ChipSelectPin, GPIO_PIN_RESET); /* Active low*/
	}
	else
	{
		HAL_GPIO_WritePin(context->ChipSelectPort, context->ChipSelectPin, GPIO_PIN_SET);
	}

	HAL_Delay(1);
}

void L2HAL_LY68L6400_QSPI_SoftSpi_SetClockState(L2HAL_LY68L6400_QSPI_ContextStruct *context, bool isHigh)
{
	if (isHigh)
	{
		HAL_GPIO_WritePin(context->ClockPort, context->ClockPin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(context->ClockPort, context->ClockPin, GPIO_PIN_RESET);
	}
}

void L2HAL_LY68L6400_QSPI_SoftSpi_SetMosiState(L2HAL_LY68L6400_QSPI_ContextStruct *context, bool isHigh)
{
	if (isHigh)
	{
		HAL_GPIO_WritePin(context->MosiPort, context->MosiPin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(context->MosiPort, context->MosiPin, GPIO_PIN_RESET);
	}
}

GPIO_PinState L2HAL_LY68L6400_QSPI_SoftSpi_ReadMisoState(L2HAL_LY68L6400_QSPI_ContextStruct *context)
{
	return HAL_GPIO_ReadPin(context->MisoPort, context->MisoPin);
}

uint8_t L2HAL_LY68L6400_QSPI_SoftSpi_SwapBytes(L2HAL_LY68L6400_QSPI_ContextStruct *context, uint8_t toSend)
{
	uint8_t readData = 0;

	for (uint8_t i = 0; i < 8; i++)
	{
		L2HAL_LY68L6400_QSPI_SoftSpi_SetClockState(context, false);
		HAL_Delay(1);

		L2HAL_LY68L6400_QSPI_SoftSpi_SetMosiState(context, toSend & 0x80);

		HAL_Delay(1);

		toSend <<= 1;

		L2HAL_LY68L6400_QSPI_SoftSpi_SetClockState(context, true);
		HAL_Delay(1);

		readData <<= 1;
		if (L2HAL_LY68L6400_QSPI_SoftSpi_ReadMisoState(context) == GPIO_PIN_SET)
		{
			readData |= 0x01;
		}

		HAL_Delay(1);
	}

	return readData;
}

void L2HAL_LY68L6400_QSPI_MemoryReadInternal(L2HAL_LY68L6400_QSPI_ContextStruct *context, uint32_t startAddress, uint8_t size, uint8_t* buffer)
{
	if (size > L2HAL_LY68L6400_QSPI_READ_MAX_TRANSACTION_SIZE)
	{
		L2HAL_Error(Generic);
	}

	if (startAddress + (uint32_t)size > L2HAL_LY68L6400_QSPI_CAPACITY)
	{
		L2HAL_Error(Generic);
	}

	QSPI_CommandTypeDef sCommand = { 0 };

	sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
	sCommand.Instruction = 0xEB;

	sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
	sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
	sCommand.Address = startAddress;

	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytes = 0;

	sCommand.DataMode = QSPI_DATA_4_LINES;
	sCommand.NbData = size;

	sCommand.DummyCycles = 6;

	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;

	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command_IT(context->QSPIHandle, &sCommand) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}

	if (HAL_QSPI_Receive_DMA(context->QSPIHandle, buffer) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}
}

void L2HAL_LY68L6400_QSPI_MemoryWriteInternal(L2HAL_LY68L6400_QSPI_ContextStruct *context, uint32_t startAddress, uint8_t size, uint8_t* buffer)
{
	if (size > L2HAL_LY68L6400_QSPI_WRITE_MAX_TRANSACTION_SIZE)
	{
		L2HAL_Error(Generic);
	}

	if (startAddress + (uint32_t)size > L2HAL_LY68L6400_QSPI_CAPACITY)
	{
		L2HAL_Error(Generic);
	}

	QSPI_CommandTypeDef sCommand = { 0 };

	sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
	sCommand.Instruction = 0x38;

	sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
	sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
	sCommand.Address = startAddress;

	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytes = 0;

	sCommand.DataMode = QSPI_DATA_4_LINES;
	sCommand.NbData = size;

	sCommand.DummyCycles = 0;

	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;

	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command_IT(context->QSPIHandle, &sCommand) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}

	if (HAL_QSPI_Transmit_DMA(context->QSPIHandle, buffer) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}
}

void L2HAL_LY68L6400_QSPI_MemoryRead(L2HAL_LY68L6400_QSPI_ContextStruct *context, uint32_t startAddress, uint32_t size, uint8_t* buffer)
{
	if (startAddress + (uint32_t)size > L2HAL_LY68L6400_QSPI_CAPACITY)
	{
		L2HAL_Error(Generic);
	}

	uint32_t bufferStartAddress = 0;
	uint32_t packetStartAddress = startAddress;
	uint32_t remaining = size;

	while (remaining > 0)
	{
		uint8_t toRead;
		if (remaining > L2HAL_LY68L6400_QSPI_READ_MAX_TRANSACTION_SIZE)
		{
			toRead = L2HAL_LY68L6400_QSPI_READ_MAX_TRANSACTION_SIZE;
		}
		else
		{
			toRead = (uint8_t)remaining;
		}

		L2HAL_LY68L6400_QSPI_MemoryReadInternal(context, packetStartAddress, toRead, &buffer[bufferStartAddress]);

		bufferStartAddress += toRead;
		packetStartAddress += toRead;
		remaining -= toRead;
	}
}

void L2HAL_LY68L6400_QSPI_MemoryWrite(L2HAL_LY68L6400_QSPI_ContextStruct *context, uint32_t startAddress, uint32_t size, uint8_t* buffer)
{
	if (startAddress + (uint32_t)size > L2HAL_LY68L6400_QSPI_CAPACITY)
	{
		L2HAL_Error(Generic);
	}

	uint32_t bufferStartAddress = 0;
	uint32_t packetStartAddress = startAddress;
	uint32_t remaining = size;

	while (remaining > 0)
	{
		uint8_t toWrite;
		if (remaining > L2HAL_LY68L6400_QSPI_WRITE_MAX_TRANSACTION_SIZE)
		{
			toWrite = L2HAL_LY68L6400_QSPI_WRITE_MAX_TRANSACTION_SIZE;
		}
		else
		{
			toWrite = (uint8_t)remaining;
		}

		L2HAL_LY68L6400_QSPI_MemoryWriteInternal(context, packetStartAddress, toWrite, &buffer[bufferStartAddress]);

		bufferStartAddress += toWrite;
		packetStartAddress += toWrite;
		remaining -= toWrite;
	}
}

void L2HAL_LY68L6400_QSPI_InitQspi(L2HAL_LY68L6400_QSPI_ContextStruct *context)
{
	context->QSPIHandle->Init.ClockPrescaler = L2HAL_LY68L6400_QSPI_PRESCALER;
	context->QSPIHandle->Init.FifoThreshold = 1;
	context->QSPIHandle->Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
	context->QSPIHandle->Init.FlashSize = L2HAL_LY68L6400_QSPI_ADDRESS_SIZE;
	context->QSPIHandle->Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
	context->QSPIHandle->Init.ClockMode = QSPI_CLOCK_MODE_0;
	context->QSPIHandle->Init.FlashID = QSPI_FLASH_ID_1;
	context->QSPIHandle->Init.DualFlash = QSPI_DUALFLASH_DISABLE;

	if (HAL_QSPI_Init(context->QSPIHandle) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}
}

void L2HAL_LY68L6400_QSPI_ExitQspiMode(L2HAL_LY68L6400_QSPI_ContextStruct *context)
{
	QSPI_CommandTypeDef sCommand = { 0 };

	sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
	sCommand.Instruction = 0xF5;

	sCommand.AddressMode = QSPI_ADDRESS_NONE;
	sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
	sCommand.Address = 0x00;

	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytes = 0;

	sCommand.DataMode = QSPI_DATA_NONE;
	sCommand.NbData = 0;

	sCommand.DummyCycles = 0;

	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;

	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command_IT(context->QSPIHandle, &sCommand) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}
}
