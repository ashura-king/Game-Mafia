#include "GameObject.hpp"
#include <algorithm>
#include <cmath>

GameObject::GameObject() : lastSpawnX(0.0f)
{
}

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

void GameObjec::UpdateObjects(float cameraDelta)
{
  // Update all objects
  for (auto &obj : objects)
  {
    obj->Update(cameraDelta);
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

std::vector<Platform *> GameObjectManager::CheckCollisions(const Rectangle &playerBounds)
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