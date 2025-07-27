/*
 * low_level_packets_processor.h
 *
 *  Created on: Jul 26, 2025
 *      Author: Shakti
 */

#ifndef INCLUDE_PACKETS_PROCESSOR_LOW_LEVEL_PACKETS_PROCESSOR_H_
#define INCLUDE_PACKETS_PROCESSOR_LOW_LEVEL_PACKETS_PROCESSOR_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Call it before working with packets processor
 * @param onPacketReceived Pointer to function, called when new correct packet received. !! FUNCTION CALLED IN UART INTERRUPT CONTEXT !!
 * @param payload Payload (packet length and CRC are stripped). This pointer valid only until exit from function
 * @param payloadLength Payload length
 */
void LLPP_Init(void (*onPacketReceived) (uint8_t* payload, uint8_t payloadLength));

/**
 * Call it to start to listen for incoming packets.
 */
void LLPP_StartListen(void);

/**
 * Cancels listening
 */
void LLPP_AbortListen(void);

/**
 * Send packet
 */
void LLPP_Send(uint8_t* payload, uint8_t payloadLength);

#endif /* INCLUDE_PACKETS_PROCESSOR_LOW_LEVEL_PACKETS_PROCESSOR_H_ */
