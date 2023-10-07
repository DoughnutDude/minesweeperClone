#include <raylib.h>
#include "screens.h"

int boardHeight = 16;
int boardWidth = 30;
int mineDensity = 20;
int minesDesired = 99;
bool mineGenMode = 1; // 0 = by density, 1 = til mineCount

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
	DrawTextEx(font, button.text, pos, font.baseSize, font.glyphPadding, button.textColor);
}

void DrawTextBox(TextBox textBox, int textOffsetX, int textOffsetY)
{
    if (CheckCollisionPointRec(GetMousePosition(), textBox.button.rect))
    { // Dim on mouse-over
        textBox.button.rectColor.r -= 10;
        textBox.button.rectColor.g -= 10;
        textBox.button.rectColor.b -= 10;
    }
    DrawRectangleRec(textBox.button.rect, textBox.button.rectColor);
    Vector2 pos = { textBox.button.rect.x + textOffsetX, textBox.button.rect.y + textOffsetY };
    DrawTextEx(font, textBox.button.text, pos, font.baseSize, font.glyphPadding, textBox.button.textColor);
    Vector2 textWidth = MeasureTextEx(font, textBox.button.text, font.baseSize, font.glyphPadding);
    pos.x += textWidth.x + font.glyphPadding;
    DrawTextEx(font, textBox.value, pos, font.baseSize, font.glyphPadding, textBox.button.textColor);
}