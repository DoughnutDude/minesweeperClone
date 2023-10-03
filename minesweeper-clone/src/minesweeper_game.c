/*******************************************************************************************
*
*   raylib game template
*
*   Game title: Minesweeper Clone
*   Game description: <Game description>
*       Copyright (c) 2023 (DoughnutDude)
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

/**********************************************************************************************
*   Modified from the original software for use in Minesweeper Clone
*   Copyright (c) 2023 (DoughnutDude)
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"    // NOTE: Declares global (extern) variables and screens functions

#include "stdio.h"
#include "time.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
// NOTE: Those variables are shared between modules through screens.h
//----------------------------------------------------------------------------------
GameScreen currentScreen = LOGO;
Font font = { 0 };
Music music = { 0 };
Sound fxCoin = { 0 };

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
global_var const int screenWidth = 1280;
global_var const int screenHeight = 720;

// Required variables to manage screen transitions (fade-in, fade-out)
global_var float transAlpha = 0.0f;
global_var bool onTransition = false;
global_var bool transFadeOut = false;
global_var int transFromScreen = -1;
global_var GameScreen transToScreen = UNKNOWN;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
internal void ChangeToScreen(int screen);     // Change to screen, no transition effect

internal void UpdateDrawFrame(void);          // Update and draw one frame

internal void CustomLog(int msgType, const char* text, va_list args);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    SetTraceLogCallback(CustomLog);

    InitWindow(screenWidth, screenHeight, "bepis Minesweeper");
    SetExitKey(KEY_KP_0);

    InitAudioDevice();      // Initialize audio device

    // Load global data (assets that must be available in all screens, i.e. font)
    ChangeDirectory("../../../src");
    font = LoadFont("resources/Inconsolata-ExtraBold.ttf");
    //music = LoadMusicStream("resources/ambient.ogg");
    fxCoin = LoadSound("resources/coin.wav");
    ChangeDirectory("../projects/VS2022/raylib_game");

    SetMusicVolume(music, 1.0f);
    PlayMusicStream(music);

    // Setup and init first screen
    currentScreen = LOGO;
    InitLogoScreen();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Unload current screen data before closing
    switch (currentScreen)
    {
        case LOGO: UnloadLogoScreen(); break;
        case TITLE: UnloadTitleScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case ENDING: UnloadEndingScreen(); break;
        default: break;
    }

    // Unload global data loaded
    UnloadFont(font);
    UnloadMusicStream(music);
    UnloadSound(fxCoin);

    CloseAudioDevice();     // Close audio context

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
// Change to next screen, no transition
internal void ChangeToScreen(GameScreen screen)
{
    // Unload current screen
    switch (currentScreen)
    {
        case LOGO: UnloadLogoScreen(); break;
        case TITLE: UnloadTitleScreen(); break;
        case OPTIONS: UnloadOptionsScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case ENDING: UnloadEndingScreen(); break;
        default: break;
    }

    // Init next screen
    switch (screen)
    {
        case LOGO: InitLogoScreen(); break;
        case TITLE: InitTitleScreen(); break;
        case OPTIONS: InitOptionsScreen(); break;
        case GAMEPLAY: InitGameplayScreen(); break;
        case ENDING: InitEndingScreen(); break;
        default: break;
    }

    currentScreen = screen;
}

// Update and draw game frame
internal void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    UpdateMusicStream(music);       // NOTE: Music keeps playing between screens
    int finishResult = 0;
    switch(currentScreen)
    {
        case LOGO:
        {
            UpdateLogoScreen();
            finishResult = FinishLogoScreen();

        } break;
        case TITLE:
        {
            UpdateTitleScreen();
            finishResult = FinishTitleScreen();

        } break;
        case OPTIONS:
        {
            UpdateOptionsScreen();
            finishResult = FinishOptionsScreen();

        } break;
        case GAMEPLAY:
        {
            UpdateGameplayScreen();
            finishResult = FinishGameplayScreen();

        } break;
        case ENDING:
        {
            UpdateEndingScreen();
            finishResult = FinishEndingScreen();

        } break;
        default: break;
    }
    if (finishResult > 0)
    {
        ChangeToScreen((GameScreen)finishResult);
    }

    if ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)))
    {
        if (IsWindowFullscreen())
        {
            ToggleFullscreen();
            SetWindowSize(screenWidth, screenHeight);
        }
        else
        {
            int monitor = GetCurrentMonitor();
            SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
            ToggleFullscreen();
        }
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);

        switch(currentScreen)
        {
            case LOGO: DrawLogoScreen(); break;
            case TITLE: DrawTitleScreen(); break;
            case OPTIONS: DrawOptionsScreen(); break;
            case GAMEPLAY: DrawGameplayScreen(); break;
            case ENDING: DrawEndingScreen(); break;
            default: break;
        }

        // Draw full screen rectangle in front of everything

        //DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}

// Logger
internal void CustomLog(int msgType, const char* text, va_list args)
{
    char timeStr[64] = { 0 };
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);

    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s] ", timeStr);

    switch (msgType)
    {
    case LOG_INFO: printf("[INFO] : "); break;
    case LOG_ERROR: printf("[ERROR]: "); break;
    case LOG_WARNING: printf("[WARN] : "); break;
    case LOG_DEBUG: printf("[DEBUG]: "); break;
    default: break;
    }

    vprintf(text, args);
    printf("\n");
}
