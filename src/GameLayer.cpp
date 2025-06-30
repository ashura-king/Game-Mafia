#include "includes/GameLayer.hpp"
#include <cmath>

Gamelayer::Gamelayer(const char *file, float y, float scal, float parallaxFactor)
    : yOffset(y), scale(scal), scrollX(0.0f), parallaxSpeed(parallaxFactor)
{
  texture = LoadTexture(file);
}

Gamelayer::~Gamelayer()
{
  UnloadTexture(texture);
}

void Gamelayer::UpdateLayer(float cameraDelta)
{
  scrollX -= cameraDelta * parallaxSpeed;

  float width = texture.width * scale;
  scrollX = fmod(scrollX, width);
  if (scrollX > 0)
    scrollX -= width;
}

void Gamelayer::Drawlayer()
{
  float width = texture.width * scale;
  int screenWidth = GetScreenWidth();

  // Normalize scrollX into range [-width, 0]
  float normalizedScroll = fmod(scrollX, width);
  if (normalizedScroll > 0)
    normalizedScroll -= width;

  // Start drawing slightly left of the screen
  float startX = normalizedScroll;

  // Draw two or more copies to fully cover the screen
  for (float x = startX; x < screenWidth; x += width)
  {
    DrawTextureEx(texture, {x, yOffset}, 0.0f, scale, GRAY);
  }

  // Final copy to cover small gaps at the far right
  DrawTextureEx(texture, {startX + width * (float)ceil((screenWidth - startX) / width), yOffset}, 0.0f, scale, GRAY);
}
