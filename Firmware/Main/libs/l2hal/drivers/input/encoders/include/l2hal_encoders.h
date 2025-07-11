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

/**
 * @file
 * @brief Incremental encoder level 2 HAL driver.
 */

#ifndef L2HAL_DRIVERS_INPUT_ENCODERS_INCLUDE_L2HAL_ENCODERS_H_
#define L2HAL_DRIVERS_INPUT_ENCODERS_INCLUDE_L2HAL_ENCODERS_H_

#include <stdlib.h>
#include "../..//buttons/include/l2hal_buttons.h"

/**
 * Encoder rotation direction.
 */
typedef enum
{
	Clockwise,
	CounterClockwise
} L2HAL_Encoders_Direction;

/**
 * Structure, describing one incremental encoder.
 */
typedef struct
{
	/**
	 * Encoder channel, which becomes active first when rotated clockwise.
	 */
	L2HAL_Buttons_ButtonStruct* LeadingChannel;

	/**
	 * Encoder channel, which becomes active first when rotated counter-clockwise.
	 */
	L2HAL_Buttons_ButtonStruct* TrailingChannel;

	/**
	 * This level on input channels is considered active. GPIO_PIN_RESET is encoder pulls input to ground when active, GPIO_PIN_SET if it pulls input to
	 * logical one level.
	 */
	GPIO_PinState ActiveState;

	/**
	 * When calling L2HAL_Encoders_AddEncoder() user can provide pointer to arbitrary context (for example to code, which owns encoder).
	 * This pointer is stored here.
	 */
	void* ArbitraryContextPointer;

	/**
	 * Encoder callback handler. Parameters are pointer to L2HAL_Encoders_EncoderStruct for given encoder and rotation direction.
	 */
	void (*EncoderEventsCallback)(void*, L2HAL_Encoders_Direction);

} L2HAL_Encoders_EncoderStruct;


/**
 * Driver context.
 */
typedef struct
{
	/**
	 * Pointer to instance of buttons driver.
	 */
	L2HAL_Buttons_ContextStruct* ButtonsDriverContext;

	/**
	 * Number of attached encoders.
	 */
	uint8_t EncodersCount;

	/**
	 * Attached encoders.
	 */
	L2HAL_Encoders_EncoderStruct* Encoders;

} L2HAL_Encoders_ContextStruct;


/**
 * Initialize encoders driver. Feed it with pointer to existing context. In context you have to set up pointer to initialized buttons driver
 * (buttonsDriverContext);
 * @param context Pointer to encoders driver context.
 */
void L2HAL_Encoders_Init(L2HAL_Encoders_ContextStruct* context);


/**
 * Call this function to add new encoder. Function don't check itself if given pins are busy or not, but underlaying buttons driver does check.
 * @param context Pointer to driver context.
 * @param leadingPort Port, to what leading encoder channel is connected (see L2HAL_Encoders_EncoderStruct for explaination of terms "leading"
 * and "trailing".
 * @param leadingPin Pin in leading port, to what leading encoder channel is connected.
 * @param trailingPort Port, to what trailing encoder channel is connected.
 * @param trailingPin Pin in trailing port, to what leading encoder channel is connected.
 * @param activeState Active pins state (see L2HAL_Encoders_EncoderStruct for details).
 * @param leadingPinDebouncingInterval Debouncing interval for leading pin (see L2HAL_Buttons_AddButton).
 * @param encoderEventsCallback Pointer to function, which will be called when is being encoder turned.
 * @param arbitraryContextPointer You can provide here pointer to something (for example to code, what owns encoder). This pointer will be stored within encoder
 * struct and so will be available in encoder callback handler.
 * @return Pointer to created encoder struct.
 */
L2HAL_Encoders_EncoderStruct* L2HAL_Encoders_AddEncoder(L2HAL_Encoders_ContextStruct* context,
		GPIO_TypeDef* leadingPort,
		uint16_t leadingPin,
		GPIO_TypeDef* trailingPort,
		uint16_t trailingPin,
		GPIO_PinState activeState,
		uint16_t leadingPinDebouncingInterval,
		void* arbitraryContextPointer,
		void (*encoderEventsCallback)(void*, L2HAL_Encoders_Direction));

#endif /* L2HAL_DRIVERS_INPUT_ENCODERS_INCLUDE_L2HAL_ENCODERS_H_ */
