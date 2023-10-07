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


#define buttonCount 5
#define textBoxCount 3
union {
    struct {
        Button resumeButton;
        Button defaults;
        Button mineGenMode;
        Button mainMenuButton;
        Button quitButton;

        TextBox boardWidth;
        TextBox boardHeight;
        TextBox mineCap;
    };
    struct {
        Button buttons[buttonCount];
        TextBox textBoxes[textBoxCount];
    };
} menu;

int textBoxFocus = 0; // Signifies which text box element has focus. 0 = none
int digitCount = 0;
int digitCap = 2;
int digits[3] = {};

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
        menu.buttons[i] = {};
        menu.buttons[i].rect = { GetScreenWidth() / 5.0f, GetScreenHeight() / 5.0f + i*80, 400, 60 };
        menu.buttons[i].rectColor = BROWN;
        menu.buttons[i].textColor = BEIGE;
    }
    menu.resumeButton.text = (previousScreen == GAMEPLAY) ? "Resume": "Go Back";
    menu.defaults.text = "Default Settings";
    menu.mineGenMode.text = (mineGenMode == 0) ? "Mine Gen Mode:  %" : "Mine Gen Mode:  #";
    menu.mainMenuButton.text = "Exit to Title Screen";
    menu.quitButton.text = "Quit";

    for (int i = 0; i < ARRAYCOUNT(menu.textBoxes); ++i)
    {
        menu.textBoxes[i] = {};
        menu.textBoxes[i].button = {};
        menu.textBoxes[i].button.rect = { GetScreenWidth() / 5.0f + 480, GetScreenHeight() / 5.0f + i * 80, 400, 60 };
        menu.textBoxes[i].button.rectColor = BROWN;
        menu.textBoxes[i].button.textColor = BEIGE;
        //menu.textBoxes[i].button.textColor -= {10,10,10,0};
    }
    menu.boardWidth.button.text = "Board Width: ";
    menu.boardWidth.value[0] = (char)(boardWidth / 10 + 48);
    menu.boardWidth.value[1] = (char)(boardWidth % 10 + 48);
    menu.boardHeight.button.text = "Board Height: ";
    menu.boardHeight.value[0] = (char)(boardHeight/ 10 + 48);
    menu.boardHeight.value[1] = (char)(boardHeight% 10 + 48);

    menu.mineCap.button.text = (mineGenMode == 0) ? "Mine Density: %" : "Number of Mines: ";
    menu.mineCap.value[0] = (mineGenMode == 0) ? (char)(mineDensity / 10 + 48) : (char)(minesDesired / 100 + 48);
    menu.mineCap.value[1] = (mineGenMode == 0) ? (char)(mineDensity % 10 + 48) : (char)(minesDesired % 100 / 10 + 48);
    menu.mineCap.value[2] = (mineGenMode == 0) ? 0                             : (char)(minesDesired % 10 + 48);
}

// Options Screen Update logic
void UpdateOptionsScreen(void)
{
    menu.mineGenMode.text = (mineGenMode == 0) ? "Mine Gen Mode:  %" : "Mine Gen Mode:  #";
    menu.mineCap.button.text = (mineGenMode == 0) ? "Mine Density: %" : "Number of Mines: ";
    bool clickL = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    Vector2 mousePos = GetMousePosition();
    if (clickL)
    {
        if (CheckCollisionPointRec(GetMousePosition(), menu.boardWidth.button.rect))
        {
            textBoxFocus = 1;
            digits[0] = 0;
            digits[1] = 0;
            digitCount = 0;
            digitCap = 2;
        }
        else if (CheckCollisionPointRec(GetMousePosition(), menu.boardHeight.button.rect))
        {
            textBoxFocus = 2;
            digits[0] = 0;
            digits[1] = 0;
            digitCount = 0;
            digitCap = 2;
        }
        else if (CheckCollisionPointRec(GetMousePosition(), menu.mineCap.button.rect))
        {
            textBoxFocus = 3;
            digits[0] = 0;
            digits[1] = 0;
            digits[2] = 0;
            digitCount = 0;
            digitCap = (mineGenMode == 0) ? 2 : 3;
        }
        else
        {
            textBoxFocus = 0;
            if (CheckCollisionPointRec(mousePos, menu.resumeButton.rect))
            {
                finishResult = (int)previousScreen;
                PlaySound(fxCoin);
            }
            else if (CheckCollisionPointRec(mousePos, menu.defaults.rect))
            {
                mineGenMode = 0;
                boardWidth = 30;
                boardHeight = 16;
                mineDensity = 20;
                minesDesired = 99;
                PlaySound(fxCoin);
            }
            else if (CheckCollisionPointRec(mousePos, menu.mineGenMode.rect))
            {
                mineGenMode = !mineGenMode;
                PlaySound(fxCoin);
            }
            else if (CheckCollisionPointRec(mousePos, menu.mainMenuButton.rect))
            {
                finishResult = (int)TITLE;
            }
            else if (CheckCollisionPointRec(mousePos, menu.quitButton.rect))
            {
                finishResult = -1;
                PlaySound(fxCoin);
                running = false;
            }
        }
    }

    if (textBoxFocus)
    {
        int key = GetKeyPressed();
        while (key > 0)
        {
            if ((key >= 48) && (key <= 57) && (digitCount < digitCap))
            {
                digits[digitCount] = key-48;
                ++digitCount;

            }
            key = GetKeyPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            --digitCount;
            if (digitCount < 0)
            {
                digitCount = 0;
            }
            digits[digitCount] = '\0';
        }
        switch (textBoxFocus)
        {
        case 1:
            boardWidth = digits[0]*powInt(10,digitCount-1) + digits[1];
            break;
        case 2:
            boardHeight = digits[0]*powInt(10,digitCount-1) + digits[1];
            break;
        case 3:
            if (mineGenMode == 0)
            {
                mineDensity = digits[0] * powInt(10, digitCount - 1) + digits[1];
            }
            else if (mineGenMode == 1)
            {
                minesDesired = digits[0] * powInt(10, digitCount - 1) + digits[1] * powInt(10, digitCount - 2) + digits[2];
            }
            break;
        }
    }
    menu.boardWidth.value[0] = (char)(boardWidth / 10 + 48);
    menu.boardWidth.value[1] = (char)(boardWidth % 10 + 48);
    menu.boardHeight.value[0] = (char)(boardHeight / 10 + 48);
    menu.boardHeight.value[1] = (char)(boardHeight % 10 + 48);
    if (boardWidth < 3) boardWidth = 3;
    if (boardWidth > maxBoardWidth) boardWidth = maxBoardHeight;
    if (boardHeight < 3) boardHeight = 3;
    if (boardHeight > maxBoardHeight) boardHeight = maxBoardHeight;

    menu.mineCap.value[0] = (mineGenMode == 0) ? (char)(mineDensity / 10 + 48) : (char)(minesDesired / 100 + 48);
    menu.mineCap.value[1] = (mineGenMode == 0) ? (char)(mineDensity % 10 + 48) : (char)(minesDesired % 100 / 10 + 48);
    menu.mineCap.value[2] = (mineGenMode == 0) ? 0 : (char)(minesDesired % 10 + 48);
    if (mineGenMode && minesDesired > boardWidth * boardHeight) minesDesired = boardWidth * boardHeight - 1;

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
    DrawTextEx(font, "OPTIONS", pos, font.baseSize, font.glyphPadding, BEIGE);
    for (int i = 0; i < buttonCount; ++i)
    {
        DrawButton(menu.buttons[i], 10, 10);
    }
    for (int i = 0; i < textBoxCount; ++i)
    {
        DrawTextBox(menu.textBoxes[i], 10, 10);
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