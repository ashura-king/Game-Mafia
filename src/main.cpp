#include <raylib.h>
#include "includes/Layer.hpp"
#include "includes/Button.hpp"
#include "includes/TextOutlined.hpp"
#include "includes/GameLayer.hpp"
#include "includes/GameType.hpp"
#include "includes/Character.hpp"
#include "includes/Bot.hpp"
#include "includes/Popup.hpp"
#include <vector>
#include <string>

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

    Sound clickSound = LoadSound("Audio/start.mp3");
    Music backgroundMusic = LoadMusicStream("Audio/Intro1.mp3");
    Music playingMusic = LoadMusicStream("Audio/PlayingSound.mp3");

    Gamestate currentState = Gamestate::MENU;

    Texture2D titleTexture = LoadTexture("resource/TitleGame.png");
    float titleScale = scale * 3.0f;
    Vector2 titlePosition = {
        (screenWidth - (titleTexture.width * titleScale)) / 2.0f,
        20.0f * scale};

    Character player("resource/Idle.png",
                     "resource/Idle_2.png",
                     "resource/Walk.png",
                     "resource/Run.png",
                     "resource/Shot.png",
                     "resource/Jump.png",
                     "resource/Attack_1.png",
                     "Audio/Gun.mp3",
                     "Audio/Attack.mp3",
                     "resource/bullet.png",
                     120.0f, 270.0f, 2.0f);
    Bot bot("resource/enemyIdle.png",
            "resource/enemyIdle2.png",
            "resource/enemyWalk.png",
            "resource/enemyRun.png",
            "resource/enemyAttack.png",
            600.0f,
            150.0f,
            3.0f);

    player.SetJumpSpeed(15.0f);
    player.SetGravity(0.8f);
    player.SetGroundY(270.0f);
    player.SetFireCooldown(0.3f);
    player.SetGunshotVolume(0.7f);

    // Game layers
    Gamelayer mainsky("resource/mainsky.png", 0.0f, scale);
    Gamelayer backhouse("resource/housemain2.png", 0.0f, scale);
    Gamelayer middlehouse("resource/housemain.png", 0.0f, scale);
    Gamelayer fronthouse("resource/housemain1.png", 0.0f, scale);
    Gamelayer fountain("resource/fountain&bush.png", 0.0f, scale);
    Gamelayer policebox("resource/policebox.png", 0.0f, scale);
    Gamelayer mainroad("resource/mainroad.png", 0.0f, scale);
    std::vector<Gamelayer *> mainlayers = {&mainsky, &backhouse, &middlehouse, &fronthouse, &fountain, &policebox, &mainroad};

    Layer background("resource/Sky_pale.png", 0.1f, 0, scale);
    Layer midground("resource/back.png", 0.5f, 0, scale);
    Layer houses("resource/Houses3_pale.png", 1.0f, 70, scale);
    Layer foreground("resource/houses1.png", 1.0f, 70, scale);
    Layer shop("resource/minishop&callbox.png", 1.0f, 80, scale);
    Layer road("resource/road&lamps.png", 1.0f, 75, scale);
    std::vector<Layer *> menuLayers = {&background, &midground, &houses, &foreground, &shop, &road};

    Layer sky("resource/sky.png", 0.1f, 0, scale);
    Layer building("resource/houses3.png", 0.5f, 0, scale);
    Layer houded("resource/night2.png", 1.0f, 70, scale);
    Layer houdes1("resource/night.png", 1.0f, 75, scale);
    Layer roads("resource/road.png", 1.0f, 75, scale);
    Layer crosswalk("resource/crosswalk.png", 1.0f, 70, scale);
    std::vector<Layer *> gameLayers = {&sky, &building, &houded, &houdes1, &roads, &crosswalk};

    Button startButton("resource/button1.png", "resource/button2.png", "resource/button3.png", scale * 5.0f, true, 70.0f);
    Button exitButton("resource/exit1.png", "resource/exit2.png", "resource/exit3.png", scale * 5.0f, true, 160.0f);
    Button yesButton("resource/yes.png", "resource/yes2.png", "resource/yes3.png", 2.5f);
    Button noButton("resource/no.png", "resource/no2.png", "resource/no3.png", 2.5f);

    int frameCounter = 0;
    int dotCount = 0;
    const int maxDots = 3;
    std::string animatedText = " ";
    int gameTimer = 0;
    const int fadeDuration = 300;
    bool fadeOutComplete = false;
    bool playingMusicStarted = false;
    bool running = true;
    bool showExitPop = false;
    Popup popup;

    PlayMusicStream(backgroundMusic);

    while (!WindowShouldClose() && running)
    {
        switch (currentState)
        {
        case Gamestate::MENU:
            UpdateMusicStream(backgroundMusic);

            for (Layer *layer : menuLayers)
                layer->Update();

            if (!showExitPop)
            {
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
                    showExitPop = true;
                }
            }

            BeginDrawing();
            ClearBackground(GetColor(0x052c46ff));

            for (Layer *layer : menuLayers)
                layer->Draw();
            startButton.Draw();
            exitButton.Draw();
            DrawTextureEx(titleTexture, titlePosition, 0.0f, titleScale, WHITE);

            if (showExitPop)
            {
                popup.DrawExitPopup(running, showExitPop, clickSound, yesButton, noButton);
            }

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

            player.HandleInput();
            player.Update();
            Vector2 playerPos = {player.GetX(), player.GetY()};
            float deltaTime = GetFrameTime();
            bot.Update();
            bot.UpdateAI(playerPos, deltaTime);

            float backgroundSpeed = player.GetCurrentMovementSpeed();

            for (Gamelayer *main : mainlayers)
                main->UpdateLayer(backgroundSpeed);

            if (!playingMusicStarted)
            {
                StopMusicStream(backgroundMusic);
                PlayMusicStream(playingMusic);
                SetMusicVolume(playingMusic, 0.5f);
                playingMusicStarted = true;
            }

            UpdateMusicStream(playingMusic);

            BeginDrawing();
            ClearBackground(WHITE);

            for (Gamelayer *main : mainlayers)
                main->Drawlayer();
            bot.Draw();
            player.Draw();

            EndDrawing();
            break;
        }
        }
    }

    UnloadMusicStream(playingMusic);
    UnloadMusicStream(backgroundMusic);
    UnloadSound(clickSound);
    UnloadTexture(titleTexture);
    CloseWindow();

    return 0;
}
