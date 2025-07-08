#include "GameObject.hpp"
#include <algorithm>
#include <cmath>

GameObject::GameObject() : lastSpawnX(0.0f) {}

GameObject::~GameObject()
{
  ClearAllObjects();
}

void GameObject::AddObject(const char *texturePath, float x, float y, float scale, ObjectType type)
{
  objects.push_back(std::make_unique<Platform>(texturePath, x, y, scale, type));
}

void GameObject::RemoveInactiveObjects()
{
  objects.erase(
      std::remove_if(objects.begin(), objects.end(),
                     [](const std::unique_ptr<Platform> &obj)
                     {
                       return !obj->IsActive();
                     }),
      objects.end());
}

void GameObject::ClearAllObjects()
{
  objects.clear();
}

void GameObject::UpdateObjects(float cameraDelta)
{
  // Updated to handle camera position-based deactivation
  // Note: cameraDelta is now treated as cameraX position for deactivation checks
  float cameraX = cameraDelta; // Reinterpret parameter as camera position
  float screenWidth = 800.0f;  // Default screen width - should be passed from Controller

  // Update all objects - don't move them with camera delta
  // Objects should stay in their world positions
  for (auto &obj : objects)
  {
    // Check if object should be deactivated
    obj->CheckDeactivation(cameraX, screenWidth);
  }

  // Remove inactive objects periodically
  static int frameCount = 0;
  if (++frameCount % 60 == 0)
  { // Clean up every 60 frames
    RemoveInactiveObjects();
  }
}

void GameObject::UpdateObjects(float cameraX, float screenWidth)
{
  // Update all objects - don't move them with camera delta
  // Objects should stay in their world positions
  for (auto &obj : objects)
  {
    // Check if object should be deactivated
    obj->CheckDeactivation(cameraX, screenWidth);
  }

  // Remove inactive objects periodically
  static int frameCount = 0;
  if (++frameCount % 60 == 0)
  { // Clean up every 60 frames
    RemoveInactiveObjects();
  }
}

void GameObject::DrawObjects()
{
  // Draw objects in order (back to front if needed)
  for (auto &obj : objects)
  {
    obj->Draw();
  }
}

std::vector<Platform *> GameObject::CheckCollisions(const Rectangle &playerBounds)
{
  std::vector<Platform *> collisions;

  for (auto &obj : objects)
  {
    if (obj->CheckCollision(playerBounds))
    {
      collisions.push_back(obj.get());
    }
  }

  return collisions;
}

Platform *GameObject::GetNearestObject(float x, float y, float maxDistance)
{
  Platform *nearest = nullptr;
  float minDistance = maxDistance;

  for (auto &obj : objects)
  {
    if (!obj->IsActive())
      continue;

    Vector2 objPos = obj->GetPosition();
    float distance = sqrt((objPos.x - x) * (objPos.x - x) + (objPos.y - y) * (objPos.y - y));

    if (distance < minDistance)
    {
      minDistance = distance;
      nearest = obj.get();
    }
  }

  return nearest;
}

// Add method to get objects vector for iteration (if needed)
// const std::vector<std::unique_ptr<Platform>>& GameObject::GetObjects() const
// {
//   return objects;
// }

// GetObjectCount() is already defined inline in the header as size_t
// No need to implement it again here