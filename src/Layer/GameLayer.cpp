
#include "Layer/GameLayer.hpp"
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

  for (float x = startX; x < screenWidth; x += width)
  {
    DrawTextureEx(texture, {x, yOffset}, 0.0f, scale, WHITE);
  }

  DrawTextureEx(texture, {startX + width, yOffset}, 0.0f, scale, WHITE);
}
