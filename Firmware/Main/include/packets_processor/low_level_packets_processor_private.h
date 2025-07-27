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
 * Payload length is less than full packet length by this value
 */
#define LLPP_PACKET_PAYLOAD_DELTA 5U

/**
 * Possible packet sizes
 */
#define LLPP_PACKET_MIN_SIZE (LLPP_PACKET_PAYLOAD_DELTA + 1U)
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
 * Pointer to function, called when new correct packet received. !! FUNCTION CALLED IN UART INTERRUPT CONTEXT !!
 * @param payload Payload (packet length and CRC are stripped). This pointer valid only until exit from function
 * @param payloadLength Payload length
 */
void (*LLPP_OnPacketReceivedPtr) (uint8_t* payload, uint8_t payloadLength);

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
uint8_t* LLPP_PacketRxBuffer;

/**
 * Current byte index within LLPP_PacketRxBuffer
 */
uint8_t LLPP_PacketRxBufferIndex;

/**
 * Decreases each millisecond, when reaches 0, resets state machine to Listen state.
 */
uint16_t LLPP_PacketRxTimeoutTimer;

/**
 * When first byte of packet came, we are able to detect packet length. That length
 * is stored here
 */
uint8_t LLPP_ExpectedPacketLength;

/**
 * Outgoing packet buffer
 */
uint8_t* LLPP_PacketTxBuffer;

/**
 * Outgoing packet length
 */
uint8_t LLPP_PacketTxLength;

/**
 * True if outgoing packet is being transmitted
 */
bool LLPP_PacketTxInProgress;

/**
 * Call this every millisecond
 */
void LLPP_Tick(void);

/**
 * Ask for next byte
 */
void LLPP_AskForNextByte(void);

/**
 * Free packet RX buffer and set pointer no NULL
 */
void LLPP_FreePacketRxBuffer(void);

/**
 * Free packet TX buffer and set pointer no NULL
 */
void LLPP_FreePacketTxBuffer(void);

/**
 * Send data in non-blocking mode (data copied to internal buffer on call)
 */
void LLPP_SendNonBlocking(uint8_t* data, uint8_t length);


#endif /* INCLUDE_PACKETS_PROCESSOR_LOW_LEVEL_PACKETS_PROCESSOR_PRIVATE_H_ */
