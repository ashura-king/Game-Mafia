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
} class Platform
{
public:
  Platform(conts char *texturePath, float x, float y, float scale = 1.0f, ObjectType type = ObjectType::OBSTACLE);
  ~Platform();

  void Update(float cameraDelta);
  void Draw();

  Rectangle GetBoundBox() const;
  bool CheckCollision(const Rectangle &other) const;

  // Getters;
  void GetPosition() const { return position; }
  float GetScale() const { return scale; }
  ObjectType GetType() const { return type; }
  bool IsActive() const { return active; }
  // Setter;
  void SetPosition(float x, float y);
  void SetActive(bool IsActive) { return active = IsActive; }

private:
  Texture2D texture;
  Vector2 position;
  ObjectType type;
  bool active;

  // collision
  Vector2 collisionOffset;
  Vector2 collisionSize;
};