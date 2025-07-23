/*
 * bluetooth.c
 *
 *  Created on: Jul 23, 2025
 *      Author: Shakti
 */

#include "../../include/bluetooth/bluetooth.h"
#include "../../include/hal.h"
#include "../../include/constants/addresses.h"
#include "../../include/constants/paths.h"

BluetoothContextStruct BluetoothSetup(char* configPath)
{
	BluetoothContextStruct context;

	FMGL_ConsoleAddLine(&Console, "Loading bluetooth settings:");
	FMGL_ConsoleAddLine(&Console, CONSTANTS_PATHS_BLUETOOTH_CONFIG);

	context.BluetoothConfigContext = ConfigLoad
	(
		configPath,
		CONSTANTS_ADDRESSES_BLUETOOTH_CONFIG_BASE_ADDRESS,
		&RamContext,
		(void (*)(void*, uint32_t, uint32_t, uint8_t*))&L2HAL_LY68L6400_MemoryRead,
		(void (*)(void*, uint32_t, uint32_t, uint8_t*))&L2HAL_LY68L6400_MemoryWrite
	);

	bool isSuccess;
	char buffer[32];

	/* Name */
	context.Name = ConfigGetStringValueByKey
	(
		&context.BluetoothConfigContext,
		CONSTATNS_BLUETOOTH_NAME_CONFIG_KEY,
		&isSuccess
	);

	if (!isSuccess)
	{
		FMGL_ConsoleAddLine(&Console, "Can't find device name setting!");
		L2HAL_Error(Generic);
	}

	sprintf(buffer, "Name: %s", context.Name);
	FMGL_ConsoleAddLine(&Console, buffer);

	/* Pin */
	context.Pin = ConfigGetStringValueByKey
	(
		&context.BluetoothConfigContext,
		CONSTATNS_BLUETOOTH_PIN_CONFIG_KEY,
		&isSuccess
	);

	if (!isSuccess)
	{
		FMGL_ConsoleAddLine(&Console, "Can't find device PIN setting!");
		L2HAL_Error(Generic);
	}

	sprintf(buffer, "PIN: %s", context.Pin);
	FMGL_ConsoleAddLine(&Console, buffer);

	FMGL_ConsoleAddLine(&Console, "Setting-up bluetooth module...");

	HAL_UART1_DeInit();
	HAL_UART1_Init(CONSTANTS_BLUETOOTH_FULL_SPEED_BAUDRATE);

	BluetoothModuleContext = L2HAL_HC06_AttachToDevice(&UART1Handle);
	if (!BluetoothModuleContext.IsFound)
	{
		FMGL_ConsoleAddLine(&Console, "Normal mode failed, trying first power on setup...");

		BluetoothFactorySetup(CONSTANTS_BLUETOOTH_FULL_SPEED_MODE, context.Name, context.Pin);

		FMGL_ConsoleAddLine(&Console, "Success, returning to normal mode...");

		HAL_UART1_DeInit();
		HAL_UART1_Init(CONSTANTS_BLUETOOTH_FULL_SPEED_BAUDRATE);

		BluetoothModuleContext = L2HAL_HC06_AttachToDevice(&UART1Handle);
		if (!BluetoothModuleContext.IsFound)
		{
			/* Switch to fullspeed mode failed */
			FMGL_ConsoleAddLine(&Console, "FAILED");
			L2HAL_Error(Generic);
		}
	}

	FMGL_ConsoleAddLine(&Console, "Success");

	return context;
}

void BluetoothFactorySetup
(
	enum L2HAL_HC06_BAUDRARTE_MODE baudrate,
	const char* name,
	const char* pin
)
{
	HAL_UART1_DeInit();
	HAL_UART1_Init(HAL_BLUETOOTH_FACTORY_SPEED_BAUDRATE);

	BluetoothModuleContext = L2HAL_HC06_AttachToDevice(&UART1Handle);
	if (!BluetoothModuleContext.IsFound)
	{
		/* Bluetooth really failed */
		FMGL_ConsoleAddLine(&Console, "Bluetooth module hardware failure!");
		L2HAL_Error(Generic);
	}

	/* Initial setup */
	L2HAL_HC06_SetName(&BluetoothModuleContext, name);
	L2HAL_HC06_SetPIN(&BluetoothModuleContext, pin);

	L2HAL_HC06_SetBaudrate(&BluetoothModuleContext, baudrate);

	HAL_Delay(CONSTATNS_BLUETOOTH_AFTER_BAUDRATE_CHANGE_DELAY);
}
