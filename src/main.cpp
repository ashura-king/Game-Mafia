#include <raylib.h>
#include "includes/Layer.h"
#include "includes/Button.h"

enum class Gamestate
{
    MENU,
    GAME,   // Loading + Fade
    PLAYING // Actual game logic
};

// Outlined Text
void DrawTextOutlined(const char *text, int posX, int posY, int fontSize, Color textColor, Color outlineColor)
{
    DrawText(text, posX - 1, posY - 1, fontSize, outlineColor);
    DrawText(text, posX + 1, posY - 1, fontSize, outlineColor);
    DrawText(text, posX - 1, posY + 1, fontSize, outlineColor);
    DrawText(text, posX + 1, posY + 1, fontSize, outlineColor);
    DrawText(text, posX, posY, fontSize, textColor);
}

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

    Gamestate currentState = Gamestate::MENU;

    // Title texture
    Texture2D titleTexture = LoadTexture("resource/TitleGame.png");
    float titleScale = scale * 3.0f;
    Vector2 titlePosition = {
        (screenWidth - (titleTexture.width * titleScale)) / 2.0f,
        20.0f * scale};

    // Layers
    Layer background("resource/Sky_pale.png", 0.1f, 0, scale);
    Layer midground("resource/back.png", 0.5f, 0, scale);
    Layer houses("resource/Houses3_pale.png", 1.0f, 70, scale);
    Layer foreground("resource/houses1.png", 1.0f, 70, scale);
    Layer shop("resource/minishop&callbox.png", 1.0f, 80, scale);
    Layer road("resource/road&lamps.png", 1.0f, 75, scale);
    Layer sky("resource/sky.png", 0.1f, 0, scale);
    Layer building("resource/houses3.png", 0.5f, 0, scale);
    Layer houded("resource/night2.png", 1.0f, 70, scale);
    Layer houdes1("resource/night.png", 1.0f, 75, scale);
    Layer roads("resource/road.png", 1.0f, 75, scale);
    Layer crosswalk("resource/crosswalk.png", 1.0f, 70, scale);

    // Buttons
    Button startButton{"resource/button1.png", "resource/button2.png", "resource/button3.png", scale * 5.0f, true, 70.0f};
    Button exitButton{"resource/exit1.png", "resource/exit2.png", "resource/exit3.png", scale * 5.0f, true, 160.0f};

    bool running = true;

    // For "Please wait..." text animation
    int frameCounter = 0;
    int dotCount = 0;
    int maxDots = 3;
    std::string animatedText = " ";

    // Fade control
    int gameTimer = 0;
    const int fadeDuration = 300; // 5 seconds at 60 FPS
    bool fadeOutComplete = false;

    while (!WindowShouldClose() && running)
    {
        // ========== MENU ==========
        if (currentState == Gamestate::MENU)
        {
            background.Update();
            midground.Update();
            houses.Update();
            foreground.Update();
            shop.Update();
            road.Update();

            startButton.Update();
            exitButton.Update();

            if (startButton.IsClicked())
            {
                currentState = Gamestate::GAME;
                gameTimer = 0;
                fadeOutComplete = false;
            }

            if (exitButton.IsClicked())
            {
                running = false;
            }

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

        // ========== LOADING WITH FADE ==========
        else if (currentState == Gamestate::GAME)
        {
            sky.Update();
            building.Update();
            houded.Update();
            houdes1.Update();
            roads.Update();
            crosswalk.Update();

            frameCounter++;
            if (frameCounter >= 30)
            {
                frameCounter = 0;
                dotCount = (dotCount + 1) % (maxDots + 1);
                animatedText = "Please wait" + std::string(dotCount, '.');
            }

            if (!fadeOutComplete)
            {
                gameTimer++;
                if (gameTimer >= fadeDuration)
                {
                    fadeOutComplete = true;
                    currentState = Gamestate::PLAYING;
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            sky.Draw();
            building.Draw();
            houded.Draw();
            houdes1.Draw();
            roads.Draw();
            crosswalk.Draw();

            DrawTextOutlined(animatedText.c_str(), 350, 270, 40, WHITE, BLACK);

            if (!fadeOutComplete)
            {
                float alpha = 1.0f - (float)gameTimer / fadeDuration;
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, alpha));
            }
            EndDrawing();
        }

        // ========== MAIN GAMEPLAY ==========
        else if (currentState == Gamestate::PLAYING)
        {
            // TODO: Replace with your actual game logic
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Game Started!", 350, 250, 30, GREEN);
            EndDrawing();
        }
    }

    UnloadTexture(titleTexture);
    CloseWindow();
    return 0;
}
