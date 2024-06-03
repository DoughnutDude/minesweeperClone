/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Screens Functions Declarations (Init, Update, Draw, Unload)
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

#ifndef SCREENS_H
#define SCREENS_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
#define local_persist static
#define global_var	  static
#define internal      static

#define maxBoardHeight 99
#define maxBoardWidth 99

typedef enum GameScreen { UNKNOWN = -1, LOGO = 0, TITLE = 1, OPTIONS = 2, GAMEPLAY = 3, ENDING = 4} GameScreen;
typedef struct Button {
	Rectangle rect;
	Color rectColor;
	Color textColor;
	char * text;
};
struct TextBox {
    Button button;
    char value[256];
};

//----------------------------------------------------------------------------------
// Macros
//----------------------------------------------------------------------------------
#define ARRAYCOUNT(arr) (sizeof(arr) / sizeof((arr)[0]))
//#define assert(expression) if(!(expression)) {*(int *) 0 = 0;}
#define ASSERT(expression)

//----------------------------------------------------------------------------------
// Global Variables Declaration (shared by several modules)
//----------------------------------------------------------------------------------
extern GameScreen currentScreen;
extern Font font;
extern Music music;
extern Sound fxCoin;

extern bool running;
extern int boardHeight;
extern int boardWidth;
extern int mineDensity;
extern int minesDesired;
extern bool mineGenMode; // 0 = by density, 1 = til maxMineCount
extern int startingHP;
extern float timer;
extern float timeStart;

extern int board[maxBoardHeight][maxBoardWidth]; // -2 = mine(clicked on), -1 = mine, non-negative = number of adjacent mines
extern int boardMask[maxBoardHeight][maxBoardWidth]; // 0 = revealed, 1 = hidden, 2 = flagged, -1 = incorrectly flagged (only used for game lose screen)

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

inline int powInt(int a, int b)
{
    int result = 1;
    for (int i = 0; i < b; ++i)
    {
        result *= a;
    }
    return result;
}

//----------------------------------------------------------------------------------
// Drawing Functions Declaration
//----------------------------------------------------------------------------------
 void DrawButton(Button button, int textOffsetX, int textOffsetY);
 void DrawTextBox(TextBox textBox, int textOffsetX, int textOffsetY);

//----------------------------------------------------------------------------------
// Logo Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitLogoScreen(void);
void UpdateLogoScreen(void);
void DrawLogoScreen(void);
void UnloadLogoScreen(void);
int FinishLogoScreen(void);

//----------------------------------------------------------------------------------
// Title Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitTitleScreen(void);
void UpdateTitleScreen(void);
void DrawTitleScreen(void);
void UnloadTitleScreen(void);
int FinishTitleScreen(void);

//----------------------------------------------------------------------------------
// Options Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitOptionsScreen(void);
void UpdateOptionsScreen(void);
void DrawOptionsScreen(void);
void UnloadOptionsScreen(void);
int FinishOptionsScreen(void);

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitGameplayScreen(void);
void UpdateGameplayScreen(void);
void DrawGameplayScreen(void);
void UnloadGameplayScreen(void);
int FinishGameplayScreen(void);

//----------------------------------------------------------------------------------
// Ending Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitEndingScreen(void);
void UpdateEndingScreen(void);
void DrawEndingScreen(void);
void UnloadEndingScreen(void);
int FinishEndingScreen(void);

#ifdef __cplusplus
}
#endif

inline Color operator-(Color a, Color b)
{
    Color result = {};
    result.r = a.r - b.r;
    result.g = a.g - b.g;
    result.b = a.b - b.b;
    return result;
}

inline Color &operator-=(Color &a, Color b)
{
    a = a - b;
    return a;
}

//----------------------------------------------------------------------------------
// Vector Operations
//----------------------------------------------------------------------------------

inline Vector2 operator+(Vector2 a, Vector2 b)
{
    Vector2 result = {};
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}
inline Vector2 &operator+=(Vector2 &a, Vector2 b)
{
    a = a + b;
    return a;
}
inline Vector2 operator-(Vector2 a, Vector2 b)
{
    Vector2 result = {};
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}
inline Vector2 &operator-=(Vector2 &a, Vector2 b)
{
    a = a - b;
    return a;
}
inline Vector2 operator*(float a, Vector2 b)
{
    Vector2 result = {};
    result.x = a * b.x;
    result.y = a * b.y;
    return result;
}
inline Vector2 operator*(Vector2 a, float b)
{
    Vector2 result = b * a;
    return result;
}
inline Vector2 &operator*=(Vector2 &a, float b)
{
    a = a * b;
    return a;
}

#endif // SCREENS_H
