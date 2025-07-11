/*
	This file is part of Shakti Lucidia's STM32 level 2 HAL.

	STM32 level 2 HAL is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	-------------------------------------------------------------------------

	Created by Shakti Lucidia

	Feel free to contact: shakti_lucidia@proton.me

	Repository: https://github.com/shaktilucidia/stm32-l2hal

	-------------------------------------------------------------------------
 */

#include <stdlib.h>
#include "../include/l2hal_buttons_private.h"

void L2HAL_Buttons_SysTickHandler(void)
{
	/* Decreasing all buttons debouncing timers */
	for (uint8_t contextIndex = 0; contextIndex < L2HAL_Buttons_Metacontext.ContextsCount; contextIndex ++)
	{
		L2HAL_Buttons_ContextStruct* context = L2HAL_Buttons_Metacontext.Contexts[contextIndex];

		for (uint8_t buttonIndex = 0; buttonIndex < context->ButtonsCount; buttonIndex ++)
		{
			L2HAL_Buttons_ButtonStruct* button = &(context->Buttons[buttonIndex]);

			if (0 == button->DebouncingTimer)
			{
				continue;
			}

			button->DebouncingTimer --;
		}
	}
}

void L2HAL_Buttons_Init(L2HAL_Buttons_ContextStruct* context)
{
	/* Updating metacontext */
	if (0 == L2HAL_Buttons_Metacontext.ContextsCount)
	{
		L2HAL_Buttons_Metacontext.Contexts = malloc(sizeof(void*));
	}
	else
	{
		L2HAL_Buttons_Metacontext.Contexts = realloc(L2HAL_Buttons_Metacontext.Contexts, (L2HAL_Buttons_Metacontext.ContextsCount + 1) * sizeof(void*));
	}

	context->ButtonsCount = 0;
	context->Buttons = malloc(0);

	context->IsPortDataCanBeUpdated = true;

	if (NULL == context->CustomPinInitializer)
	{
		context->CustomPinInitializer = L2HAL_Buttons_DefaultPinInitializer;
	}

	/* Saving context into metacontext */
	L2HAL_Buttons_Metacontext.Contexts[L2HAL_Buttons_Metacontext.ContextsCount] = context;
	L2HAL_Buttons_Metacontext.ContextsCount ++;

	/* Initial ports states */
	L2HAL_Buttons_GetPortsData(context);

	if (!L2HAL_Buttons_Metacontext.IsInitialized)
	{
		/* Initializing SysTick handler */
		L2HAL_SysTick_RegisterHandler(&L2HAL_Buttons_SysTickHandler);

		L2HAL_Buttons_Metacontext.IsInitialized = true;
	}
}

void L2HAL_Buttons_GetPortsData(L2HAL_Buttons_ContextStruct* context)
{
	/* Polling ports (even non-initialized) */
	for (uint8_t portIndex = 0; portIndex < L2HAL_MAX_PORTS_COUNT; portIndex ++)
	{
		context->PortsData[portIndex] = L2HAL_IndexesToPorts[portIndex]->IDR & L2HAL_BUTTONS_PORT_DATA_MASK;
	}
}

uint8_t L2HAL_Buttons_GetPortIndex(GPIO_TypeDef* port)
{
	for (uint8_t portIndex = 0; portIndex < L2HAL_MAX_PORTS_COUNT; portIndex ++)
	{
		if (port == L2HAL_IndexesToPorts[portIndex])
		{
			return portIndex;
		}
	}

	L2HAL_Error(WrongArgument);
}

L2HAL_Buttons_ButtonStruct* L2HAL_Buttons_AddButton(L2HAL_Buttons_ContextStruct* context,
		GPIO_TypeDef* port,
		uint16_t pin,
		void* arbitraryContextPointer,
		uint16_t debouncingInterval,
		void (*buttonEventsCallback)(void*, GPIO_PinState, uint16_t*))
{
	/* Getting port index and checking if such button already exist? */
	uint8_t portIndex = L2HAL_Buttons_GetPortIndex(port);

	for (uint8_t buttonIndex = 0; buttonIndex < context->ButtonsCount; buttonIndex ++)
	{
		L2HAL_Buttons_ButtonStruct* currentButton = &context->Buttons[buttonIndex];

		if ((currentButton->PortIndex == portIndex) && (currentButton->Pin == pin))
		{
			/* Button already exists */
			L2HAL_Error(WrongArgument);
		}
	}

	/* Allocating memory for new button */
	context->Buttons = realloc(context->Buttons, (context->ButtonsCount + 1) * sizeof(L2HAL_Buttons_ButtonStruct));
	if (NULL == context->Buttons)
	{
		/* Realloc failed */
		L2HAL_Error(Generic);
	}

	/* Filling structure*/
	L2HAL_Buttons_ButtonStruct* newButton = &context->Buttons[context->ButtonsCount];
	newButton->PortIndex = portIndex;
	newButton->Pin = pin;
	newButton->ButtonEventsCallback = buttonEventsCallback;
	newButton->ArbitraryContextPointer = arbitraryContextPointer;
	newButton->DebouncingInterval = debouncingInterval;
	newButton->DebouncingTimer = 0;

	/* Initializing port */
	if (!context->SkipPinsInitialization)
	{
		context->CustomPinInitializer(port, pin);
	}

	/* Updating port data to get current button state */
	L2HAL_Buttons_GetPortsData(context);

	/* Setting button initial state */
	newButton->PreviousState = L2HAL_Buttons_GetButtonState(newButton, context->PortsData);

	/* Increasing counter lately to be interrupt-safe (if we increase it early, then it is possible that interrupt will came between
	 * increase and new button setup */
	context->ButtonsCount ++;

	return newButton;
}

GPIO_PinState L2HAL_Buttons_GetButtonState(L2HAL_Buttons_ButtonStruct* button, uint16_t* portsData)
{
	if (0x00 != (portsData[button->PortIndex] & button->Pin))
	{
		return GPIO_PIN_SET;
	}
	else
	{
		return GPIO_PIN_RESET;
	}
}



void L2HAL_Buttons_Poll(L2HAL_Buttons_ContextStruct* context)
{
	/* Waiting for all handlers to complete */
	while (!context->IsPortDataCanBeUpdated) {}

	/* Updating ports data */
	L2HAL_Buttons_GetPortsData(context);

	/* For each button detecting new state and calling handlers in case of difference */
	context->IsPortDataCanBeUpdated = false;
	for (uint8_t buttonIndex = 0; buttonIndex < context->ButtonsCount; buttonIndex ++)
	{
		L2HAL_Buttons_ButtonStruct* buttonStruct = &context->Buttons[buttonIndex];

		if (0 != buttonStruct->DebouncingTimer)
		{
			continue;
		}

		GPIO_PinState newState = L2HAL_Buttons_GetButtonState(buttonStruct, context->PortsData);

		if (newState != buttonStruct->PreviousState)
		{
			/* Calling handler if needed */
			if (NULL != buttonStruct->ButtonEventsCallback)
			{
				buttonStruct->ButtonEventsCallback(buttonStruct, newState, context->PortsData);
			}

			/* Updating state */
			buttonStruct->PreviousState = newState;
			buttonStruct->DebouncingTimer = buttonStruct->DebouncingInterval;
		}
	}
	context->IsPortDataCanBeUpdated = true;
}



