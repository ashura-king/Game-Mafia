#include "Platform/Platform.hpp"
#include <raylib.h>
#include <vector>

Platform::Platform(const char *texturePath, float x, float y, float scale, ObjectType type)
    : position({x, y}), type(type), scale(scale), active(true)
{
  texture = LoadTexture(texturePath);

  // Optional: error check texture
  if (texture.id <= 0)
  {
    TraceLog(LOG_WARNING, "Platform texture failed to load: %s", texturePath);
  }

  // Set collision area - position.y is the TOP of the collision box
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
  // Move with camera - but don't move if it's a static world object
  // Only move UI elements or HUD objects with camera
  // For world objects, they should stay in their world position

  // Optional: deactivate if far off-screen (use world position)
  // This should be based on camera position, not object position
  // Remove this line or modify it based on camera position passed from Controller
}

void Platform::Draw()
{
  if (!active)
    return;

  // Draw texture at the collision position
  // No need to shift - the collision box and visual should match
  DrawTextureEx(texture, position, 0.0f, scale, WHITE);

#ifdef DEBUG
  DrawRectangleLinesEx(GetBoundBox(), 1, RED); // Debug bounding box
#endif
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

// Add method to check if object should be deactivated
void Platform::CheckDeactivation(float cameraX, float screenWidth)
{
  // Deactivate if object is too far behind camera
  if (position.x + (texture.width * scale) < cameraX - screenWidth)
  {
    active = false;
  }
}