#include "includes/GameLayer.hpp"

Gamelayer::Gamelayer(const char *file, float y, float scal)
    : yOffset(y), scale(scal), scrollX(0.0f)
{
  texture = LoadTexture(file);
}

Gamelayer::~Gamelayer()
{
  UnloadTexture(texture);
}

void Gamelayer::UpdateLayer(float playerSpeed)
{
  // Adjust scrollX based on player movement
  scrollX -= playerSpeed * 0.5f; // 0.5f = parallax factor (slower than player)

  // Optional: wrap scrollX for seamless repeat
  float width = texture.width * scale;
  if (scrollX <= -width)
    scrollX += width;
  if (scrollX >= width)
    scrollX -= width;
}

void Gamelayer::Drawlayer()
{
  float width = texture.width * scale;

  // Draw texture twice to ensure seamless loop
  for (float x = scrollX; x < GetScreenWidth(); x += width)
  {
    DrawTextureEx(texture, {x, yOffset}, 0.0f, scale, WHITE);
  }

  // Draw 1 more before scrollX if needed
  if (scrollX > 0)
  {
    DrawTextureEx(texture, {scrollX - width, yOffset}, 0.0f, scale, WHITE);
  }
}
