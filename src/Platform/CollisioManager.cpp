// Fixed CollisionManager.cpp for Flat Horizontal Run-and-Gun Game
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
    obj->Update(cameraX);
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
      collisions.push_back(obj.get());
    }
  }
  return collisions;
}

// For horizontal run-and-gun: Only block movement when player is on ground level
Collision *CollisionManager::CheckHorizontalCollision(const Rectangle &playerBounds, float previousX)
{
  for (auto &obj : objects)
  {
    if (!obj->GetIsActive())
      continue;

    CollisionType type = obj->GetCollision();
    if (type != CollisionType::BLOCKING && type != CollisionType::WALL)
      continue;

    Rectangle objBounds = obj->GetBoundBox();

    if (CheckCollisionRecs(playerBounds, objBounds))
    {
      float playerBottom = playerBounds.y + playerBounds.height;
      float playerTop = playerBounds.y;
      float objTop = objBounds.y;
      float objBottom = objBounds.y + objBounds.height;

      // Only block horizontal movement if player is at ground level
      // Allow jumping over obstacles
      if (playerBottom > objTop + 10.0f && playerTop < objBottom - 10.0f)
      {
        return obj.get();
      }
    }
  }

  return nullptr;
}

// Simple ground check - player should always return to ground level
Collision *CollisionManager::CheckVerticalCollision(const Rectangle &playerBounds)
{
  // For flat run-and-gun, always return to ground level
  float playerBottom = playerBounds.y + playerBounds.height;

  // Check if player should land on an obstacle while jumping
  for (auto &obj : objects)
  {
    if (!obj->GetIsActive())
      continue;

    Rectangle objBounds = obj->GetBoundBox();

    float playerLeft = playerBounds.x;
    float playerRight = playerBounds.x + playerBounds.width;
    float objLeft = objBounds.x;
    float objRight = objBounds.x + objBounds.width;
    float objTop = objBounds.y;

    // Check if player is above obstacle and falling
    bool horizontalOverlap = (playerRight > objLeft + 5.0f) && (playerLeft < objRight - 5.0f);

    if (horizontalOverlap && playerBottom >= objTop && playerBottom <= objTop + 15.0f)
    {
      return obj.get();
    }
  }

  return nullptr;
}

// Check if player is on ground level
bool CollisionManager::IsPlayerOnGroundLevel(const Rectangle &playerBounds)
{
  float playerBottom = playerBounds.y + playerBounds.height;
  return (playerBottom >= groundY - 5.0f);
}

void CollisionManager::CreateTestLevel()
{
  // Create obstacles that can be jumped over
  AddBlockingObject("", 400.0f, 120.0f, 48.0f); // Car 1 - can jump over
  AddBlockingObject("", 600.0f, 108.0f, 52.0f); // Car 2 - can jump over
  AddBlockingObject("", 850.0f, 64.0f, 32.0f);  // Barrier - can jump over
  AddWall("", 1200.0f, 32.0f, 128.0f);          // Wall - needs higher jump

  // Ground platform (invisible ground collision)
  AddPlatform("", 0.0f, groundY, 2000.0f, 32.0f);
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