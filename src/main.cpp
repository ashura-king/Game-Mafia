#include <raylib.h>
#include "Layer.h"
#include "Button.h"

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

    // Create each layer
    Layer background("resource/Sky_pale.png", 0.1f, 0, scale);
    Layer midground("resource/back.png", 0.5f, 0, scale);
    Layer houses("resource/Houses3_pale.png", 1.0f, 70, scale);
    Layer foreground("resource/houses1.png", 1.0f, 70, scale);
    Layer shop("resource/minishop&callbox.png", 1.0f, 80, scale);
    Layer road("resource/road&lamps.png", 1.0f, 75, scale);

    // Create Button - auto-centered and scaled, moved down a bit
    Button startButton{"resource/button2.png", scale * 5.0f, true, 10.0f};
    Button exitButton{"resource/button2.png", scale * 5.0f, true, 140.0f};
    while (!WindowShouldClose())
    {
        // Update
        background.Update();
        midground.Update();
        houses.Update();
        foreground.Update();
        shop.Update();
        road.Update();
        startButton.Update();
        if (startButton.IsClicked())
        {
            printf("Button clicked!\n");
        }
        if (exitButton.IsClicked())
        {
            printf("Exit button clicked\n");
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

        // Draw button on top
        startButton.Draw();
        exitButton.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}