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

#include "../include/terminusRegular12.h"

uint16_t FMGL_FontTerminusRegular12GetCharacterWidth (uint8_t character)
{
	return FMGL_FONT_TERMINUS_REGULAR_12_CHARACTER_WIDTH;
}

const uint8_t* FMGL_FontTerminusRegular12GetCharacterRaster(uint8_t character)
{
	if (character < FMGL_FONT_TERMINUS_REGULAR_12_FIRST_CHARACTER_CODE)
	{
		return FMGL_FontTerminusRegular12_WrongCharacterCode;
	}

	return FMGL_FontTerminusRegular12Characters[character - FMGL_FONT_TERMINUS_REGULAR_12_FIRST_CHARACTER_CODE];
}

FMGL_API_Font FMGL_FontTerminusRegular12Init(void)
{
	FMGL_API_Font font;

	font.Height = FMGL_FONT_TERMINUS_REGULAR_12_CHARACTER_HEIGHT;
	font.GetCharacterWidth = &FMGL_FontTerminusRegular12GetCharacterWidth;
	font.GetCharacterRaster = &FMGL_FontTerminusRegular12GetCharacterRaster;

	return font;
}
