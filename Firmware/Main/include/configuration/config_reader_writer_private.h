/*
 * config_reader_writer_private.h
 *
 *  Created on: Jul 19, 2025
 *      Author: Shakti
 */

#ifndef INCLUDE_CONFIGURATION_CONFIG_READER_WRITER_PRIVATE_H_
#define INCLUDE_CONFIGURATION_CONFIG_READER_WRITER_PRIVATE_H_

#include <stdbool.h>

/**
 * Add new byte to buffer, reallocating it and returning new address
 */
char* ConfigAddByteToArray(char* buffer, uint32_t size, char byteToAdd);

/**
 * Returns true if line contains key=value pair with given key and is not commented-out
 */
bool IsKeyValuePairByKey(char* buffer, char* key);

/**
 * Result value if buffer contains key=value pair, otherwise \0
 */
void ConfigGetValueFromPair(char* buffer, char* key, char* result);



#endif /* INCLUDE_CONFIGURATION_CONFIG_READER_WRITER_PRIVATE_H_ */
