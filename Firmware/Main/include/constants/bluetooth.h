/*
 * bluetooth.h
 *
 *  Created on: Jul 23, 2025
 *      Author: Shakti
 */

#ifndef INCLUDE_CONSTANTS_BLUETOOTH_H_
#define INCLUDE_CONSTANTS_BLUETOOTH_H_

#include "../../libs/l2hal/l2hal_config.h"

/**
 * Constants, related to bluetooth
 */

/**
 * Factory UART speed
 */
#define CONSTANTS_BLUETOOTH_FACTORY_SPEED_BAUDRATE 9600

/**
 * Full UART speed
 */
#define CONSTANTS_BLUETOOTH_FULL_SPEED_BAUDRATE 115200

/**
 * HC-06 bluetooth module mode, corresponding to CONSTANTS_BLUETOOTH_FULL_SPEED_BAUDRATE
 */
#define CONSTANTS_BLUETOOTH_FULL_SPEED_MODE L2HAL_HC06_BAUDRARTE_MODE_115200

/**
 * Wait this time (in ms) after module baudrate change
 */
#define CONSTATNS_BLUETOOTH_AFTER_BAUDRATE_CHANGE_DELAY 1000

/**
 * Store device name in this config KVP
 */
#define CONSTATNS_BLUETOOTH_NAME_CONFIG_KEY "name"

/**
 * Store device pin in this config KVP
 */
#define CONSTATNS_BLUETOOTH_PIN_CONFIG_KEY "pin"


#endif /* INCLUDE_CONSTANTS_BLUETOOTH_H_ */
