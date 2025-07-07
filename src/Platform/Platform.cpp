#include "Platform/Platform.hpp"
#include <raylib.h>

Platform::Platform(const char *texturePath, float x, float y, float scale, ObjectType type)
    : position({x, y}), type(type), scale(scale), active(true)
{
  texture = LoadTexture(texturePath);

  // Optional: error check texture
  if (texture.id <= 0)
  {
    TraceLog(LOG_WARNING, "Platform texture failed to load: %s", texturePath);
  }

  // Set default collision area based on texture size and scale
  collisionOffset = {0, 0};
  collisionSize = {
      static_cast<float>(texture.width) * scale,
      static_cast<float>(texture.height) * scale};
}

Platform::~Platform()
{
  UnloadTexture(texture);
}

void Platform::Update(float cameraDelta)
{
  // Move with camera
  position.x -= cameraDelta;

  // Optional: deactivate if far off-screen
  if (position.x < -texture.width * scale - 100)
  {
    active = false;
  }
}

void Platform::Draw()
{
  if (!active)
    return;

  DrawTextureEx(texture, position, 0.0f, scale, WHITE);
  DrawRectangleLinesEx(GetBoundBox(), 1, RED);

  // Optional: Debug bounding box
  // DrawRectangleLinesEx(GetBoundBox(), 1, RED);
}

Rectangle Platform::GetBoundBox() const
{
  return Rectangle{
      position.x + collisionOffset.x,
      position.y + collisionOffset.y,
      collisionSize.x,
      collisionSize.y};
}

bool Platform::CheckCollision(const Rectangle &other) const
{
  if (!active)
    return false;
  return CheckCollisionRecs(GetBoundBox(), other);
}

bool Platform::IsInCameraView(float cameraX, float cameraWidth) const
{
  float objectRight = position.x + texture.width * scale;
  float cameraRight = cameraX + cameraWidth;

  return (objectRight >= cameraX) && (position.x <= cameraRight);
}

void Platform::SetPosition(float x, float y)
{
  position = {x, y};
}
