#include <raylib.h>
#include "includes/Layer.h"
#include "includes/Button.h"

enum class Gamestate
{
    MENU,
    GAME
};
int main()
{
    const int screenWidth = 960;
    const int screenHeight = 540;

    const int originalWidth = 1920;
    const int originalHeight = 1080;

    float scaleX = (float)screenWidth / originalWidth;
    float scaleY = (float)screenHeight / originalHeight;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    InitWindow(screenWidth, screenHeight, "Mafia City");
    SetTargetFPS(60);
    // Game STATE
    Gamestate currentState = Gamestate::MENU;

    // title game
    Texture2D titleTexture = LoadTexture("resource/TitleGame.png");
    Texture2D gameBackground = LoadTexture("resource/City3_pale.png");

    float titleScale = scale * 3.0f;
    Vector2 titlePosition = {
        (screenWidth - (titleTexture.width * titleScale)) / 2.0f,
        20.0f * scale};

    // Create each layer
    Layer background("resource/Sky_pale.png", 0.1f, 0, scale);
    Layer midground("resource/back.png", 0.5f, 0, scale);
    Layer houses("resource/Houses3_pale.png", 1.0f, 70, scale);
    Layer foreground("resource/houses1.png", 1.0f, 70, scale);
    Layer shop("resource/minishop&callbox.png", 1.0f, 80, scale);
    Layer road("resource/road&lamps.png", 1.0f, 75, scale);

    // Create Button
    bool running = true;
    Button startButton{"resource/button1.png", "resource/button2.png", "resource/button3.png", scale * 5.0f, true, 70.0f};
    Button exitButton{"resource/exit1.png", "resource/exit2.png", "resource/exit3.png", scale * 5.0f, true, 160.0f};

    while (!WindowShouldClose() && running)
    {
        // Update

        if (currentState == Gamestate::MENU)
        {
            background.Update();
            midground.Update();
            houses.Update();
            foreground.Update();
            shop.Update();
            road.Update();

            // Update both buttons
            startButton.Update();
            exitButton.Update();

            if (startButton.IsClicked())
            {
                currentState = Gamestate::GAME;
            }
            if (exitButton.IsClicked())
            {
                printf("Exit button clicked!\n");
                running = false;
            }

            BeginDrawing();
            ClearBackground(GetColor(0x052c46ff));

            // Draw layers
            background.Draw();
            midground.Draw();
            houses.Draw();
            foreground.Draw();
            shop.Draw();
            road.Draw();

            // Draw buttons on top
            startButton.Draw();
            exitButton.Draw();
            // Draw title
            DrawTextureEx(titleTexture, titlePosition, 0.0f, titleScale, WHITE);
        }
        else if (currentState == Gamestate::GAME)
        {
            DrawTextureEx(gameBackground, {0.0}, 0.0f, scale, WHITE);
            DrawText("Game is running...", 300, 250, 30, RAYWHITE);
        }
        EndDrawing();
    }
    // Cleanup
    UnloadTexture(titleTexture);
    CloseWindow();
    return 0;
}