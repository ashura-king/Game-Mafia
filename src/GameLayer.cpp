// Gamelayer.cpp
#include "includes/Gamelayer.hpp"
#include <cmath>

Gamelayer::Gamelayer(const char *file, float y, float scal, float parallaxFactor)
    : yOffset(y), scale(scal), scrollX(0.0f), parallaxSpeed(parallaxFactor)
{
  texture = LoadTexture(file);
  SetTextureFilter(texture, TEXTURE_FILTER_POINT); // prevent blurry seams
}

Gamelayer::~Gamelayer()
{
  UnloadTexture(texture);
}

void Gamelayer::UpdateLayer(float cameraDelta)
{
  scrollX -= cameraDelta * parallaxSpeed;
  // Keep scrollX in [0, width)
  float width = texture.width * scale;
  scrollX = fmodf(scrollX, width);
  if (scrollX < 0)
    scrollX += width;
}

void Gamelayer::Drawlayer()
{
  float width = texture.width * scale;
  int screenWidth = GetScreenWidth();

  float startX = -scrollX;

  // Draw at least two textures to fill screen
  for (float x = startX; x < screenWidth; x += width)
  {
    DrawTextureEx(texture, {x, yOffset}, 0.0f, scale, WHITE);
  }

  // Extra draw to prevent 1px gap due to float rounding (just in case)
  DrawTextureEx(texture, {startX + width, yOffset}, 0.0f, scale, WHITE);
}
