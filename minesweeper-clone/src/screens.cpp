#include <raylib.h>
#include "screens.h"

void DrawButton(Button button, int textOffsetX, int textOffsetY)
{
	if (CheckCollisionPointRec(GetMousePosition(), button.rect))
	{ // Dim on mouse-over
		button.rectColor.r -= 10;
		button.rectColor.g -= 10;
		button.rectColor.b -= 10;
	}
	DrawRectangleRec(button.rect, button.rectColor);
	Vector2 pos = { button.rect.x + textOffsetX, button.rect.y + textOffsetY };
	DrawTextEx(font, button.text, pos, font.baseSize, 2, button.textColor);
}