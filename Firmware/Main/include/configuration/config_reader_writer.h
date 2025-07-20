/*
 * config_reader_writer.h
 *
 *  Created on: Jul 19, 2025
 *      Author: Shakti
 */

#ifndef INCLUDE_CONFIGURATION_CONFIG_READER_WRITER_H_
#define INCLUDE_CONFIGURATION_CONFIG_READER_WRITER_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * Read config by this blocks
 */
#define CONFIG_READ_BLOCK_SIZE 512

/**
 * Path to config max length (including \0)
 */
#define CONFIG_MAX_PATH_LENGTH 256

/**
 * Config file context struct
 */
typedef struct
{
	/**
	 * Path to config file
	 */
	char Path[CONFIG_MAX_PATH_LENGTH];

	/**
	 * Loaded config file base address in external RAM
	 */
	uint32_t BaseAddress;

	/**
	 * Config file size in bytes
	 */
	uint32_t ConfigSize;

	/**
	 * External RAM driver context
	 */
	void* MemoryDriverContext;

	/**
	 * External RAM read function
	 */
	void (*MemoryReadFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*);

	/**
	 * External RAM write function
	 */
	void (*MemoryWriteFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*);

} ConfigContextStruct;

/**
 * Load config from file to external memory
 */
ConfigContextStruct ConfigLoad
(
	char* path,
	uint32_t baseAddress,
	void* memoryDriverContext,
	void (*memoryReadFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*),
	void (*memoryWriteFunctionPtr)(void*, uint32_t, uint32_t, uint8_t*)
);


/* Try to find string value in external memory located config */
char* ConfigGetStringValueByKey
(
	ConfigContextStruct* context,
	char* key,
	bool* isFound
);

/* Try to find integer value in external memory located config */
int32_t ConfigGetIntValueByKey
(
	ConfigContextStruct* context,
	char* key,
	bool* isFound
);


#endif /* INCLUDE_CONFIGURATION_CONFIG_READER_WRITER_H_ */
