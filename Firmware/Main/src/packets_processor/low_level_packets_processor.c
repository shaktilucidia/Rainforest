/*
 * low_level_packets_processor.c
 *
 *  Created on: Jul 26, 2025
 *      Author: Shakti
 */

#include "../../include/packets_processor/low_level_packets_processor.h"
#include "../../include/packets_processor/low_level_packets_processor_private.h"
#include <stdlib.h>

void LLPP_Init(void (*onPacketReceived) (uint8_t* payload, uint8_t payloadLength))
{
	LLPP_OnPacketReceivedPtr = onPacketReceived;

	LLPP_State = LLPP_STATE_NOT_LISTEN;

	L2HAL_SysTick_RegisterHandler(&LLPP_Tick);
}

void LLPP_Tick(void)
{
	if (LLPP_PacketRxTimeoutTimer > 0)
		{
		LLPP_PacketRxTimeoutTimer --;
		}
		else
		{
			if (LLPP_STATE_IN_PROGRESS == LLPP_State)
			{
				/* Timeout, aborting ongoing packet reception. */
				LLPP_State = LLPP_STATE_LISTEN;

				LLPP_FreePacketRxBuffer();

				LLPP_PacketRxBufferIndex = 0;
			}
		}
}

void LLPP_StartListen(void)
{
	LLPP_State = LLPP_STATE_LISTEN;
	LLPP_PacketRxBufferIndex = 0;
	LLPP_PacketRxTimeoutTimer = LLPP_PACKET_NEXT_BYTE_TIMEOUT;

	LLPP_AskForNextByte();
}

void LLPP_AbortListen(void)
{
	if (HAL_UART_AbortReceive(&UART1Handle) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}

	LLPP_State = LLPP_STATE_NOT_LISTEN;

	if (NULL != LLPP_PacketRxBuffer)
	{
		LLPP_FreePacketRxBuffer();
	}
}

void LLPP_AskForNextByte(void)
{
	if(HAL_UART_Receive_IT(&UART1Handle, &LLPP_RxByteBuffer, 1) != HAL_OK)
	{
		L2HAL_Error(Generic);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if (USART1 != UartHandle->Instance)
	{
		return;
	}

	switch (LLPP_State)
	{
		case LLPP_STATE_NOT_LISTEN:
			return;

		case LLPP_STATE_LISTEN:

			/* First byte came */
			LLPP_PacketRxTimeoutTimer = LLPP_PACKET_NEXT_BYTE_TIMEOUT;

			LLPP_ExpectedPacketLength = LLPP_RxByteBuffer; /* Packet length is always in first byte*/

			/* Checking packet length */
			if (LLPP_ExpectedPacketLength < LLPP_PACKET_MIN_SIZE || LLPP_ExpectedPacketLength > LLPP_PACKET_MAX_SIZE)
			{
				LLPP_AskForNextByte();
				return; /* Invalid packet */
			}

			LLPP_PacketRxBufferIndex = 0;
			LLPP_PacketRxBuffer = malloc(LLPP_ExpectedPacketLength);
			LLPP_PacketRxBuffer[LLPP_PacketRxBufferIndex] = LLPP_RxByteBuffer;


			/* Moving to next state*/
			LLPP_PacketRxBufferIndex ++;
			LLPP_State = LLPP_STATE_IN_PROGRESS;

			LLPP_AskForNextByte();

			break;

		case LLPP_STATE_IN_PROGRESS:
			LLPP_PacketRxTimeoutTimer = LLPP_PACKET_NEXT_BYTE_TIMEOUT;
			LLPP_PacketRxBuffer[LLPP_PacketRxBufferIndex] = LLPP_RxByteBuffer;
			LLPP_PacketRxBufferIndex ++;

			if (LLPP_PacketRxBufferIndex == LLPP_ExpectedPacketLength)
			{
				/* We have a new packet */

				/* CRC check */
				volatile uint32_t calculatedCrc = L2HAL_CRC_Calculate(&CrcContext, LLPP_PacketRxBuffer, LLPP_ExpectedPacketLength - sizeof(uint32_t));
				volatile uint32_t expectedCrc = *(uint32_t*)&LLPP_PacketRxBuffer[LLPP_ExpectedPacketLength - sizeof(uint32_t)];

				if (calculatedCrc != expectedCrc)
				{
					/* Wrong CRC, dropping packet */
					LLPP_State = LLPP_STATE_LISTEN;

					LLPP_FreePacketRxBuffer();
					LLPP_PacketRxBufferIndex = 0;

					LLPP_AskForNextByte();

					return;
				}

				LLPP_State = LLPP_STATE_LISTEN;

				uint8_t payloadLength = LLPP_ExpectedPacketLength - LLPP_PACKET_PAYLOAD_DELTA;
				uint8_t* payload = malloc(payloadLength);

				memcpy(payload, (uint8_t*)&LLPP_PacketRxBuffer[1], payloadLength);

				LLPP_FreePacketRxBuffer();
				LLPP_PacketRxBufferIndex = 0;

				LLPP_OnPacketReceivedPtr(payload, payloadLength);

				free(payload);
			}

			LLPP_AskForNextByte();

			break;

		default:
			L2HAL_Error(Generic);
			break;
	}
}

void LLPP_FreePacketRxBuffer(void)
{
	free(LLPP_PacketRxBuffer);
	LLPP_PacketRxBuffer = NULL;
}
