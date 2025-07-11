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
#include "../../include/l2hal_aux.h"
#include "../../include/l2hal_errors.h"

FMGL_API_DriverContext FMGL_API_AttachToDriver
(
	void* deviceContext,
	uint16_t (*getWidth) (void),
	uint16_t (*getHeight) (void),
	void (*setActiveColor) (void* deviceContext, FMGL_API_ColorStruct color),
	void (*drawPixel) (void* deviceContext, uint16_t x, uint16_t y),
	FMGL_API_ColorStruct (*getPixel) (void* deviceContext, uint16_t x, uint16_t y),
	void (*pushFramebuffer) (void* deviceContext),
	FMGL_API_ColorStruct blankingColor,
	void (*clearFramebuffer) (void* deviceContext, FMGL_API_ColorStruct blankingColor)
)
{
	FMGL_API_DriverContext context;

	context.DeviceContext = deviceContext;
	context.GetWidth = getWidth;
	context.GetHeight = getHeight;
	context.SetActiveColor = setActiveColor;
	context.DrawPixel = drawPixel;
	context.GetPixel = getPixel;
	context.PushFramebuffer = pushFramebuffer;
	context.ClearFramebuffer = clearFramebuffer;

	/* Maximal coordinates */
	context.MaxX = context.GetWidth() - 1;
	context.MaxY = context.GetHeight() - 1;

	/* Blanking color */
	context.BlankingColor = blankingColor;

	return context;
}

void FMGL_API_SetActiveColor(FMGL_API_DriverContext* context, FMGL_API_ColorStruct color)
{
	context->ActiveColor = color;
	context->SetActiveColor(context->DeviceContext, context->ActiveColor);
}

void FMGL_API_SetBlankingColor(FMGL_API_DriverContext* context, FMGL_API_ColorStruct color)
{
	context->BlankingColor = color;
}

FMGL_API_ColorStruct FMGL_API_GetActiveColor(FMGL_API_DriverContext* context)
{
	return context->ActiveColor;
}

void FMGL_API_DrawPixel (FMGL_API_DriverContext* context, uint16_t x, uint16_t y)
{
	context->DrawPixel(context->DeviceContext, x, y);
}

FMGL_API_ColorStruct FMGL_API_GetPixel(FMGL_API_DriverContext* context, uint16_t x, uint16_t y)
{
	FMGL_API_ColorStruct result;
	result.R = 0;
	result.G = 0;
	result.B = 0;

	return context->GetPixel(context->DeviceContext, x, y);
}

void FMGL_API_PushFramebuffer (FMGL_API_DriverContext* context)
{
	context->PushFramebuffer(context->DeviceContext);
}

uint16_t FMGL_API_GetDisplayWidth(FMGL_API_DriverContext* context)
{
	return context->GetWidth();
}

uint16_t FMGL_API_GetDisplayHeight(FMGL_API_DriverContext* context)
{
	return context->GetHeight();
}

void FMGL_API_RenderXBM(FMGL_API_DriverContext* context, FMGL_API_XBMImage* image, uint16_t x, uint16_t y, uint16_t scaleX, uint16_t scaleY,
		FMGL_API_ColorStruct activeColor, FMGL_API_ColorStruct inactiveColor, FMGL_API_XBMTransparencyMode transparency)
{
	/* Do at least one pixel fit the screen? */
	if (x > context->MaxX || y > context->MaxY)
	{
		return;
	}

	uint32_t scaledWidth = image->Width * scaleX;
	uint32_t scaledHeight = image->Height * scaleY;

	FMGL_API_ColorStruct colors[FMGL_PRIV_XBM_COLORS_NUMBER]; /* Colors, active color first */
	colors[FMGL_PRIV_XBM_ACTIVE_COLOR_INDEX] = activeColor;
	colors[FMGL_PRIV_XBM_INACTIVE_COLOR_INDEX] = inactiveColor;

	for (uint8_t colorIndex = 0; colorIndex < FMGL_PRIV_XBM_COLORS_NUMBER; colorIndex++)
	{
		if
		(
			((FMGL_PRIV_XBM_ACTIVE_COLOR_INDEX == colorIndex) && (FMGL_XBMTransparencyModeTransparentActive == transparency))
			||
			((FMGL_PRIV_XBM_INACTIVE_COLOR_INDEX == colorIndex) && (FMGL_XBMTransparencyModeTransparentInactive == transparency))
		)
		{
			/* Skipping transparent color */
			continue;
		}

		FMGL_API_SetActiveColor(context, colors[colorIndex]);

		for (uint32_t dy = 0; dy < scaledHeight; dy++)
		{
			uint16_t sy = dy / scaleY;

			for (uint32_t dx = 0; dx < scaledWidth; dx++)
			{
				uint16_t sx = dx / scaleX;

				bool isActive = FMGL_Priv_IsActiveXBMPixel(image, sx, sy);

				if
				(
					((FMGL_PRIV_XBM_ACTIVE_COLOR_INDEX == colorIndex) && isActive)
					||
					((FMGL_PRIV_XBM_INACTIVE_COLOR_INDEX == colorIndex) && !isActive)
				)
				{
					/* Active pixel */
					FMGL_API_DrawPixel(context, dx + x, dy + y);
				}
			}
		}
	}
}

void FMGL_API_DrawLineHorizontal(FMGL_API_DriverContext* context, uint16_t x1, uint16_t x2, uint16_t y)
{
	if (y > context->MaxY)
	{
		return;
	}

	uint16_t minX = MIN(x1, x2);
	if (minX > context->MaxX)
	{
		/* Out of screen, can't draw */
		return;
	}

	uint16_t maxX = MAX(x1, x2);

	if (maxX > context->MaxX)
	{
		maxX = context->MaxX;
	}

	for (uint16_t x = minX; x <= maxX; x++)
	{
		FMGL_API_DrawPixel(context, x, y);
	}
}

void FMGL_API_DrawLineVertical(FMGL_API_DriverContext* context, uint16_t x, uint16_t y1, uint16_t y2)
{
	if (x > context->MaxX)
	{
		return;
	}

	uint16_t minY = MIN(y1, y2);

	if (minY > context->MaxY)
	{
		return;
	}

	uint16_t maxY = MAX(y1, y2);

	if (maxY > context->MaxY)
	{
		maxY = context->MaxY;
	}

	for (uint16_t y = minY; y <= maxY; y++)
	{
		FMGL_API_DrawPixel(context, x, y);
	}
}

/**
 * Draws rectangle without filling it.
 */
void FMGL_API_DrawRectangle(FMGL_API_DriverContext* context, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	/* Will at least one pixel be on screen */
	uint16_t minX = MIN(x1, x2);

	if (minX > context->MaxX)
	{
		return;
	}

	uint16_t minY = MIN(y1, y2);

	if (minY > context->MaxY)
	{
		return;
	}

	/* At least some pixels will be displayed */
	uint16_t maxX = MAX(x1, x2);
	uint16_t maxY = MAX(y1, y2);

	uint16_t maxVisibleX = MIN(maxX, context->MaxX);
	uint16_t maxVisibleY = MIN(maxY, context->MaxY);

	/* Drawing 100% visible parts */
	FMGL_API_DrawLineHorizontal(context, minX, maxVisibleX, minY);
	FMGL_API_DrawLineVertical(context, minX, minY, maxVisibleY);

	/* Right border */
	if (maxX <= context->MaxX)
	{
		FMGL_API_DrawLineVertical(context, maxVisibleX, minY, maxVisibleY);
	}

	/* Bottom border */
	if (maxY <= context->MaxY)
	{
		FMGL_API_DrawLineHorizontal(context, minX, maxVisibleX, maxVisibleY);
	}
}

void FMGL_API_DrawRectangleFilled(FMGL_API_DriverContext* context,
		uint16_t x1,
		uint16_t y1,
		uint16_t x2,
		uint16_t y2,
		FMGL_API_ColorStruct borderColor,
		FMGL_API_ColorStruct fillColor)
{
	FMGL_API_ColorStruct activeColor = FMGL_API_GetActiveColor(context);

	/* Will at least one pixel be on screen */
	uint16_t minX = MIN(x1, x2);

	if (minX > context->MaxX)
	{
		return;
	}

	uint16_t minY = MIN(y1, y2);

	if (minY > context->MaxY)
	{
		return;
	}

	/* At least some pixels will be displayed */
	uint16_t maxX = MAX(x1, x2);
	uint16_t maxY = MAX(y1, y2);

	uint16_t maxVisibleX = MIN(maxX, context->MaxX);
	uint16_t maxVisibleY = MIN(maxY, context->MaxY);

	uint16_t fillLeftX = minX + 1;
	uint16_t fillTopY = minY + 1;

	uint16_t fillRightX = maxVisibleX; /* Initially think that right and bottom borders are off-screen*/
	uint16_t fillBottomY = maxVisibleY;

	/* Borders */
	FMGL_API_SetActiveColor(context, borderColor);
	FMGL_API_DrawLineHorizontal(context, minX, maxVisibleX, minY);
	FMGL_API_DrawLineVertical(context, minX, minY, maxVisibleY);

	if (maxX <= context->MaxX)
	{
		/*Right border is on screen so moving fill right bottom 1 pixel left*/
		fillRightX -= 1;
		FMGL_API_DrawLineVertical(context, maxVisibleX, minY, maxVisibleY);
	}

	if (maxY <= context->MaxY)
	{
		fillBottomY -= 1;
		FMGL_API_DrawLineHorizontal(context, minX, maxVisibleX, maxVisibleY);
	}

	/* Fill */
	if ((fillLeftX > context->MaxX) || (fillTopY > context->MaxY) || (fillRightX < fillLeftX) || (fillBottomY < fillTopY))
	{
		FMGL_API_SetActiveColor(context, activeColor);
		return;
	}

	FMGL_API_SetActiveColor(context, fillColor);
	for (uint16_t y = fillTopY; y <= fillBottomY; y++)
	{
		FMGL_API_DrawLineHorizontal(context, fillLeftX, fillRightX, y);
	}

	FMGL_API_SetActiveColor(context, activeColor);
}

void FMGL_API_FillScreen(FMGL_API_DriverContext* context, FMGL_API_ColorStruct color)
{
	uint16_t maxX = FMGL_API_GetDisplayWidth(context) - 1;
	uint16_t maxY = FMGL_API_GetDisplayHeight(context) - 1;

	FMGL_API_DrawRectangleFilled(context, 0, 0, maxX, maxY, color, color);
}

void FMGL_API_ClearScreen(FMGL_API_DriverContext* context)
{
	if (NULL != context->ClearFramebuffer)
	{
		context->ClearFramebuffer(context->DeviceContext, context->BlankingColor);
	}
	else
	{
		FMGL_API_FillScreen(context, context->BlankingColor);
	}

}

void FMGL_API_RenderOneLineDumb(FMGL_API_DriverContext* context, FMGL_API_FontSettings* fontSettings, uint16_t x, uint16_t y, uint16_t* width,
		bool isDryRun, char* string)
{
	uint16_t scaledCharactersSpacing = fontSettings->CharactersSpacing * fontSettings->Scale;
	uint16_t currentX = x;

	char* currentChar = string;

	while (true)
	{
		if ('\0' == *currentChar)
		{
			/* End of line */
			return;
		}
		else if ('\n' == *currentChar)
		{
			/* Newline is not allowed */
			L2HAL_Error(WrongArgument);
		}

		/* Could we draw current pixel? */
		if (currentX > context->MaxX)
		{
			/* No */
			return;
		}

		/* Drawing */
		if (!isDryRun)
		{
			FMGL_Priv_RenderCharacter(context, fontSettings, currentX, y, *currentChar);
		}

		currentX += fontSettings->Font->GetCharacterWidth(*currentChar) * fontSettings->Scale;
		*width = currentX - x - 1;

		/* Adding intercharacter spacing */
		currentX += scaledCharactersSpacing;

		currentChar ++;
	}
}

void FMGL_API_RenderTextWithLineBreaks(FMGL_API_DriverContext* context, FMGL_API_FontSettings* fontSettings, uint16_t x, uint16_t y, uint16_t* width, uint16_t* height,
		bool isDryRun, char* string)
{
	*width = 0;
	*height = y;

	size_t length = strlen(string);

	uint16_t scaledLineHeight = (fontSettings->Font->Height + fontSettings->LinesSpacing) * fontSettings->Scale;

	/* Scanning string for newlines */
	uint16_t startPos = 0;
	for (uint16_t pos = 0; pos < length; pos ++)
	{
		if ('\n' == string[pos])
		{
			FMGL_Priv_RenderSubstring(context, fontSettings, startPos, pos - startPos, x, &y, scaledLineHeight, width, isDryRun, string);
			startPos = pos + 1; /* +1 To move after newline */
		}
	}

	FMGL_Priv_RenderSubstring(context, fontSettings, startPos, length - startPos, x, &y, scaledLineHeight, width, isDryRun, string);

	*height = y - *height;
}

void FMGL_API_DrawCircle(FMGL_API_DriverContext* context, uint16_t centerX, uint16_t centerY, uint16_t radius)
{
	int32_t x0 = centerX;
	int32_t y0 = centerY;

	int32_t x = 0;
	int32_t y = radius;

	int32_t delta = 1 - 2 * radius;
	int32_t error = 0;

	while(y >= 0)
	{
		FMGL_API_DrawPixel(context, (uint16_t)(x0 + x), (uint16_t)(y0 + y));
		FMGL_API_DrawPixel(context, (uint16_t)(x0 + x), (uint16_t)(y0 - y));
		FMGL_API_DrawPixel(context, (uint16_t)(x0 - x), (uint16_t)(y0 + y));
		FMGL_API_DrawPixel(context, (uint16_t)(x0 - x), (uint16_t)(y0 - y));

		error = 2 * (delta + y) - 1;
		if(delta < 0 && error <= 0)
		{
			++x;
			delta += 2 * x + 1;
			continue;
		}

		error = 2 * (delta - x) - 1;
		if(delta > 0 && error > 0)
		{
			--y;
			delta += 1 - 2 * y;
			continue;
		}

		++x;
		delta += 2 * (x - y);
		--y;
	}
}
