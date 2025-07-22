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

#include "../include/l2hal_sdcard_private.h"
#include "../include/l2hal_sdcard.h"
#include "../../../include/l2hal_errors.h"
#include <stdlib.h>
#include <string.h>

enum L2HAL_SDCard_InitResult L2HAL_SDCard_Init
(
	L2HAL_SDCard_ContextStruct* context,
	SPI_HandleTypeDef *spiHandle,

	GPIO_TypeDef* chipSelectPort,
	uint16_t chipSelectPin
)
{
	context->SPIHandle = spiHandle;

	context->ChipSelectPort = chipSelectPort;
	context->ChipSelectPin = chipSelectPin;

	context->IsDataTransferInProgress = false;

	/*
	Step 1.

	Set DI and CS high and apply 74 or more clock pulses to SCLK. Without this
	step under certain circumstances SD-card will not work. For instance, when
	multiple SPI devices are sharing the same bus (i.e. MISO, MOSI, CS).
	*/
	L2HAL_SDCard_Select(context, false);

	const uint8_t high = 0xFF;
	for(uint8_t i = 0; i < 10; i++) // Each packet is 8 bits long, so 8 x 10 bits are sent, i.e. 80 clocks
	{
		L2HAL_SDCard_WriteDataNoCSControl(context, &high, 1);
	}

	/*
	Step 2.

	Send CMD0 (GO_IDLE_STATE): Reset the SD card.
	*/
	L2HAL_SDCard_Select(context, true);
	L2HAL_SDCard_WaitForBusyCleared(context);

	const uint8_t cmd0[] = { 0x40 | 0x00 /* CMD0 */, 0x00, 0x00, 0x00, 0x00 /* ARG = 0 */, (0x4A << 1) | 1 /* CRC7 + end bit */ };
	L2HAL_SDCard_WriteDataNoCSControl(context, cmd0, sizeof(cmd0));

	uint8_t r1Response;
	if (!L2HAL_SDCard_ReadR1(context, &r1Response))
	{
		// We have no SD-card inserted
		L2HAL_SDCard_Select(context, false);
		return NoCardInserted;
	}

	if (r1Response != 0x01)
	{
		L2HAL_SDCard_Select(context, false);
		return Error;
	}

	/*
	Step 3.

	After the card enters idle state with a CMD0, send a CMD8 with argument of
	0x000001AA and correct CRC prior to initialization process. If the CMD8 is
	rejected with illegal command error (0x05), the card is SDC version 1 or
	MMC version 3. If accepted, R7 response (R1(0x01) + 32-bit return value)
	will be returned. The lower 12 bits in the return value 0x01AA means that
	the card is SDC version 2 and it can work at voltage range of 2.7 to 3.6
	volts. If not the case, the card should be rejected.
	*/
	L2HAL_SDCard_WaitForBusyCleared(context);

	const uint8_t cmd8[] = { 0x40 | 0x08 /* CMD8 */, 0x00, 0x00, 0x01, 0xAA /* ARG */, (0x43 << 1) | 1 /* CRC7 + end bit */ };
	L2HAL_SDCard_WriteDataNoCSControl(context, cmd8, sizeof(cmd8));

	if (!L2HAL_SDCard_ReadR1(context, &r1Response))
	{
		L2HAL_SDCard_Select(context, false);
		return Error;
	}

	if (r1Response != 0x01)
	{
		L2HAL_SDCard_Select(context, false);
		return Error;
	}

	uint8_t r7Response[4];
	L2HAL_SDCard_ReadData(context, r7Response, sizeof(r7Response));

	if(((r7Response[2] & 0x0b00001111) != 0x01) || (r7Response[3] != 0xAA))
	{
		L2HAL_SDCard_Select(context, false);
		return UnsupportedCard;
	}

	/*
	Step 4.

	And then initiate initialization with ACMD41 with HCS flag (bit 30).
	*/
	while(true)
	{
		L2HAL_SDCard_WaitForBusyCleared(context);

		const uint8_t cmd55[] = { 0x40 | 0x37 /* CMD55 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
		L2HAL_SDCard_WriteDataNoCSControl(context, cmd55, sizeof(cmd55));

		if (!L2HAL_SDCard_ReadR1(context, &r1Response))
		{
			L2HAL_SDCard_Select(context, false);
			return Error;
		}

		if (r1Response != 0x01)
		{
			L2HAL_SDCard_Select(context, false);
			return Error;
		}

		L2HAL_SDCard_WaitForBusyCleared(context);

		const uint8_t acmd41[] = { 0x40 | 0x29 /* ACMD41 */, 0x40, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
		L2HAL_SDCard_WriteDataNoCSControl(context, acmd41, sizeof(acmd41));

		if (!L2HAL_SDCard_ReadR1(context, &r1Response))
		{
			L2HAL_SDCard_Select(context, false);
			return Error;
		}

		if(r1Response == 0x00)
		{
			// Ready
			break;
		}

		if(r1Response != 0x01)
		{
			L2HAL_SDCard_Select(context, false);
			return Error;
		}
	}

	/*
	Step 5.

	After the initialization completed, read OCR register with CMD58 and check
	CCS flag (bit 30). When it is set, the card is a high-capacity card known
	as SDHC/SDXC.
	*/
	L2HAL_SDCard_WaitForBusyCleared(context);

	const uint8_t cmd58[] = { 0x40 | 0x3A /* CMD58 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
	L2HAL_SDCard_WriteDataNoCSControl(context, cmd58, sizeof(cmd58));

	if (!L2HAL_SDCard_ReadR1(context, &r1Response))
	{
		L2HAL_SDCard_Select(context, false);
		return Error;
	}

	if (r1Response != 0x00)
	{
		L2HAL_SDCard_Select(context, false);
		return Error;
	}

	uint8_t response[4];
	L2HAL_SDCard_ReadData(context, response, sizeof(response));

	if((response[0] & 0xC0) != 0xC0)
	{
		L2HAL_SDCard_Select(context, false);
		return UnsupportedCard;
	}

	/* Done */
	L2HAL_SDCard_Select(context, false);

	return Success;
}

void L2HAL_SDCard_Select(L2HAL_SDCard_ContextStruct *context, bool isSelected)
{
	const uint8_t high = 0xFF;
	L2HAL_SDCard_WriteDataNoCSControl(context, &high, 1);

	if (isSelected)
	{
		HAL_GPIO_WritePin(context->ChipSelectPort, context->ChipSelectPin, GPIO_PIN_RESET); /* Active low*/
	}
	else
	{
		HAL_GPIO_WritePin(context->ChipSelectPort, context->ChipSelectPin, GPIO_PIN_SET);
	}

	L2HAL_SDCard_WriteDataNoCSControl(context, &high, 1);
}

void L2HAL_SDCard_WriteDataNoCSControl(L2HAL_SDCard_ContextStruct *context, uint8_t *data, uint16_t dataSize)
{
	context->IsDataTransferInProgress = true;
	if (HAL_SPI_Transmit_DMA(context->SPIHandle, data, dataSize) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}

	L2HAL_SDCard_WaitForDataTransferCompletion(context);
}

void L2HAL_SDCard_ReadData(L2HAL_SDCard_ContextStruct *context, uint8_t *buffer, uint16_t readSize)
{
	uint8_t* transmitData = malloc(readSize);
	memset(transmitData, 0xFF, readSize);

	context->IsDataTransferInProgress = true;
	if (HAL_SPI_TransmitReceive_DMA(context->SPIHandle, transmitData, buffer, readSize) != HAL_OK)
	{
		free(transmitData);
		L2HAL_Error(Generic);
	}

	L2HAL_SDCard_WaitForDataTransferCompletion(context);

	free(transmitData);
}

void L2HAL_SDCard_WaitForBusyCleared(L2HAL_SDCard_ContextStruct *context)
{
	uint8_t flags;
	do
	{
		L2HAL_SDCard_ReadData(context, &flags, 1);
	}
	while(flags != 0xFF);
}

bool L2HAL_SDCard_ReadR1(L2HAL_SDCard_ContextStruct *context, uint8_t* response)
{
	uint8_t toTransmit = 0xFF;
	uint16_t timeout = 0xFFFF;
	while(true)
	{
		L2HAL_SDCard_ReadData(context, response, 1);

		if((*response & 0b10000000) == 0) // 8th bit alwyas zero, r1 recevied
		{
			break;
		}

		timeout --;
		if (0 == timeout)
		{
			// We have timeout, mostly probably no SD-card inserted
			return false;
		}
	}

	return true;
}

uint32_t L2HAL_SDCard_ReadBlocksCount(L2HAL_SDCard_ContextStruct* context)
{
	L2HAL_SDCard_Select(context, true);

	L2HAL_SDCard_WaitForBusyCleared(context);

	/* CMD9 (SEND_CSD) command */
	const uint8_t cmd9[] = { 0x40 | 0x09 /* CMD9 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
	L2HAL_SDCard_WriteDataNoCSControl(context, cmd9, sizeof(cmd9));

	uint8_t r1Response;
	if (!L2HAL_SDCard_ReadR1(context, &r1Response))
	{
		L2HAL_SDCard_Select(context, false);
		L2HAL_Error(Generic);
	}

	if (r1Response != 0x00)
	{
		L2HAL_SDCard_Select(context, false);
		L2HAL_Error(Generic);
	}

	L2HAL_SDCard_WaitForToken(context, L2HAL_SDCARD_DATA_TOKEN_CMD9);

	uint8_t csd[16];
	L2HAL_SDCard_ReadData(context, csd, sizeof(csd));

	uint8_t crc[2];
	L2HAL_SDCard_ReadData(context, crc, sizeof(crc));

	L2HAL_SDCard_Select(context, false);

	// first byte is VVxxxxxxxx where VV is csd.version
	if((csd[0] & 0xC0) != 0x40) // csd.version != 1
	{
		L2HAL_Error(Generic);
	}

	uint32_t result = csd[7] & 0x3F; // two bits are reserved
	result = (result << 8) | csd[8];
	result = (result << 8) | csd[9];

	// Full volume: (C_SIZE+1)*512KByte == (C_SIZE+1)<<19
	// Block size: 512Byte == 1<<9
	// Blocks number: CARD_SIZE/BLOCK_SIZE = (C_SIZE+1)*(1<<19) / (1<<9) = (C_SIZE+1)*(1<<10)
	result = (result + 1) << 10;

	return result;
}

void L2HAL_SDCard_WaitForToken(L2HAL_SDCard_ContextStruct *context, uint8_t token)
{
	uint8_t byteBuffer;

	while(true)
	{
		L2HAL_SDCard_ReadData(context, &byteBuffer, sizeof(byteBuffer));

		if (byteBuffer == token)
		{
			return;
		}

		if (byteBuffer != 0xFF)
		{
			L2HAL_Error(Generic); /* We can have either 0xFF or token */
		}
	}
}

void L2HAL_SDCard_ReadSingleBlock(L2HAL_SDCard_ContextStruct* context, uint32_t blockNumber, uint8_t* buffer)
{
	L2HAL_SDCard_Select(context, true);

	L2HAL_SDCard_WaitForBusyCleared(context);

	/* CMD17 (SEND_SINGLE_BLOCK) command */
	uint8_t cmd17[] =
	{
		0x40 | 0x11 /* CMD17 */,
		(blockNumber >> 24) & 0xFF, /* ARG */
		(blockNumber >> 16) & 0xFF,
		(blockNumber >> 8) & 0xFF,
		blockNumber & 0xFF,
		(0x7F << 1) | 1 /* CRC7 + end bit */
	};

	L2HAL_SDCard_WriteDataNoCSControl(context, cmd17, sizeof(cmd17));

	uint8_t r1Response;
	if (!L2HAL_SDCard_ReadR1(context, &r1Response))
	{
		L2HAL_SDCard_Select(context, false);
		L2HAL_Error(Generic);
	}

	if (r1Response != 0x00)
	{
		L2HAL_SDCard_Select(context, false);
		L2HAL_Error(Generic);
	}

	L2HAL_SDCard_WaitForToken(context, L2HAL_SDCARD_DATA_TOKEN_CMD17);

	L2HAL_SDCard_ReadData(context, buffer, L2HAL_SDCARD_BLOCK_SIZE);

	uint8_t crc[2];
	L2HAL_SDCard_ReadData(context, crc, sizeof(crc));

	L2HAL_SDCard_Select(context, false);
}

void L2HAL_SDCard_WaitForDataTransferCompletion(L2HAL_SDCard_ContextStruct *context)
{
	while (context->IsDataTransferInProgress) {} /* First wait for DMA completion */
	while (HAL_SPI_GetState(context->SPIHandle) != HAL_SPI_STATE_READY) { } /* Then wait for SPI ready*/
}

void L2HAL_SDCard_MarkDataTransferAsCompleted(L2HAL_SDCard_ContextStruct *context)
{
	context->IsDataTransferInProgress = false;
}

void L2HAL_SDCard_WriteSingleBlock(L2HAL_SDCard_ContextStruct* context, uint32_t blockNumber, uint8_t* buffer)
{
	L2HAL_SDCard_Select(context, true);

	L2HAL_SDCard_WaitForBusyCleared(context);

	/* CMD24 (WRITE_BLOCK) command */
	uint8_t cmd24[] =
	{
		0x40 | 0x18 /* CMD24 */,
		(blockNumber >> 24) & 0xFF, /* ARG */
		(blockNumber >> 16) & 0xFF,
		(blockNumber >> 8) & 0xFF,
		blockNumber & 0xFF,
		(0x7F << 1) | 1 /* CRC7 + end bit */
	};

	L2HAL_SDCard_WriteDataNoCSControl(context, cmd24, sizeof(cmd24));

	uint8_t r1Response;
	if (!L2HAL_SDCard_ReadR1(context, &r1Response))
	{
		L2HAL_SDCard_Select(context, false);
		L2HAL_Error(Generic);
	}

	if (r1Response != 0x00)
	{
		L2HAL_SDCard_Select(context, false);
		L2HAL_Error(Generic);
	}

	uint8_t dataToken = L2HAL_SDCARD_DATA_TOKEN_CMD24;
	L2HAL_SDCard_WriteDataNoCSControl(context, &dataToken, sizeof(dataToken));

	L2HAL_SDCard_WriteDataNoCSControl(context, buffer, L2HAL_SDCARD_BLOCK_SIZE);

	uint8_t crc[2] = { 0xFF, 0xFF };
	L2HAL_SDCard_WriteDataNoCSControl(context, crc, sizeof(crc));

	/*
		dataResp:
		xxx0abc1
			010 - Data accepted
			101 - Data rejected due to CRC error
			110 - Data rejected due to write error
	*/
	uint8_t dataResponse;
	L2HAL_SDCard_ReadData(context, &dataResponse, sizeof(dataResponse));
	if((dataResponse & 0x1F) != 0x05)
	{
		/* Data rejected */
		L2HAL_SDCard_Select(context, false);
		L2HAL_Error(Generic);
	}

	L2HAL_SDCard_WaitForBusyCleared(context);

	L2HAL_SDCard_Select(context, false);
}
