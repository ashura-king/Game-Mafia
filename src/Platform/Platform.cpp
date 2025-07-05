#include "Platform/Platform.hpp"

Platform::Platform(const char *texturePath, float x, float y, float scale, ObjectType type)
    : texture(LoadTexture(texturePath)), // texture is declared first
      position({x, y}),                  // matches order
      type(type),
      scale(scale),
      active(true)
{
  texture = LoadTexture(texturePath);
  SetTextureFilter(texture, TEXTURE_FILTER_POINT);

  collisionOffset = {0.0f, 0.0f};
  collisionSize = {texture.width * scale, texture.height * scale};

  switch (type)
  {
  case ObjectType::CRATE:
    collisionOffset = {0, 0};
    collisionSize = {texture.width * scale, texture.height * scale};
    break;
  case ObjectType::BARREL:
    collisionOffset = {scale * 2, scale * 2};
    collisionSize = {texture.width * scale - scale * 4, texture.height * scale - scale * 4};
    break;
  case ObjectType::OLDCAR:
    collisionOffset = {0.0f, 0.0f};
    collisionSize = {texture.width * scale, texture.height * scale};
    break;
  default:
    break;
  }
}

Platform::~Platform()
{
  UnloadTexture(texture);
}

void Platform::Update(float cameraDelta)
{
  position.x -= cameraDelta;
  if (position.x < -texture.width * scale - 100)
  {
    active = false;
  }
}
void Platform::Draw()
{
  if (!active)
    return;

  DrawTextureEx(texture, position, 0.0, scale, WHITE);

#ifdef DEBUG
  Rectangle bbox = GetBoundingBox();
  DrawRectangleLines(bbox.x, bbox.y, bbox.width, bbox.height, RED);
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
void Platform::SetPosition(float x, float y)
{
  position.x = x;
  position.y = y;
}