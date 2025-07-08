#pragma once
#include <raylib.h>
#include <vector>
#include <string>

enum class ObjectType
{
  BARREL,
  CRATE,
  OLDCAR,
  OBSTACLE
};

class Platform
{
public:
  Platform(const char *texturePath, float x, float y, float scale = 1.0f, ObjectType type = ObjectType::OBSTACLE);
  ~Platform();

  void Update(float cameraDelta);
  void Draw();

  Rectangle GetBoundBox() const;
  bool CheckCollision(const Rectangle &other) const;
  bool IsInCameraView(float cameraX, float cameraWidth) const;

  // Getters
  Vector2 GetPosition() const { return position; }
  float GetScale() const { return scale; }
  ObjectType GetType() const { return type; }
  bool IsActive() const { return active; }

  // Setters
  void SetPosition(float x, float y);
  void SetActive(bool isActive) { active = isActive; }
  void CheckDeactivation(float cameraX, float screenWidth);

private:
  Texture2D texture;
  Vector2 position;
  ObjectType type;
  float scale;
  bool active;

  // Collision
  Vector2 collisionOffset;
  Vector2 collisionSize;
};
