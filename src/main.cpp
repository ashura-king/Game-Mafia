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

enum class AnimationType
{
    REPEATING,
    ONESHOT
};

enum Direction
{
    Left = -1,
    Right = 1
};

struct Animation
{
    int first;
    int last;
    int curr;
    float speed;
    float duration_left;
    int step;
    AnimationType type;
};

void Animation_Update(Animation *self)
{
    float deltaTime = GetFrameTime();
    self->duration_left -= deltaTime;

    if (self->duration_left <= 0.0f)
    {
        self->duration_left = self->speed;
        self->curr += self->step;

        if (self->curr > self->last)
        {
            switch (self->type)
            {
            case AnimationType::REPEATING:
                self->curr = self->first;
                break;
            case AnimationType::ONESHOT:
                self->curr = self->last;
                break;
            }
        }
        else if (self->curr < self->first)
        {
            switch (self->type)
            {
            case AnimationType::REPEATING:
                self->curr = self->last;
                break;
            case AnimationType::ONESHOT:
                self->curr = self->first;
                break;
            }
        }
    }
}

Rectangle animation_frame(Animation *self, int frame_width, int frame_height)
{
    int x = self->curr * frame_width;
    int y = 0;
    return Rectangle{(float)x, (float)y, (float)frame_width, (float)frame_height};
}

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

    Texture2D titleTexture = LoadTexture("resource/TitleGame.png");
    float titleScale = scale * 3.0f;
    Vector2 titlePosition = {
        (screenWidth - (titleTexture.width * titleScale)) / 2.0f,
        20.0f * scale};

    Texture2D player_idle = LoadTexture("resource/Idle.png");
    if (player_idle.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load resource/Idle.png");
    }

    Texture2D player_idle_left = LoadTexture("resource/Idle_2.png");
    if (player_idle_left.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load resource/Idle_2.png");
    }

    Texture2D player_walk = LoadTexture("resource/Walk.png");
    if (player_walk.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load resource/Walk.png");
    }

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

    int frameCounter = 0;
    int dotCount = 0;
    const int maxDots = 3;
    std::string animatedText = " ";
    int gameTimer = 0;
    const int fadeDuration = 300;
    bool fadeOutComplete = false;
    bool playingMusicStarted = false;
    bool running = true;

    float playerX = 120.0f;
    Direction player_direction = Right;
    bool isWalking = false;

    // Character dimensions
    const float playerWidth = 128 * 2;
    const float playerHeight = 128 * 2;

    PlayMusicStream(backgroundMusic);

    // Idle animation for right direction
    Animation idleRightAnim = {
        .first = 0,
        .last = 4,
        .curr = 0,
        .speed = 0.15f,
        .duration_left = 0.15f,
        .step = 1,
        .type = AnimationType::REPEATING};

    // Idle animation for left direction
    Animation idleLeftAnim = {
        .first = 0,
        .last = 4,
        .curr = 0,
        .speed = 0.15f,
        .duration_left = 0.15f,
        .step = 1,
        .type = AnimationType::REPEATING};

    // Walking animation
    Animation walkAnim = {
        .first = 0,
        .last = 5,
        .curr = 0,
        .speed = 0.08f,
        .duration_left = 0.08f,
        .step = 1,
        .type = AnimationType::REPEATING};

    while (!WindowShouldClose() && running)
    {
        switch (currentState)
        {
        case Gamestate::MENU:
            UpdateMusicStream(backgroundMusic);

            for (Layer *layer : menuLayers)
                layer->Update();

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
            float moveSpeed = 2.0f;
            isWalking = false;

            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
            {
                playerSpeed = moveSpeed;
                player_direction = Right;
                isWalking = true;
            }
            else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
            {
                playerSpeed = -moveSpeed;
                player_direction = Left;
                isWalking = true;
            }

            float newPlayerX = playerX + playerSpeed;
            float backgroundSpeed = playerSpeed;

            if (newPlayerX < 0)
            {
                newPlayerX = 0;
                backgroundSpeed = 0;
            }
            else if (newPlayerX > screenWidth - playerWidth)
            {

                newPlayerX = screenWidth - playerWidth;
            }

            playerX = newPlayerX;

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

            if (isWalking)
            {
                Animation_Update(&walkAnim);
            }
            else
            {

                if (player_direction == Right)
                {
                    Animation_Update(&idleRightAnim);
                }
                else
                {
                    Animation_Update(&idleLeftAnim);
                }
            }

            BeginDrawing();
            ClearBackground(WHITE);
            for (Gamelayer *main : mainlayers)
                main->Drawlayer();

            Texture2D currentTexture;
            Rectangle source;

            if (isWalking)
            {
                currentTexture = player_walk;
                source = animation_frame(&walkAnim, 128, 128);

                if (player_direction == Left)
                {
                    source.width = -source.width;
                }
            }
            else
            {

                if (player_direction == Right)
                {
                    currentTexture = player_idle;
                    source = animation_frame(&idleRightAnim, 128, 128);
                }
                else
                {
                    currentTexture = player_idle_left;
                    source = animation_frame(&idleLeftAnim, 128, 128);
                }
            }

            Rectangle dest = {playerX, 270, playerWidth, playerHeight};
            Vector2 origin = {0, 0};

            DrawTexturePro(currentTexture, source, dest, origin, 0.0f, WHITE);

            EndDrawing();
            break;
        }
        }
    }

    UnloadMusicStream(playingMusic);
    UnloadMusicStream(backgroundMusic);
    UnloadSound(clickSound);
    UnloadTexture(titleTexture);
    UnloadTexture(player_idle);
    UnloadTexture(player_idle_left);
    UnloadTexture(player_walk);
    CloseWindow();

    return 0;
}