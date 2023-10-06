/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Options Screen Functions Definitions (Init, Update, Draw, Unload)
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

GameScreen previousScreen;

#define buttonCount 3
union {
    struct {
        Button resumeButton;
        Button mainMenuButton;
        Button quitButton;
    };
    Button buttons[buttonCount];
} menu;

//----------------------------------------------------------------------------------
// Options Screen Functions Definition
//----------------------------------------------------------------------------------

// Options Screen Initialization logic
void InitOptionsScreen(void)
{
    framesCounter = 0;
    finishResult = 0;

    previousScreen = currentScreen;


    for (int i = 0; i < ARRAYCOUNT(menu.buttons); ++i)
    {
        menu.buttons[i].rect = { GetScreenWidth() / 3.0f, GetScreenHeight() / 3.0f + i*80, 400, 60 };
        menu.buttons[i].rectColor = BROWN;
        menu.buttons[i].textColor = BEIGE;
    }
    menu.resumeButton.text = (previousScreen == GAMEPLAY) ? "Resume": "Go Back";
    menu.mainMenuButton.text = "Exit to Title Screen";
    menu.quitButton.text = "Quit";
}

// Options Screen Update logic
void UpdateOptionsScreen(void)
{
    bool clickL = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    Vector2 mousePos = GetMousePosition();
    if (clickL)
    {
        if (CheckCollisionPointRec(mousePos, menu.mainMenuButton.rect))
        {
            finishResult = (int)TITLE;
        }
        else if (CheckCollisionPointRec(mousePos, menu.resumeButton.rect))
        {
            finishResult = (int)previousScreen;
            PlaySound(fxCoin);
        } else if (CheckCollisionPointRec(mousePos, menu.quitButton.rect))
        {
            finishResult = -1;
            PlaySound(fxCoin);
            running = false;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        finishResult = (int)previousScreen;
        PlaySound(fxCoin);
    }
}

// Options Screen Draw logic
void DrawOptionsScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), DARKGRAY); //draw backdrop
    Vector2 pos = { 20, 10 };
    DrawTextEx(font, "OPTIONS WILL GO HERE:", pos, font.baseSize, 4, LIGHTGRAY);
    for (int i = 0; i < buttonCount; ++i)
    {
        DrawButton(menu.buttons[i], 10, 10);
    }
}

// Options Screen Unload logic
void UnloadOptionsScreen(void)
{
}

// Options Screen should finish?
int FinishOptionsScreen(void)
{
    return finishResult;
}