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
#include "raymath.h"

#include <stdio.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
global_var int framesCounter = 0;
global_var int finishResult = 0;
global_var float dt = 0.0f;

bool winCon;
int hp = 1;
int actionCount = 0;

Vector2 screenCenter = { 0 };
Vector2 cameraPos = { 0 };
Camera2D camera = { 0 };
Rectangle player = { 0 };

int mineCount = 0;
int maxMines = 0;
#define boardHeight 20 // in number of tiles
#define boardWidth 30
int board[boardHeight][boardWidth] = { 0 }; // -2 = mine(clicked on), -1 = mine, non-negative = number of adjacent mines
int boardMask[boardHeight][boardWidth] = { 0 }; // 0 = revealed, 1 = hidden, 2 = flagged
Rectangle boardRect = { 0 };
float tileSize = 40.0f;
Vector2 boardCenter = { 0 };
int textSize = 0;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
int DetectMinesTouchingTile(int x, int y)
{
    int result = 0;
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
                    board[neighborY][neighborX] < 0)
                {
                    ++result;
                }
            }
        }
    }
    return result;
}

// Increment the clue count of each adjacent tile to let them know that they are touching a mine (board[y][x]).
void PingTilesTouchingMine(int x, int y)
{
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

void GenerateMinesRecursively(int x, int y)
{
    if (board[y][x] < 0 || mineCount >= maxMines)
    {
        return;
    }
    board[y][x] = -1;
    ++mineCount;
    PingTilesTouchingMine(x, y);
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

void FloodFillClearTilesRecursively(int x, int y)
{
    if (((x < boardWidth) && (x >= 0) && (y < boardHeight) && (y >= 0)) && (board[y][x] >= 0) && (boardMask[y][x] == 1))
    {
        boardMask[y][x] = 0;

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
                        (boardMask[neighborY][neighborX] == 1 || (board[neighborY][neighborX] >= 0)))
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
            } else return;
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
                FloodFillClearTilesRecursively(x - 1, y - 1);//north-west
            } else return;
        } else return;
#endif
    }
}

void AttemptTileReveal(int x, int y)
{
    if (boardMask[y][x] == 1)
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
            boardMask[y][x] = 0;
        }
    }
}

Rectangle MakeRectFromTile(int x, int y)
{
    Rectangle result = { x * tileSize,
                         y * tileSize,
                         tileSize, tileSize };
    return result;
}

//
// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishResult = 0;
    dt = GetFrameTime();

    winCon = false;
    hp = 3;
    actionCount = 0;

    screenCenter.x = (float)GetScreenWidth() / 2.0f;
    screenCenter.y = (float)GetScreenHeight() / 2.0f;

    boardCenter = { (tileSize * boardWidth) / 2.0f, (tileSize * boardHeight) / 2.0f };
    boardRect = { 0, 0, (tileSize * boardWidth), (tileSize * boardHeight) };

    cameraPos = boardCenter;

    camera.target = cameraPos;
    camera.offset = screenCenter;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    textSize = 0.8f * (float)tileSize;

    // init board
    mineCount = 0;
    maxMines = 0.21f * (boardWidth * boardHeight); // TODO: replace constant val with mine density var

    for (int y = 0; y < boardHeight; ++y)
    {
        for (int x = 0; x < boardWidth; ++x)
        {
            boardMask[y][x] = 1; // Set this to 0 to start with board revealed.
        }
    }
#if 1
    int x = GetRandomValue(0, boardWidth-1);
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
            PingTilesTouchingMine(x, y);
        }
    }
#else
    for (int y = 0; y < boardHeight; ++y)
    {
        for (int x = 0; x < boardWidth; ++x)
        {
            // TODO: Parameterize board generation for the possibility of
            //no-guess modes, adjustable mine density, dynamic procedural/random gen, etc.
            int randVal = GetRandomValue(0,5); // ~20% mine density. Less consistent as density value goes down.
            if (!randVal) {
                board[y][x] = -1;
                ++mineCount;
                PingTilesTouchingMine(x, y);
            }
        }
    }
#endif
    char str[16];
    sprintf(str, "mines: %d\n",mineCount);
    printf(str);
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
#if 1
    ++framesCounter;
    dt = GetFrameTime();

    screenCenter.x = (float)GetScreenWidth() / 2;
    screenCenter.y = (float)GetScreenHeight() / 2;
    camera.offset = screenCenter;

    boardRect = { 0, 0, (tileSize * boardWidth),
                  (tileSize * boardHeight) };

    float scrollSpeedX = 3.0f;
    float scrollSpeedY = 3.0f;

    if (IsKeyDown(KEY_W)) cameraPos.y -= scrollSpeedY;
    if (IsKeyDown(KEY_S)) cameraPos.y += scrollSpeedY;
    if (IsKeyDown(KEY_A)) cameraPos.x -= scrollSpeedX;
    if (IsKeyDown(KEY_D)) cameraPos.x += scrollSpeedX;

#if 0 // For camera rotation. Would need adjustment of mouse coords.
    if (IsKeyDown(KEY_E)) camera.rotation--;
    else if (IsKeyDown(KEY_Q)) camera.rotation++;
    if (camera.rotation > 40) camera.rotation = 40;
    else if (camera.rotation < -40) camera.rotation = -40;
#endif

#if 0 // For camera zoom. Would need adjustment of mouse coords.
    camera.zoom += ((float)GetMouseWheelMove() * 0.05f);
    if (camera.zoom > 3.0f) camera.zoom = 3.0f;
    else if (camera.zoom < 0.1f) camera.zoom = 0.1f;
#endif

    if (IsKeyPressed(KEY_R)) cameraPos = boardCenter;
    camera.target = cameraPos;

#if 0
    if (((framesCounter) % 7) == 0) {
        --origin.y; //this is where we'd do the scrolling stuff
    }
#endif

    // Mouse capture
    if (hp > 0)
    {
        if (IsKeyPressed(KEY_P))
        {
            for (int y = 0; y < boardHeight; ++y)
            {
                for (int x = 0; x < boardWidth; ++x)
                {
                    boardMask[y][x] = 0;
                }
            }
        }
        bool clickL = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        bool clickR = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
        bool clickM = IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE);
        Vector2 mousePos = GetMousePosition() + camera.target - camera.offset;
        if (clickL != clickR != clickM)
        {
            if (CheckCollisionPointRec(mousePos, boardRect))
            {
                int x = (mousePos.x)/tileSize;
                int y = (mousePos.y)/tileSize;
                Rectangle boardTile;

                if (clickL && boardMask[y][x] == 1)
                {
                    ++actionCount;
                    printf("ac %d\n", actionCount);//debug output
                    if (actionCount == 1) // Move mines away from first click and its adjacent tiles.
                    {
                        for (int offsetY = -1; offsetY < 2; ++offsetY)
                        {
                            for (int offsetX = -1; offsetX < 2; ++offsetX)
                            {
                                int neighborX = x + offsetX;
                                int neighborY = y + offsetY;
                                if ((neighborX < boardWidth) && (neighborX >= 0) &&
                                    (neighborY < boardHeight) && (neighborY >= 0) &&
                                    board[neighborY][neighborX] < 0)
                                {
                                    bool mineMovedSuccessfully = false;
                                    while (!mineMovedSuccessfully)
                                    {
                                        int newX = GetRandomValue(0, boardWidth-1);
                                        int newY = GetRandomValue(0, boardHeight-1);
                                        // Make sure new random tile is not in neighborhood
                                        if (!((newX <= x + 1) && (newX >= x - 1) &&
                                            (newY <= y + 1) && (newY >= y - 1)) &&
                                            board[newY][newX] != -1)
                                        {
                                            board[neighborY][neighborX] = DetectMinesTouchingTile(neighborX, neighborY);
                                            for (int offsetY2 = -1; offsetY2 < 2; ++offsetY2)
                                            {
                                                for (int offsetX2 = -1; offsetX2 < 2; ++offsetX2)
                                                {
                                                    if (!((offsetX2 == 0) && (offsetY2 == 0)))
                                                    {
                                                        int neighborX2 = neighborX + offsetX2;
                                                        int neighborY2 = neighborY + offsetY2;
                                                        if ((neighborX2 < boardWidth) && (neighborX2 >= 0) &&
                                                            (neighborY2 < boardHeight) && (neighborY2 >= 0) &&
                                                            board[neighborY2][neighborX2] > 0)
                                                        {
                                                            --board[neighborY2][neighborX2];
                                                            //board[neighborY2][neighborX2] = DetectMinesTouchingTile(neighborX2, neighborY2);
                                                        }
                                                    }
                                                }
                                            }
                                            board[newY][newX] = -1;
                                            PingTilesTouchingMine(newX, newY);
                                            mineMovedSuccessfully = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    AttemptTileReveal(x, y);
                }
                else if (clickR)
                {
                    if (boardMask[y][x] == 1)
                    {
                        ++actionCount;
                        printf("ac %d\n", actionCount);//debug output
                        boardMask[y][x] = 2;
                    }
                    else if (boardMask[y][x] == 2)
                    {
                        ++actionCount;
                        printf("ac %d\n", actionCount);//debug output
                        boardMask[y][x] = 1;
                    }
                }
                else if (clickM && boardMask[y][x] == 0)
                {
                    // [Chording]:
                    // First make sure that the number of adjacent flagged/mine-havin tiles
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
                                    (boardMask[neighborY][neighborX] == 2 || (board[neighborY][neighborX] == -2) ||
                                    (boardMask[neighborY][neighborX] == 0 && board[neighborY][neighborX] == -1)))
                                {
                                    ++numOfAdjacentFlags;
                                }
                            }
                        }
                    }
                    // Then reveal all adjacent tiles
                    if (numOfAdjacentFlags == board[y][x])
                    {
                        ++actionCount;
                        printf("ac %d\n", actionCount);//debug output
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
                        if (!(boardMask[y][x] == 0 || board[y][x] < 0))
                        {
                            winCon = false;
                            break;
                        }
                    }
                }
                winCon = winCon && (hp > 0); // We also make sure that the player has health remaining.
            }
        }
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
                    boardMask[y][x] = 0;
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
#endif
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
#if 1
    Vector2 mousePos = {};
    bool clickL = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    bool clickM = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);
    if (clickL || clickM)
    {
        mousePos = GetMousePosition() + camera.target - camera.offset;
    }

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), DARKGRAY); // Draw backdrop

    //----------------------------------------------------------------------------------
    BeginMode2D(camera); // Everything within the 2D mode gets affected by camera movement/transformations
    
    // Draw minesweeper board
    DrawRectangleLines(boardRect.x-1, boardRect.y-1, boardRect.width+2, boardRect.height+2, SKYBLUE); // Board outline/border
    for (int y = 0; y < boardHeight; ++y)
    {
        for (int x = 0; x < boardWidth; ++x)
        {
            Rectangle boardTile = MakeRectFromTile(x,y);
            Color tileColor = { 120,120,120,255 };
            Color textColor = BLACK;
            char tileTextSymbol[4];

            if (boardMask[y][x] > 0) // Draw hidden tile
            {
                textColor = { 0,0,0,0 };
                if (!(clickL && CheckCollisionPointRec(mousePos, boardTile)))
                {
                    tileColor = { 150,150,150,255 };
                    if (boardMask[y][x] == 2)
                    {
                        sprintf(tileTextSymbol, "X\n");
                        textColor = BROWN; // Draw flag
                    }
                }
                if (clickM)
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
                                (boardMask[y][x] != 2) && CheckCollisionPointRec(mousePos, neighborTile))
                                //mousePos += cameraPos - boardCenter; Vector2{-tileSize,tileSize}
                            {
                                textColor = { 0,0,0,0 };
                                tileColor = { 120,120,120,255 };
                            }
                        }
                    }
                }
            }
            else // Comment out this line to see tile values through the mask.
            {
                sprintf(tileTextSymbol, "%d\n", board[y][x]);
                switch (board[y][x])
                {
                case -2:
                    tileColor = DARKBROWN;
                case -1:
                    textColor = { 255,255,255,255 };
                    sprintf(tileTextSymbol, "X\n");
                    break;
                case 0:
                    textColor = { 0,0,0,0 };
                    break;
                case 1:
                    textColor = { 0,0,255,255 };
                    break;
                case 2:
                    textColor = { 00,90,00,255 };
                    break;
                case 3:
                    textColor = { 200,00,00,255 };
                    break;
                case 4:
                    textColor = { 00,00,90,255 };
                    break;
                case 5:
                    textColor = { 90,10,10,255 };
                    break;
                case 6:
                    textColor = { 03,82,84,255 };
                    break;
                case 7:
                    textColor = { 10,10,10,255 };
                    break;
                case 8:
                    textColor = { 75,75,75,255 };
                    break;
                }
            }
            DrawRectangle(boardTile.x, boardTile.y, tileSize, tileSize, tileColor);
            tileColor.r -= 20;
            tileColor.g -= 20;
            tileColor.b -= 20;
            DrawRectangleLines(boardTile.x, boardTile.y, tileSize, tileSize, tileColor);
            if (textColor.a > 0)
            {
                DrawTextEx(font, tileTextSymbol, { boardTile.x + tileSize / 3, boardTile.y + tileSize / 10 }, textSize, 0, textColor);
            }
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
        DrawTextEx(font, "GAME OVER", { screenCenter.x - 180, screenCenter.y - 40 },
                   font.baseSize*2, 4, gameOverColor);
    }
    else if (winCon)
    {
        Color victoryColor = DARKPURPLE;
        victoryColor.a = 100;
        DrawRectangle(screenCenter.x - 200, screenCenter.y - 50, 400, 100, victoryColor);
        victoryColor = BEIGE;
        victoryColor.a = 200;
        DrawTextEx(font, "YOU WON", { screenCenter.x - 180, screenCenter.y - 40 },
            font.baseSize * 2, 4, victoryColor);
    }

    Vector2 pos = {5,10};
    Color uiBackdropColor = DARKPURPLE;
    uiBackdropColor.a = 100;
    DrawRectangle(0, 0, 40, 40, uiBackdropColor);
    DrawTextEx(font, "ESC", pos, font.baseSize/2, 4, BEIGE);
#endif
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    for (int y = 0; y < boardHeight; ++y)
    {
        for (int x = 0; x < boardWidth; ++x)
        {
            board[y][x] = 0;
            boardMask[y][x] = 1;
        }
    }
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishResult;
}