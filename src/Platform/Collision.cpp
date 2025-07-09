#include "Platform/Collision.hpp"
#include <raylib.h>
#include <cmath>

Collision::Collision(const std::string &texturePath, float posX, float posY,
                     float objW, float objH, CollisionType collisionType, float objScale)
    : x(posX), y(posY), width(objW), height(objH), type(collisionType), IsActive(true), scale(objScale)
{
  if (!texturePath.empty() && FileExists(texturePath.c_str()))
  {
    texture = LoadTexture(texturePath.c_str());
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
  }
  else
  {
    texture = {0};
    TraceLog(LOG_WARNING, "Texture not loaded: %s", texturePath.c_str());
  }
}

Collision::~Collision()
{
  if (texture.id > 0)
  {
    UnloadTexture(texture);
  }
}

void Collision::Draw()
{
  if (!IsActive)
    return;

  if (texture.id > 0)
  {
    Rectangle dest = {x, y, width, height};
    Rectangle source = {0, 0, (float)texture.width, (float)texture.height};
    DrawTexturePro(texture, source, dest, {0, 0}, 0.0f, RAYWHITE);
  }
  else
  {
    Color color = WHITE;
    switch (type)
    {
    case CollisionType::PLATFORM:
      color = BROWN;
      break;
    case CollisionType::BLOCKING:
      color = RED;
      break;
    case CollisionType::WALL:
      color = GRAY;
      break;
    }

    DrawRectangle((int)x, (int)y, (int)width, (int)height, color);
  }
}

void Collision::Update(float cameraX)
{
  float screenWidth = GetScreenWidth();
  float distance = fabs(x - cameraX);

  if (distance > screenWidth * 2.0f)
  {
    IsActive = false;
  }
  else
  {
    IsActive = true;
  }
}

Rectangle Collision::GetBoundBox() const
{
  return {x, y, width, height};
}

void Collision::SetPosition(float newX, float newY)
{
  x = newX;
  y = newY;
}
