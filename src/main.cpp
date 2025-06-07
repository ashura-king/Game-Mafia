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
    Layer sky("resource/sky.png", 0.1f, 0, scale);
    Layer building("resource/houses3.png", 0.5f, 0, scale);
    Layer houded("resource/night.png", 1.0f, 70, scale);
    Layer houdes1("resource/night2.png", 1.0f, 70, scale);
    Layer crosswalk("resource/crosswalk.png", 1.0f, 70, scale);

    // Create Button
    bool running = true;
    Button startButton{"resource/button1.png", "resource/button2.png", "resource/button3.png", scale * 5.0f, true, 70.0f};
    Button exitButton{"resource/exit1.png", "resource/exit2.png", "resource/exit3.png", scale * 5.0f, true, 160.0f};

    int framerCounter = 0;
    int Dot = 0;
    int maxDot = 3;
    std::string animatedText = "Loading Please Wait";

    while (!WindowShouldClose() && running)
    {
        // MENU STATE::
        if (currentState == Gamestate::MENU)
        {
            background.Update();
            midground.Update();
            houses.Update();
            foreground.Update();
            shop.Update();
            road.Update();
            // Creating Buttons
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
            // Beging draw for Menu state
            BeginDrawing();
            ClearBackground(GetColor(0x052c46ff));
            background.Draw();
            midground.Draw();
            houses.Draw();
            foreground.Draw();
            shop.Draw();
            road.Draw();

            startButton.Draw();
            exitButton.Draw();

            DrawTextureEx(titleTexture, titlePosition, 0.0f, titleScale, WHITE);

            EndDrawing();
        }
        // GAMESTATE GAME
        else if (currentState == Gamestate::GAME)
        {
            // Update game layers
            sky.Update();
            building.Update();
            houded.Update();
            houdes1.Update();
            crosswalk.Update();

            // Update animation for dots
            framerCounter++;
            if (framerCounter >= 30)
            {
                framerCounter = 0;
                Dot = (Dot + 1) % (maxDot + 1);
                animatedText = "Game is running" + std::string(Dot, '.');
            }
            // Creating Game Loader
            BeginDrawing();
            ClearBackground(RAYWHITE);

            sky.Draw();
            building.Draw();
            houded.Draw();
            houdes1.Draw();
            crosswalk.Draw();

            DrawText(animatedText.c_str(), 300, 250, 30, WHITE);

            EndDrawing();
        }
    }
    // Cleanup
    UnloadTexture(titleTexture);
    CloseWindow();
    return 0;
}