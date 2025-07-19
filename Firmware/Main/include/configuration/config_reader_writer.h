/*
 * config_reader_writer.h
 *
 *  Created on: Jul 19, 2025
 *      Author: Shakti
 */

#ifndef INCLUDE_CONFIGURATION_CONFIG_READER_WRITER_H_
#define INCLUDE_CONFIGURATION_CONFIG_READER_WRITER_H_

#include <stdint.h>

/**
 * Read config by this blocks
 */
#define CONFIG_READ_BLOCK_SIZE 512

/* Try to find value in external memory located config. Will return NULL if not found */
char* ConfigGetValueByKey
(
	char* key,
	uint32_t baseAddress,
	uint32_t configSize,
	void* memoryDriverContext,
	void (*memoryReadFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*)
);


#endif /* INCLUDE_CONFIGURATION_CONFIG_READER_WRITER_H_ */
