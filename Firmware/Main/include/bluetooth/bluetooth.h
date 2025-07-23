/*
 * bluetooth.h
 *
 *  Created on: Jul 23, 2025
 *      Author: Shakti
 */

#ifndef INCLUDE_BLUETOOTH_BLUETOOTH_H_
#define INCLUDE_BLUETOOTH_BLUETOOTH_H_

#include "../constants/bluetooth.h"
#include "../../libs/l2hal/fmgl/console/include/console.h"
#include "../configuration/config_reader_writer.h"

extern UART_HandleTypeDef UART1Handle;
extern L2HAL_HC06_ContextStruct BluetoothModuleContext;
extern FMGL_Console_ContextStruct Console;

/**
 * Bluetooth settings are stored here
 */
typedef struct
{
	/**
	 * Bluetooth config file context
	 */
	ConfigContextStruct BluetoothConfigContext;

	/**
	 * Device name
	 */
	char* Name;

	/**
	 * Pin
	 */
	char* Pin;


} BluetoothContextStruct;


/**
 * Setup bluetooth
 */
BluetoothContextStruct BluetoothSetup(char* configPath);

/**
 * Set bluetooth module baudrate, name and pin
 */
void BluetoothFactorySetup
(
	enum L2HAL_HC06_BAUDRARTE_MODE baudrate,
	const char* name,
	const char* pin
);

#endif /* INCLUDE_BLUETOOTH_BLUETOOTH_H_ */
