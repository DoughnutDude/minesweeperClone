/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Title Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

/**********************************************************************************************
*   Modified from the original software for use in Minesweeper Clone
*   Copyright (c) 2023 (DoughnutDude)
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
global_var int framesCounter = 0;
global_var int finishResult = 0;

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitTitleScreen(void)
{
    // TODO: Initialize TITLE screen variables here!
    framesCounter = 0;
    finishResult = 0;
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
    // TODO: Update TITLE screen variables here!

    // Press enter or tap to change to GAMEPLAY screen
    bool clickL = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    bool tap = IsGestureDetected(GESTURE_TAP);
    Vector2 mousePos = GetMousePosition();
    if (clickL != tap != IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        finishResult = (int)GAMEPLAY;
        PlaySound(fxCoin);
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        finishResult = (int)OPTIONS;
        PlaySound(fxCoin);
    }
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
    // TODO: Draw TITLE screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), DARKGRAY);
    Vector2 pos = { 20, 10 };
    DrawTextEx(font, "Minesweeper Clone", pos, font.baseSize, font.glyphPadding, BEIGE);

    float textWidth = MeasureText("CLICK / TAP", font.baseSize);
    Vector2 test = MeasureTextEx(font, "CLICK / TAP", font.baseSize, font.glyphPadding);
    pos = { GetScreenWidth() / 2.0f - textWidth / 2.0f, GetScreenHeight() / 3.0f };
    Rectangle startScreenRect = { pos.x - 10, pos.y - 10, textWidth + 20, 50 };
    DrawRectangleRounded(startScreenRect, 0.5f, 2, BROWN);
    DrawTextEx(font, "CLICK / TAP", pos, font.baseSize, font.glyphPadding, BEIGE);
}

// Title Screen Unload logic
void UnloadTitleScreen(void)
{
    // TODO: Unload TITLE screen variables here!
}

// Title Screen should finish?
int FinishTitleScreen(void)
{
    return finishResult;
}