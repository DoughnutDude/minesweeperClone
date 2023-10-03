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

/**********************************************************************************************
*   Modified from the original software for use in Minesweeper Clone
*   Copyright (c) 2023 (DoughnutDude)
**********************************************************************************************/


#include "raylib.h"
#include "screens.h"
#include "gameplay.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
global_var int framesCounter = 0;
global_var int finishResult = 0;

global_var bool winCon;
global_var float dt = 0.0f;

global_var int hp = 1;

Vector2 screenCenter = { 0 };
Camera2D camera = { 0 };
Rectangle player = { 0 };

int mineCount = 0;
int maxMines = 0;
#define boardHeight 16 // in number of tiles
#define boardWidth 30
int board[boardHeight][boardWidth] = { 0 }; // -2 = mine(clicked on), -1 = mine, non-negative = number of adjacent mines
int boardMask[boardHeight][boardWidth] = { 0 }; // -1 = flagged, 0 = hidden, 1 = revealed
Rectangle boardRect = { 0 };
float tileSize = 40.0f;
Vector2 screenOffset = { 0 };
int fontSize = 0;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
void GenerateMinesRecursively(int x, int y)
{
    if (board[y][x] < 0 || mineCount >= maxMines)
    {
        return;
    }
    board[y][x] = -1;
    ++mineCount;
    for (int offsetY = -1; offsetY < 2; ++offsetY)
    {
        for (int offsetX = -1; offsetX < 2; ++offsetX)
        {
            if (!((offsetX == 0) && (offsetY == 0)))
            {
                int neighborX = x + offsetX;
                int neighborY = y + offsetY;
                if ((neighborX < boardWidth) && (neighborX >= 0) &&
                    (neighborY < boardHeight) && (neighborY >= 0) &&
                    board[neighborY][neighborX] >= 0)
                {
                    ++board[neighborY][neighborX];
                }
            }
        }
    }
    int offsetX = GetRandomValue(-1, 2);
    int offsetY = GetRandomValue(-1, 2);
    while (((offsetX == 0) && (offsetY == 0)))
    {
        offsetX = GetRandomValue(-1, 2);
        offsetY = GetRandomValue(-1, 2);
    }
    int neighborX = x + offsetX;
    int neighborY = y + offsetY;
    if ((neighborX < boardWidth) && (neighborX >= 0) &&
        (neighborY < boardHeight) && (neighborY >= 0))
    {
        GenerateMinesRecursively(neighborX, neighborY);
    }
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishResult = 0;

    winCon = false;
    dt = GetFrameTime();

    hp = 3;

    screenCenter.x = (float)GetScreenWidth() / 2;
    screenCenter.y = (float)GetScreenHeight() / 2;

    camera.target = screenCenter;
    camera.offset = screenCenter;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // init board
    // TODO: move into init board function?
    screenOffset = (Vector2){ screenCenter.x - (tileSize * boardWidth + boardWidth - 1) / 2.0f,
                              screenCenter.y - (tileSize * boardHeight + boardHeight - 1) / 2.0f };
    boardRect = (Rectangle){ screenOffset.x, screenOffset.y,
                             (tileSize * boardWidth) + boardWidth - 1,
                             (tileSize * boardHeight) + boardHeight - 1 };
    fontSize = 0.8f * (float)tileSize;
    mineCount = 0;
    maxMines = 0.21f * (boardWidth * boardHeight);

#if 0 // Enable to start with board revealed
    for (int y = 0; y < boardHeight; ++y)
    {
        for (int x = 0; x < boardWidth; ++x)
        {
            boardMask[y][x] = 1;
        }
    }
#endif
#if 0
    int x = GetRandomValue(0, boardWidth-1);// windows had it such that this start pos is closest empty spot to first clicked tile.
    int y = GetRandomValue(0, boardHeight-1);
    GenerateMinesRecursively(x, y);
    while (mineCount < maxMines)
    {
        x = GetRandomValue(0, boardWidth - 1);
        y = GetRandomValue(0, boardHeight - 1);
        if (board[y][x] >= 0)
        {
            board[y][x] = -1;
            ++mineCount;
            for (int offsetY = -1; offsetY < 2; ++offsetY)
            {
                for (int offsetX = -1; offsetX < 2; ++offsetX)
                {
                    if (!((offsetX == 0) && (offsetY == 0)))
                    {
                        int neighborX = x + offsetX;
                        int neighborY = y + offsetY;
                        if ((neighborX < boardWidth) && (neighborX >= 0) &&
                            (neighborY < boardHeight) && (neighborY >= 0) &&
                            board[neighborY][neighborX] >= 0)
                        {
                            ++board[neighborY][neighborX];
                        }
                    }
                }
            }
        }
    }
#else
    for (int y = 0; y < boardHeight; ++y)
    {
        for (int x = 0; x < boardWidth; ++x)
        {
            //TODO: This can get the correct mine density(20%),
            // but needs to be modified so that first click is gauranteed safe, 
            // and also parameterized for the possibility of no-guess mode.
            int randVal = GetRandomValue(0,5);
            if (!randVal) {
                for (int offsetY = -1; offsetY < 2; ++offsetY)
                {
                    for (int offsetX = -1; offsetX < 2; ++offsetX)
                    {
                        if (!((offsetX == 0) && (offsetY == 0)))
                        {
                            int neighborX = x + offsetX;
                            int neighborY = y + offsetY;
                            if ((neighborX < boardWidth) && (neighborX >= 0) &&
                                (neighborY < boardHeight) && (neighborY >= 0) &&
                                board[neighborY][neighborX] >= 0)
                            {
                                ++board[neighborY][neighborX];
                            }
                        }
                    }
                }
                board[y][x] = -1;
            }
        }
    }
#endif
}

Rectangle MakeRectFromTile(int x, int y)
{
    Rectangle result = { x * tileSize + x + screenOffset.x,
                                   y * tileSize + y + screenOffset.y,
                                   tileSize, tileSize };
    return result;
}

void FloodFillClearTilesRecursively(int x, int y)
{
    if (((x < boardWidth) && (x >= 0) && (y < boardHeight) && (y >= 0)) && (board[y][x] >= 0) && (boardMask[y][x] == 0))
    {
        boardMask[y][x] = 1;

        if (board[y][x] > 0) return;

#if 1
        for (int offsetY = -1; offsetY < 2; ++offsetY)
        {
            for (int offsetX = -1; offsetX < 2; ++offsetX)
            {
                if (!((offsetX == 0) && (offsetY == 0)))
                {
                    int neighborX = x + offsetX;
                    int neighborY = y + offsetY;
                    if ((neighborX < boardWidth) && (neighborX >= 0) &&
                        (neighborY < boardHeight) && (neighborY >= 0) &&
                        (boardMask[neighborY][neighborX] == 0 || (board[neighborY][neighborX] >= 0)))
                    {
                        FloodFillClearTilesRecursively(neighborX, neighborY);
                    }
                }
            }
        }
#else //just the same thing, but with if-else's instead of loops
        if (x < boardWidth)
        {
            FloodFillClearTilesRecursively(x + 1, y);//east
            if (y >= 0)
            {
                FloodFillClearTilesRecursively(x + 1, y - 1);//north-east
            }
            else return;
        } else return;
        if (x >= 0)
        {
            FloodFillClearTilesRecursively(x - 1, y);//west
            if (y < boardHeight)
            {
                FloodFillClearTilesRecursively(x - 1, y + 1);//south-west
            } else return;
        } else return;
        if (y < boardHeight)
        {
            FloodFillClearTilesRecursively(x, y + 1);//south
            if (x < boardWidth)
            {
                FloodFillClearTilesRecursively(x + 1, y + 1);//south-east
            } else return;
        } else return;
        if (y >= 0)
        {
            FloodFillClearTilesRecursively(x, y - 1);//north
            if (x >= 0)
            {
                FloodFillClearTilesRecursively(x -1, y - 1);//north-west
            } else return;
        } else return;
#endif
    }
}

void AttemptTileReveal(int x, int y)
{
    if (boardMask[y][x] == 0)
    {
        switch (board[y][x])
        {
        case -1:
            --hp;
            board[y][x] = -2; // The mine has now been clicked/stepped on.
        case 0:
            // If a tile is touching 0 mines, then it clears tiles until mines are detected.
            FloodFillClearTilesRecursively(x, y);
        default:
            boardMask[y][x] = 1;
        }
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
    boardRect = (Rectangle){ screenOffset.x, screenOffset.y,
                             (tileSize * boardWidth) + boardWidth - 1,
                             (tileSize * boardHeight) + boardHeight - 1 };

    // Mouse capture
    if (hp > 0)
    {
        bool clickL = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        bool clickR = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
        bool clickM = IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE);
        Vector2 mousePos = GetMousePosition();
        if (clickL != clickR != clickM)
        {
            if (CheckCollisionPointRec(mousePos, boardRect))
            {
                for (int y = 0; y < boardHeight; ++y)
                {
                    for (int x = 0; x < boardWidth; ++x)
                    {

                        Rectangle boardTileInWorld = MakeRectFromTile(x, y);
                        if (CheckCollisionPointRec(mousePos, boardTileInWorld))
                        {
                            if (clickL)
                            {
                                if (boardMask[y][x] == 0)
                                {
                                    AttemptTileReveal(x, y);
                                }
                            }
                            else if (clickR)
                            {
                                if (boardMask[y][x] == 0)
                                {
                                    boardMask[y][x] = -1;
                                }
                                else if (boardMask[y][x] == -1)
                                {
                                    boardMask[y][x] = 0;
                                }
                            }
                            else if (clickM)
                            {
                                // [Chording]:

                                // First make sure that the number of adjacent flagged/mine-having tiles
                                //is the same as the number in the clicked tile's value.
                                int numOfAdjacentFlags = 0;
                                for (int offsetY = -1; offsetY < 2; ++offsetY)
                                {
                                    for (int offsetX = -1; offsetX < 2; ++offsetX)
                                    {
                                        if (!((offsetX == 0) && (offsetY == 0)))
                                        {
                                            int neighborX = x + offsetX;
                                            int neighborY = y + offsetY;
                                            if ((neighborX < boardWidth) && (neighborX >= 0) &&
                                                (neighborY < boardHeight) && (neighborY >= 0) &&
                                                (boardMask[neighborY][neighborX] == -1 || (board[neighborY][neighborX] == -2)))
                                            {
                                                ++numOfAdjacentFlags;
                                            }
                                        }
                                    }
                                }
                                // Then reveal all adjacent tiles
                                if (numOfAdjacentFlags == board[y][x])
                                {
#if 1
                                    for (int offsetY = -1; offsetY < 2; ++offsetY)
                                    {
                                        for (int offsetX = -1; offsetX < 2; ++offsetX)
                                        {
                                            if (!((offsetX == 0) && (offsetY == 0)))
                                            {
                                                int neighborX = x + offsetX;
                                                int neighborY = y + offsetY;
                                                if ((neighborX < boardWidth) && (neighborX >= 0) &&
                                                    (neighborY < boardHeight) && (neighborY >= 0))
                                                {
                                                    AttemptTileReveal(neighborX, neighborY);
                                                }
                                            }
                                        }
                                    }
#else
                                    if (x < boardWidth - 1)
                                    {
                                        AttemptTileReveal(x + 1, y);
                                        if (y > 0)
                                        {
                                            AttemptTileReveal(x + 1, y - 1);
                                        }
                                    }
                                    if (x > 0)
                                    {
                                        AttemptTileReveal(x - 1, y);
                                        if (y < boardHeight - 1)
                                        {
                                            AttemptTileReveal(x - 1, y + 1);
                                        }
                                    }
                                    if (y < boardHeight - 1)
                                    {
                                        AttemptTileReveal(x, y + 1);
                                        if (x < boardWidth - 1)
                                        {
                                            AttemptTileReveal(x + 1, y + 1);
                                        }
                                    }
                                    if (y > 0)
                                    {
                                        AttemptTileReveal(x, y - 1);
                                        if (x > 0)
                                        {
                                            AttemptTileReveal(x - 1, y - 1);
                                        }
                                    }
#endif
                                }
                            }
                        }
                    }
                }
            }
        }
        // Check for win condition after every mouse click.
        //if all non-mine spaces have been revealed, winCon = true.
        // Maybe broken? saw a weird bug once with premature win while not all tiles had yet been revealed.
        winCon = true;
        for (int y = 0; y < boardHeight; ++y)
        {
            for (int x = 0; x < boardWidth; ++x)
            {
                if (!(boardMask[y][x] == 1 || board[y][x] < 0))
                {
                    winCon = false;
                    break;
                }
            }
        }
        winCon = winCon && (hp > 0); // We also make sure that the player has health remaining.
    }

    // When game is done, regardless of win or lose, all mines should be revealed.
    if ((hp <= 0) || winCon)
    {
        for (int y = 0; y < boardHeight; ++y)
        {
            for (int x = 0; x < boardWidth; ++x)
            {
                if ((board[y][x] < 0) && (boardMask[y][x] != 2))
                {
                    boardMask[y][x] = 1;
                }
            }
        }
    }

    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ESCAPE))
    {
        finishResult = (int)OPTIONS;
        PlaySound(fxCoin);
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), DARKGRAY); // Draw backdrop

    //----------------------------------------------------------------------------------
    BeginMode2D(camera); // Everything within the 2D mode gets affected by camera movement/transformations
    
    // Draw minesweeper board
    DrawRectangleLinesEx(boardRect, 2.0f, SKYBLUE); // Board outline/border
    for (int y = 0; y < boardHeight; ++y)
    {
        for (int x = 0; x < boardWidth; ++x)
        {
            Rectangle boardTile = MakeRectFromTile(x,y);
            Color tileColor = { 120,120,120,255 };
            Color touchNumColor = BLACK;
            char * tileTextSymbol[64];

            if (boardMask[y][x] <= 0)
            {
                touchNumColor = (Color){ 0,0,0,0 };
                if (!(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), boardTile)))
                {
                    tileColor = (Color){ 185,185,185,255 };
                    if (boardMask[y][x] == -1)
                    {
                        sprintf(tileTextSymbol, "X\n");
                        touchNumColor = BROWN; // Draw flag
                    }
                }
                if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
                {
                    for (int offsetY = -1; offsetY < 2; ++offsetY)
                    {
                        for (int offsetX = -1; offsetX < 2; ++offsetX)
                        {
                            int neighborX = x + offsetX;
                            int neighborY = y + offsetY;
                            Rectangle neighborTile = MakeRectFromTile(neighborX, neighborY);
                            if ((neighborX < boardWidth) && (neighborX >= 0) &&
                                (neighborY < boardHeight) && (neighborY >= 0) &&
                                (boardMask[y][x] != -1) && CheckCollisionPointRec(GetMousePosition(), neighborTile))
                            {
                                touchNumColor = (Color){ 0,0,0,0 };
                                tileColor = (Color){ 120,120,120,255 };
                            }
                        }
                    }
                }
            }
            else
            {
                sprintf(tileTextSymbol, "%d\n", board[y][x]);
                switch (board[y][x])
                {
                case -2:
                    tileColor = DARKBROWN;
                case -1:
                    touchNumColor = (Color){ 0,0,0,255 };
                    sprintf(tileTextSymbol, "X\n");
                    break;
                case 0:
                    touchNumColor = (Color){ 0,0,0,0 };
                    break;
                case 1:
                    touchNumColor = (Color){ 0,0,255,255 };
                    break;
                case 2:
                    touchNumColor = (Color){ 00,100,00,255 };
                    break;
                case 3:
                    touchNumColor = (Color){ 255,00,00,255 };
                    break;
                case 4:
                    touchNumColor = (Color){ 00,00,84,255 };
                    break;
                case 5:
                    touchNumColor = (Color){ 84,00,00,255 };
                    break;
                case 6:
                    touchNumColor = (Color){ 00,82,84,255 }; //CYAN
                    break;
                case 7:
                    touchNumColor = (Color){ 84,00,84,255 };
                    break;
                case 8:
                    touchNumColor = (Color){ 75,75,75,255 };
                    break;
                }
            }
            DrawRectangle(boardTile.x, boardTile.y,
                          tileSize, tileSize, tileColor);
            DrawTextEx(font, tileTextSymbol, (Vector2){boardTile.x + tileSize/3, boardTile.y + tileSize/10}, fontSize, 0, touchNumColor);
        }
    }
    EndMode2D();
    //----------------------------------------------------------------------------------

    if (hp <= 0) // Lose screen
    {
        Color gameOverColor = MAROON;
        gameOverColor.a = 100;
        DrawRectangle(screenCenter.x - 200, screenCenter.y - 50, 400, 100, gameOverColor );
        gameOverColor = BEIGE;
        gameOverColor.a = 200;
        DrawTextEx(font, "GAME OVER", (Vector2) { screenCenter.x - 180, screenCenter.y - 40 },
                   font.baseSize*2, 4, gameOverColor);
    }
    else if (winCon)
    {
        Color victoryColor = DARKPURPLE;
        victoryColor.a = 100;
        DrawRectangle(screenCenter.x - 200, screenCenter.y - 50, 400, 100, victoryColor);
        victoryColor = BEIGE;
        victoryColor.a = 200;
        DrawTextEx(font, "YOU WON", (Vector2) { screenCenter.x - 180, screenCenter.y - 40 },
            font.baseSize * 2, 4, victoryColor);
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
    for (int y = 0; y < boardHeight; ++y)
    {
        for (int x = 0; x < boardWidth; ++x)
        {
            board[y][x] = 0;
            boardMask[y][x] = 0;
        }
    }
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishResult;
}