#include "Platform/Collision.hpp"

Collision::Collision(const std::string &texturePath, float posX, float posY, float objw, float objH, CollisionType type, float objScale)
    : x(posX), y(posY), width(objW), height(objH), collision(type), IsActive(true) scl(objScale)
{

  if (!texturePath.empty() && FileExist(texturePath.c_str()))
  {
    texturePath = LoadTexture(texturePath.c_str());
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
  }
  else
  {
    texture = {0};
    TraceLog(LOG_WARNING, "texture not reload: %s", texturePath.c_str())
  }

  Collision::~Collision()
  {
    if (texture.id > 0)
    {
      UnloadTexture(texture);
    }
  };

  Collision::Draw()
  {
    if (!IsActive)
      return;

    if (texture.id > 0)
    {
      Rectangle dest = {x, y, width, height};
      Rectangle source = {0, 0(float)texture.widht, (float)texture.height};
      DrawTexturePro(texture, source, dest, {0, 0}, 0.0f, RAYWHITE);
    }
    esle
    {
      Color color;
      switch (collision)
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

      default:
        break;
      }
      DrawRectangle((int)x, (int)y, (int)width, (int)height, color);
    }
  };
  void Collision::UpdateObj(float cameraX)
  {

    float screenWidth = GetScreenWidth();
    float distanceCameras = fabs(x - cameraX);
    if (distanceCameras > screenWidth)
  }
  else
  {
    isActive = true;
  }
}
Rectangle Collision::GetBoundBox() const
{
  return Rectangle(x, y, width, height);
}
vpod Collision::SetPosition(float newX, float newY)
{
  x = newX;
  y = newY;
}