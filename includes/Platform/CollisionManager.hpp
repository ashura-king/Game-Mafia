#pragma once
#include "Collision.hpp"
#include <vector>
#include <memory>

class CollisionManager
{
private:
  std::vector<std::unique_ptr<Collision>> objects;
  float groundY;

public:
  CollisionManager(float groundLevel);
  ~CollisionManager();

  void AddObject(const std::string &texturePath, float x, float y, float width, float height, CollisionType type);
  void AddBlockingObject(const std::string &texturePath, float x, float width, float height);
  void AddWall(const std::string &texturePath, float x, float width, float height);
  void AddPlatform(const std::string &texturePath, float x, float y, float width, float height);

  // Core
  void Update(float cameraX);
  void Draw();
  void Clear();

  // Collision Detection
  std::vector<Collision *> CheckCollision(const Rectangle &playerBounds);
  Collision *CheckHorizontalCollision(const Rectangle &playerBounds, float prevX);
  Collision *CheckVerticalCollision(const Rectangle &playerBounds);

  // Getters
  int GetObjectCount() const { return objects.size(); };
  float GetGroundY() const { return groundY; };

  // Level Loading
  void LoadFromFile(const std::string &filename);
  void CreateTestLevel(); // Testting only
};