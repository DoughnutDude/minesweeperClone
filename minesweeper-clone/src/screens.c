#include "raylib.h"
#include "screens.h"

void DrawButton(Button button, int textOffsetX, int textOffsetY)
{
	DrawRectangleRec(button.rect, button.rectColor);
	Vector2 pos = { button.rect.x + textOffsetX, button.rect.y + textOffsetY };
	DrawTextEx(font, button.text, pos, font.baseSize, 2, button.textColor);
}