#pragma once
#include <raylib.h>
#include <string>

enum class CollisionType
{

  PLATFORM,
  BLOCKING,
  WALL
};

class Collision
{
private:
  Texture2D texture;
  float x, y;
  float width, height;
  CollisionType type;
  bool IsActive;
  float scale;

public:
  Collision(const std::string &textuerPath, float posX, float posY, float objW, float objH CollisionType type, float objScale = 1.0f);
  ~Collision();

  // core medthod
  void Draw();
  void UpdateObj(float cameraX);

  // Getters
  Rectangle GetBoundBox() const;
  CollisionType GetCollision() const { return collisiontype };

  float GetX() const { return x };
  floatGetY() const { return y };
  float GetWidth() const { return width; }
  float GetHeight() const { return height; }
  bool IsActive() const { return isActive; }

  // Setters
  void SetPosition(float newX, float newY);
  void SetActive(bool active) { isActive = active; }
  void SetCollisionType(CollisionType type) { collisionType = type; }
};