#include "Platform/CollisionManager.hpp"
#include <fstream>
#include <sstream>

CollisionManager::CollisionManager(float groundLevel) : groundY(groundLevel) {}

CollisionManager::~CollisionManager()
{
  Clear();
}

void CollisionManager::AddObject(const std::string &texturePath, float x, float y, float width, float height, CollisionType type)
{
  auto obj = std::make_unique<Collision>(texturePath, x, y, width, height, type);
  objects.push_back(std::move(obj));
}

void CollisionManager::AddBlockingObject(const std::string &texturePath, float x, float width, float height)
{
  float y = groundY - height;
  AddObject(texturePath, x, y, width, height, CollisionType::BLOCKING);
}

void CollisionManager::AddWall(const std::string &texturePath, float x, float width, float height)
{
  float y = groundY - height;
  AddObject(texturePath, x, y, width, height, CollisionType::WALL);
}

void CollisionManager::AddPlatform(const std::string &texturePath, float x, float y, float width, float height)
{
  AddObject(texturePath, x, y, width, height, CollisionType::PLATFORM);
}

void CollisionManager::Update(float cameraX)
{
  for (auto &obj : objects)
  {
    obj->Update(cameraX); // Use -> for unique_ptr
  }
}

void CollisionManager::Draw()
{
  for (auto &obj : objects)
  {
    if (obj->GetIsActive())
    {
      obj->Draw();
    }
  }
}

void CollisionManager::Clear()
{
  objects.clear();
}

std::vector<Collision *> CollisionManager::CheckCollision(const Rectangle &playerBounds)
{
  std::vector<Collision *> collisions;

  for (auto &obj : objects)
  {
    if (obj->GetIsActive() && CheckCollisionRecs(playerBounds, obj->GetBoundBox()))
    {
      collisions.push_back(obj.get()); // fixed: was 'collision.push_back(...)'
    }
  }
  return collisions;
}

Collision *CollisionManager::CheckHorizontalCollision(const Rectangle &playerBounds, float previousX)
{
  for (auto &obj : objects)
  {
    if (!obj->GetIsActive())
      continue;

    CollisionType type = obj->GetCollision(); // fixed: was GetCollision()
    if (type != CollisionType::BLOCKING && type != CollisionType::WALL)
      continue;

    Rectangle objBounds = obj->GetBoundBox();

    if (CheckCollisionRecs(playerBounds, objBounds))
    {
      float playerBottom = playerBounds.y + playerBounds.height;
      float objTop = objBounds.y;

      if (playerBottom > objTop + 10)
      {
        return obj.get();
      }
    }
  }

  return nullptr;
}

Collision *CollisionManager::CheckVerticalCollision(const Rectangle &playerBounds)
{
  for (auto &obj : objects)
  {
    if (!obj->GetIsActive())
      continue;

    Rectangle objBounds = obj->GetBoundBox();

    if (CheckCollisionRecs(playerBounds, objBounds))
    {
      float playerBottom = playerBounds.y + playerBounds.height;
      float objTop = objBounds.y;

      if (playerBottom >= objTop - 10 && playerBottom <= objTop + 30)
      {
        return obj.get();
      }
    }
  }

  return nullptr;
}

void CollisionManager::CreateTestLevel()
{
  AddBlockingObject("", 400.0f, 120.0f, 48.0f); // Car 1
  AddBlockingObject("", 600.0f, 108.0f, 52.0f); // Car 2
  AddBlockingObject("", 850.0f, 64.0f, 32.0f);  // Barrier
  AddWall("", 1200.0f, 32.0f, 128.0f);          // Wall
  AddPlatform("", 0.0f, groundY - 32.0f, 2000.0f, 32.0f);
}

void CollisionManager::LoadFromFile(const std::string &filename)
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    TraceLog(LOG_ERROR, "Cannot open collision file: %s", filename.c_str());
    return;
  }

  std::string line;
  while (std::getline(file, line))
  {
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string type, texture;
    float x, y, width, height;

    if (iss >> type >> texture >> x >> y >> width >> height)
    {
      if (type == "BLOCKING")
      {
        AddBlockingObject(texture, x, width, height);
      }
      else if (type == "WALL")
      {
        AddWall(texture, x, width, height);
      }
      else if (type == "PLATFORM")
      {
        AddPlatform(texture, x, y, width, height);
      }
    }
  }

  file.close();
}
