#include <raylib.h>
#include "includes/Layer.hpp"
#include "includes/Button.hpp"
#include "includes/TextOutlined.hpp"
#include "includes/GameLayer.hpp"
#include <vector>
#include <string>

enum class Gamestate
{
    MENU,
    GAME,
    PLAYING
};

void UpdateAndDrawLayers(const std::vector<Layer *> &layers)
{
    for (Layer *layer : layers)
    {
        layer->Update();
        layer->Draw();
    }
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
    InitAudioDevice();

    // Load sounds and music
    Sound clickSound = LoadSound("Audio/Clicked.mp3");
    Music backgroundMusic = LoadMusicStream("Audio/IntroSound.mp3");
    Music playingMusic = LoadMusicStream("Audio/PlayingSound.mp3");

    Gamestate currentState = Gamestate::MENU;

    // Title texture and scaling
    Texture2D titleTexture = LoadTexture("resource/TitleGame.png");
    float titleScale = scale * 3.0f;
    Vector2 titlePosition = {
        (screenWidth - (titleTexture.width * titleScale)) / 2.0f,
        20.0f * scale};

    // Gamelayers for Main game
    Gamelayer mainsky("resource/mainsky.png", 0.0f, scale);
    Gamelayer backhouse("resource/housemain2.png", 0.0f, scale);
    Gamelayer middlehouse("resource/housemain.png", 0.0f, scale);
    Gamelayer fronthouse("resource/housemain1.png", 0.0f, scale);
    Gamelayer fountain("resource/fountain&bush.png", 0.0f, scale);
    Gamelayer policebox("resource/policebox.png", 0.0f, scale);
    Gamelayer mainroad("resource/mainroad.png", 0.0f, scale);
    std::vector<Gamelayer *> mainlayers = {&mainsky, &backhouse, &middlehouse, &fronthouse, &fountain, &policebox, &mainroad};

    // Menu Layers
    Layer background("resource/Sky_pale.png", 0.1f, 0, scale);
    Layer midground("resource/back.png", 0.5f, 0, scale);
    Layer houses("resource/Houses3_pale.png", 1.0f, 70, scale);
    Layer foreground("resource/houses1.png", 1.0f, 70, scale);
    Layer shop("resource/minishop&callbox.png", 1.0f, 80, scale);
    Layer road("resource/road&lamps.png", 1.0f, 75, scale);
    std::vector<Layer *> menuLayers = {&background, &midground, &houses, &foreground, &shop, &road};

    // Game Layers (for LoadinfScreen)
    Layer sky("resource/sky.png", 0.1f, 0, scale);
    Layer building("resource/houses3.png", 0.5f, 0, scale);
    Layer houded("resource/night2.png", 1.0f, 70, scale);
    Layer houdes1("resource/night.png", 1.0f, 75, scale);
    Layer roads("resource/road.png", 1.0f, 75, scale);
    Layer crosswalk("resource/crosswalk.png", 1.0f, 70, scale);
    std::vector<Layer *> gameLayers = {&sky, &building, &houded, &houdes1, &roads, &crosswalk};

    // Buttons
    Button startButton("resource/button1.png", "resource/button2.png", "resource/button3.png", scale * 5.0f, true, 70.0f);
    Button exitButton("resource/exit1.png", "resource/exit2.png", "resource/exit3.png", scale * 5.0f, true, 160.0f);

    int frameCounter = 0;
    int dotCount = 0;
    const int maxDots = 3;
    std::string animatedText = " ";
    int gameTimer = 0;
    const int fadeDuration = 300;
    bool fadeOutComplete = false;
    bool playingMusicStarted = false;

    PlayMusicStream(backgroundMusic);

    bool running = true;
    while (!WindowShouldClose() && running)
    {
        switch (currentState)
        {
        case Gamestate::MENU:
            UpdateMusicStream(backgroundMusic);
            UpdateAndDrawLayers(menuLayers);
            startButton.Update();
            exitButton.Update();

            if (startButton.IsClicked())
            {
                PlaySound(clickSound);
                currentState = Gamestate::GAME;
                gameTimer = 0;
                fadeOutComplete = false;
            }

            if (exitButton.IsClicked())
            {
                PlaySound(clickSound);
                running = false;
            }

            BeginDrawing();
            ClearBackground(GetColor(0x052c46ff));
            for (Layer *layer : menuLayers)
                layer->Draw();
            startButton.Draw();
            exitButton.Draw();
            DrawTextureEx(titleTexture, titlePosition, 0.0f, titleScale, WHITE);
            EndDrawing();
            break;

        case Gamestate::GAME:
            for (Layer *layer : gameLayers)
                layer->Update();

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
                float volume = 1.0f - (float)gameTimer / fadeDuration;
                SetMusicVolume(backgroundMusic, volume < 0 ? 0 : volume);

                if (gameTimer >= fadeDuration)
                {
                    fadeOutComplete = true;
                    currentState = Gamestate::PLAYING;
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            for (Layer *layer : gameLayers)
                layer->Draw();
            DrawTextOutlined(animatedText.c_str(), 350, 270, 40, WHITE, BLACK);

            if (!fadeOutComplete)
            {
                float alpha = 1.0f - (float)gameTimer / fadeDuration;
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, alpha));
            }

            EndDrawing();
            break;

        case Gamestate::PLAYING:
        {
            float playerSpeed = 0.0f;

            // Detect player input
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                playerSpeed = 2.0f; // Move right
            else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
                playerSpeed = -2.0f; // Move left

            for (Gamelayer *main : mainlayers)
                main->UpdateLayer(playerSpeed);

            if (!playingMusicStarted)
            {
                StopMusicStream(backgroundMusic);
                PlayMusicStream(playingMusic);
                SetMusicVolume(playingMusic, 0.5f);
                playingMusicStarted = true;
            }

            UpdateMusicStream(playingMusic);

            // Drawing
            BeginDrawing();
            ClearBackground(WHITE);
            for (Gamelayer *main : mainlayers)
                main->Drawlayer();

            // Optionally draw UI or character here

            EndDrawing();
            break;
        }
        }
    }

    // Cleanup
    UnloadMusicStream(playingMusic);
    UnloadMusicStream(backgroundMusic);
    UnloadSound(clickSound);
    UnloadTexture(titleTexture);
    CloseWindow();

    return 0;
}
