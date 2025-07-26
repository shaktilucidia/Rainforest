/*
 * low_level_packets_processor_private.h
 *
 *  Created on: Jul 26, 2025
 *      Author: Shakti
 */

#ifndef INCLUDE_PACKETS_PROCESSOR_LOW_LEVEL_PACKETS_PROCESSOR_PRIVATE_H_
#define INCLUDE_PACKETS_PROCESSOR_LOW_LEVEL_PACKETS_PROCESSOR_PRIVATE_H_

#include "low_level_packets_processor.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../libs/l2hal/l2hal_config.h"

extern UART_HandleTypeDef UART1Handle;
extern L2HAL_CRCContextStruct CrcContext;

/**
 * If packet isn't received during this time, reset state machine
 */
#define LLPP_PACKET_NEXT_BYTE_TIMEOUT 1000U

/**
 * Possible packet sizes
 */
#define LLPP_PACKET_MIN_SIZE 6U
#define LLPP_PACKET_MAX_SIZE 255U /* Actually not needed IN THIS SPECIFIC CASE, because max. length is 255 */

/**
 * Low-level packets state machine state
 */
typedef enum
{
	/**
	 * Listening for packets not initiated
	 */
	LLPP_STATE_NOT_LISTEN,

	/**
	 * Listening for packets, no bytes came yet
	 */
	LLPP_STATE_LISTEN,

	/**
	 * Some bytes came
	 */
	LLPP_STATE_IN_PROGRESS

}
LLPP_StateEnum;

/**
 * Packets state machine state
 */
LLPP_StateEnum LLPP_State;

/**
 * Rx buffer (we work byte-by-byte, so 1 byte is enough)
 */
uint8_t LLPP_RxByteBuffer;

/**
 * Packet is being accumulated here
 */
uint8_t LLPP_PacketRxBuffer[LLPP_PACKET_MAX_SIZE];

/**
 * Current byte index within LLPP_PacketRxBuffer
 */
uint8_t LLPP_PacketRxBufferIndex;

/**
 * Decreases each millisecond, when reaches 0, resets state machine to Listen state.
 */
uint16_t LLPP_PacketRxTimeoutTimer;

/**
 * If true then we have packet, ready to be processed.
 */
bool LLPP_IsPacketReady;

/**
 * When first byte of packet came, we are able to detect packet length. That length
 * is stored here
 */
uint8_t LLPP_ExpectedPacketLength;

/**
 * Full length of received packet
 */
uint8_t LLPP_ReceivedPacketFullLength;

/**
 * Received packet, ready to be processed
 */
uint8_t LLPP_ReceivedPacket[LLPP_PACKET_MAX_SIZE];

/**
 * Call this every millisecond
 */
void LLPP_Tick(void);

/**
 * Ask for next byte
 */
void LLPP_AskForNextByte(void);


#endif /* INCLUDE_PACKETS_PROCESSOR_LOW_LEVEL_PACKETS_PROCESSOR_PRIVATE_H_ */
