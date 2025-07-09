#pragma once
#include <raylib.h>
#include <string>

enum class CollisionType
{
  PLATFORM, // Can land on top, pass through sides
  BLOCKING, // Blocks horizontal movement
  WALL      // Blocks all movement
};

class Collision
{
private:
  Texture2D texture;
  float x, y;
  float width, height;
  CollisionType type;
  bool IsActive; // Fixed name (was capitalized like a type)
  float scale;

public:
  // Constructor
  Collision(const std::string &texturePath, float posX, float posY,
            float objW, float objH, CollisionType type, float objScale = 1.0f);

  // Destructor
  ~Collision();

  // Core methods
  void Draw();
  void Update(float cameraX);

  // Getters
  Rectangle GetBoundBox() const;
  CollisionType GetCollision() const { return type; }

  float GetX() const { return x; }
  float GetY() const { return y; }
  float GetWidth() const { return width; }
  float GetHeight() const { return height; }
  bool GetIsActive() const { return IsActive; }

  // Setters
  void SetPosition(float newX, float newY);
  void SetActive(bool active) { IsActive = active; }
  void SetCollisionType(CollisionType newType) { type = newType; }
};
