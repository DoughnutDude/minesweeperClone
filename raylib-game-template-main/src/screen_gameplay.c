/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
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

#include "raylib.h"
#include "screens.h"
#include "gameplay.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static float dt = 0.0f;

static int hp = 1;

Vector2 screenCenter = { 0 };
Camera2D camera = { 0 };
Rectangle player = { 0 };

#define boardHeight 16 // in number of tiles
#define boardWidth 30
int board[boardHeight][boardWidth]; // negative = mine, non-negative = number of touching mines
int boardMask[boardHeight][boardWidth]; // 0 = hidden, 1 = revealed
Rectangle boardRect = { 0 };
float tileSize = 40.0f;
Vector2 screenOffset = { 0 };
int fontSize = 0;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    dt = GetFrameTime();

    hp = 1;

    screenCenter.x = (float)GetScreenWidth() / 2;
    screenCenter.y = (float)GetScreenHeight() / 2;

    camera.target = screenCenter;
    camera.offset = screenCenter;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // init board
    //Vector2 randomStartPos = {GetRandomValue(0, boardWidth), GetRandomValue(0, boardHeight)};
    for (int y = 0; y < boardHeight; ++y)
    {
        for (int x = 0; x < boardWidth; ++x)
        {
            board[y][x] = 0;
            boardMask[y][x] = 0;
        }
    }
    for (int y = 0; y < boardHeight; ++y)
    {
        for (int x = 0; x < boardWidth; ++x)
        {
            int randVal = GetRandomValue(0,5);
            if (!randVal) {
                if ((x < boardWidth - 1) && (board[y][x + 1] >= 0))
                {
                    ++(board[y][x + 1]);
                    if ((y > 0) && (board[y - 1][x + 1] >= 0))
                    {
                        ++(board[y - 1][x + 1]);
                    }
                }
                if ((x > 0) && (board[y][x - 1] >= 0))
                {
                    ++(board[y][x - 1]);
                    if ((y < boardHeight - 1) && (board[y + 1][x - 1] >= 0))
                    {
                        ++(board[y + 1][x - 1]);
                    }
                }
                if ((y < boardHeight - 1) && (board[y+1][x] >= 0))
                {
                    ++(board[y + 1][x]);
                    if ((x < boardWidth - 1) && (board[y + 1][x + 1] >= 0))
                    {
                        ++(board[y+1][x + 1]);
                    }
                }
                if ((y > 0) && (board[y-1][x] >= 0))
                {
                    ++(board[y - 1][x]);
                    if ((x > 0) && (board[y-1][x - 1] >= 0))
                    {
                        ++(board[y-1][x - 1]);
                    }
                }
                board[y][x] = -1;
            }
        }
    }
    screenOffset = (Vector2){ screenCenter.x - (tileSize*boardWidth + boardWidth-1) / 2.0f,
                              screenCenter.y - (tileSize*boardHeight + boardHeight-1) / 2.0f };
    boardRect = (Rectangle){ screenOffset.x, screenOffset.y,
                             (tileSize*boardWidth) + boardWidth-1,
                             (tileSize*boardHeight) + boardHeight-1 };
    fontSize = 0.8f*(float)tileSize;
}

void OperateOnAdjacentTiles()
{

}

void FloodFillClearTilesRecursive()
{
    if (true)
    {

    }
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    ++framesCounter;
    dt = GetFrameTime();

    screenCenter.x = (float)GetScreenWidth() / 2;
    screenCenter.y = (float)GetScreenHeight() / 2;

    if (((framesCounter) % 7) == 0) {
        //--screenCenter.y; //this is where we'd do the scrolling stuff
    }
    screenOffset = (Vector2){ screenCenter.x - (tileSize*boardWidth + boardWidth-1) / 2.0f,
                              screenCenter.y - (tileSize*boardHeight + boardHeight-1) / 2.0f };

    // Mouse capture
    if (hp > 0)
    {
        Vector2 mousePos = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (CheckCollisionPointRec(mousePos, boardRect))
            {
                for (int y = 0; y < boardHeight; ++y)
                {
                    for (int x = 0; x < boardWidth; ++x)
                    {
                        Rectangle boardTileInWorld = { x * tileSize + x + screenOffset.x,
                                                       y * tileSize + y + screenOffset.y,
                                                       tileSize, tileSize };
                        if (CheckCollisionPointRec(mousePos, boardTileInWorld)) {
                            switch (board[y][x])
                            {
                            case -1:
                                --hp;
                            case 0:
                                //flood fill: TODO: move into recursive func body
                                if ((x < boardWidth - 1) && (board[y][x + 1] >= 0))
                                {
                                    ++(board[y][x + 1]);
                                    if ((y > 0) && (board[y - 1][x + 1] >= 0))
                                    {
                                        ++(board[y - 1][x + 1]);
                                    }
                                }
                                if ((x > 0) && (board[y][x - 1] >= 0))
                                {
                                    ++(board[y][x - 1]);
                                    if ((y < boardHeight - 1) && (board[y + 1][x - 1] >= 0))
                                    {
                                        ++(board[y + 1][x - 1]);
                                    }
                                }
                                if ((y < boardHeight - 1) && (board[y + 1][x] >= 0))
                                {
                                    ++(board[y + 1][x]);
                                    if ((x < boardWidth - 1) && (board[y + 1][x + 1] >= 0))
                                    {
                                        ++(board[y + 1][x + 1]);
                                    }
                                }
                                if ((y > 0) && (board[y - 1][x] >= 0))
                                {
                                    ++(board[y - 1][x]);
                                    if ((x > 0) && (board[y - 1][x - 1] >= 0))
                                    {
                                        ++(board[y - 1][x - 1]);
                                    }
                                }
                            default:
                                boardMask[y][x] = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ESCAPE))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), DARKGRAY); //draw backdrop

    BeginMode2D(camera); // Everything within the 2D mode gets affected by camera movement/transformations
    //----------------------------------------------------------------------------------
    // Draw minesweeper board
    DrawRectangleLinesEx(boardRect, 2.0f, SKYBLUE); // Board outline/border
    for (int y = 0; y < boardHeight; ++y)
    {
        for (int x = 0; x < boardWidth; ++x)
        {
            Rectangle boardTile = { x * tileSize + x + screenOffset.x,
                                    y * tileSize + y + screenOffset.y,
                                    tileSize, tileSize };
            Color tileColor = GRAY;
            Color touchNumColor = BLACK;
            char * tileTextSymbol[64];
            if (!boardMask[y][x])
            {
                tileColor = WHITE;
                touchNumColor = (Color){ 0,0,0,0 };
            }
            else
            {
                sprintf(tileTextSymbol, "%d\n", board[y][x]);
                switch (board[y][x])
                {
                case -1:
                    touchNumColor = (Color){ 0,0,0,255 };
                    sprintf(tileTextSymbol, "X\n");
                    tileColor = DARKBROWN;
                    break;
                case 0:
                    touchNumColor = (Color){ 0,0,0,0 };
                    break;
                case 1:
                    touchNumColor = BLUE;
                    break;
                case 2:
                    touchNumColor = DARKGREEN;
                    break;
                case 3:
                    touchNumColor = RED;
                    break;
                case 4:
                    touchNumColor = DARKBLUE;
                    break;
                case 5:
                    touchNumColor = BROWN;
                    break;
                case 6:
                    touchNumColor = (Color){ 0, 100, 100, 255 }; //CYAN
                    break;
                case 7:
                    touchNumColor = BLACK;
                    break;
                case 8:
                    touchNumColor = LIGHTGRAY;
                    break;
                }
            }
            DrawRectangle(boardTile.x, boardTile.y,
                          tileSize, tileSize, tileColor);
            DrawTextEx(font, tileTextSymbol, (Vector2){boardTile.x + tileSize/3, boardTile.y + tileSize/10}, fontSize, 0, touchNumColor);
        }
    }
    //----------------------------------------------------------------------------------
    EndMode2D();

    if (hp <= 0) // Lose screen
    {
        DrawRectangle(screenCenter.x - 200, screenCenter.y - 50, 400, 100, MAROON);
        DrawTextEx(font, "GAME OVER", (Vector2) { screenCenter.x - 180, screenCenter.y - 40 },
                   font.baseSize*2, 4, BEIGE);
    }

    Vector2 pos = (Vector2) {5,10};
    Color uiBackdropColor = DARKPURPLE;
    uiBackdropColor.a = 100;
    DrawRectangle(0, 0, 40, 40, uiBackdropColor);
    DrawTextEx(font, "ESC", pos, font.baseSize/2, 4, BEIGE);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{

}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}