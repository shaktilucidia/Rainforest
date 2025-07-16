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

#include "../include/fmgl_private.h"
#include "../../include/l2hal_errors.h"
#include <stdlib.h>

bool FMGL_Priv_IsActiveXBMPixel(FMGL_API_XBMImage* image, uint16_t x, uint16_t y)
{
	if (x >= image->Width || y >= image->Height)
	{
		return false;
	}

	uint16_t bytesPerRow = image->Width / FMGL_PRIV_BITS_PER_BYTE; /* Round down */
	if (0 != (image->Width % FMGL_PRIV_BITS_PER_BYTE)) /* Do we have non completely filled byte at rightmost position in row*/
	{
		bytesPerRow ++;
	}

	uint32_t index = y * bytesPerRow + x / FMGL_PRIV_BITS_PER_BYTE;
	uint8_t mask = (1 << (x % FMGL_PRIV_BITS_PER_BYTE));

	if (0 != (image->Raster[index] & mask))
	{
		return true; /* To be sure that 0xFF is returned */
	}
	else
	{
		return false;
	}
}

void FMGL_Priv_RenderCharacter(FMGL_API_DriverContext* context, FMGL_API_FontSettings* fontSettings, uint16_t x, uint16_t y, char character)
{
	/* Generating XBM image structure */
	FMGL_API_XBMImage characterImage;
	characterImage.Height = fontSettings->Font->Height;
	characterImage.Width = fontSettings->Font->GetCharacterWidth(fontSettings->Font->Context, (uint8_t)character);
	characterImage.Raster = fontSettings->Font->GetCharacterRaster(fontSettings->Font->Context, (uint8_t)character);

	FMGL_API_RenderXBM(context, &characterImage, x, y, fontSettings->Scale, fontSettings->Scale, *fontSettings->FontColor, *fontSettings->BackgroundColor, *fontSettings->Transparency);

	if (fontSettings->Font->IsLoadable)
	{
		free(characterImage.Raster);
	}
}

void FMGL_Priv_RenderSubstring(FMGL_API_DriverContext* context, FMGL_API_FontSettings* fontSettings, uint16_t startPos, uint16_t length,
		uint16_t x, uint16_t* y, uint16_t scaledLineHeight, uint16_t* width, bool isDryRun, char* string)
{
	if (length > FMGL_PRIV_MAX_SUBSTRING_LENGTH)
	{
		L2HAL_Error(Generic);
	}

	char substr[FMGL_PRIV_MAX_SUBSTRING_LENGTH + 1];
	memcpy(substr, string + startPos, length);
	substr[length] = '\0';

	uint16_t currWidth = 0;
	FMGL_API_RenderOneLineDumb(context, fontSettings, x, *y, &currWidth, isDryRun, substr);

	if (currWidth > *width)
	{
		*width = currWidth;
	}

	*y += scaledLineHeight;
}
