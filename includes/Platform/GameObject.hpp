
#pragma once
#include "Platform.hpp"
#include <vector>
#include <memory>

class GameObject
{
public:
  GameObjectManager();
  ~GameObjectManager();

  // Object management
  void AddObject(const char *texturePath, float x, float y, float scale = 1.0f, ObjectType type = ObjectType::OBSTACLE);
  void RemoveInactiveObjects();
  void ClearAllObjects();

  // Update and render
  void UpdateObjects(float cameraDelta);
  void DrawObjects();

  // Collision detection
  std::vector<GameObject *> CheckCollisions(const Rectangle &playerBounds);
  Platform *GetNearestObject(float x, float y, float maxDistance = 100.0f);

  // Getters
  size_t GetObjectCount() const { return objects.size(); }
  const std::vector < std::unique_ptr<Platform> &GetObjects() const { return objects; }

private:
  std::vector<std::unique_ptr<Platform>> objects;

  // Helper methods
  void SpawnObjectsAtDistance(float cameraX);
  float lastSpawnX;
};