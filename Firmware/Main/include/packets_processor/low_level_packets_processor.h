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
 */
void LLPP_Init(void);

/**
 * Call it to start to listen for incoming packets.
 */
void LLPP_StartListen(void);

/**
 * Cancels listening
 */
void LLPP_AbortListen(void);


#endif /* INCLUDE_PACKETS_PROCESSOR_LOW_LEVEL_PACKETS_PROCESSOR_H_ */
