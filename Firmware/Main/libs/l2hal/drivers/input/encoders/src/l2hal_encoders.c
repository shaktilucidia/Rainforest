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

#include "../include/l2hal_encoders_private.h"

void L2HAL_Encoders_ButtonsCallback(void* button, GPIO_PinState newPinState, uint16_t* portsData)
{
	/* Event happened in this encoder in leading channel */
	L2HAL_Encoders_EncoderStruct* encoderPtr = ((L2HAL_Buttons_ButtonStruct*)button)->ArbitraryContextPointer;

	if (encoderPtr->ActiveState != newPinState)
	{
		return;
	}

	/* Leading channel is just activated, what's trailing state? */
	L2HAL_Encoders_Direction direction;
	GPIO_PinState trailingState = L2HAL_Buttons_GetButtonState(encoderPtr->TrailingChannel, portsData);
	if (encoderPtr->ActiveState != trailingState)
	{
		direction = Clockwise;
	}
	else
	{
		direction = CounterClockwise;
	}

	encoderPtr->EncoderEventsCallback(encoderPtr, direction);
}

void L2HAL_Encoders_Init(L2HAL_Encoders_ContextStruct* context)
{
	context->EncodersCount = 0;
	context->Encoders = malloc(0);
}

L2HAL_Encoders_EncoderStruct* L2HAL_Encoders_AddEncoder(L2HAL_Encoders_ContextStruct* context,
		GPIO_TypeDef* leadingPort,
		uint16_t leadingPin,
		GPIO_TypeDef* trailingPort,
		uint16_t trailingPin,
		GPIO_PinState activeState,
		uint16_t leadingPinDebouncingInterval,
		void* arbitraryContextPointer,
		void (*encoderEventsCallback)(void*, L2HAL_Encoders_Direction))
{
	/* Allocating memory for new encoder */
	context->Encoders = realloc(context->Encoders, (context->EncodersCount + 1) * sizeof(L2HAL_Encoders_EncoderStruct));
	if (NULL == context->Encoders)
	{
		/* Realloc failed */
		L2HAL_Error(Generic);
	}

	/* Filling new encoder struct*/
	L2HAL_Encoders_EncoderStruct* newEncoder = &context->Encoders[context->EncodersCount];

	/* Setting-up leading channel */
	newEncoder->LeadingChannel = L2HAL_Buttons_AddButton(context->ButtonsDriverContext,
			leadingPort,
			leadingPin,
			newEncoder,
			leadingPinDebouncingInterval,
			&L2HAL_Encoders_ButtonsCallback);

	/* And trailing */
	newEncoder->TrailingChannel = L2HAL_Buttons_AddButton(context->ButtonsDriverContext,
			trailingPort,
			trailingPin,
			newEncoder,
			0,
			NULL); /* We don't need handler for trailing channel */

	newEncoder->ActiveState = activeState;
	newEncoder->ArbitraryContextPointer = arbitraryContextPointer;
	newEncoder->EncoderEventsCallback = encoderEventsCallback;

	/* Increasing counter lately to be interrupt-safe (if we increase it early, then it is possible that interrupt will came between
	* increase and new encoder setup */
	context->EncodersCount ++;

	return newEncoder;
}


