#include "Platform/ObjectSpawner.hpp"
#include <fstream>
#include <sstream>
#include <raylib.h>
#include <unordered_set>

ObjectSpawner::ObjectSpawner(GameObject *manager)
    : objectManager(manager), lastSpawnX(0.0f) {}

void ObjectSpawner::LoadObject(const std::string &filename)
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    TraceLog(LOG_WARNING, "Could not open object spawn file: %s", filename.c_str());
    return;
  }
  std::string line;
  while (std::getline(file, line))
  {
    if (line.empty() || line[0] == '#')
      continue;
    ParseSpawnLine(line);
  }
  file.close();
  TraceLog(LOG_INFO, "Loaded %zu object spawns from %s", spawnData.size(), filename.c_str());
}

void ObjectSpawner::LoadObjectFile()
{
  spawnData.clear();
  // Pre-defined object positions - these will be spawned when camera reaches them
  float groundY = 420.0f;

  spawnData.push_back({"resource/Object/Crate.png", 500.0f, groundY, 1.0f, ObjectType::CRATE});
  spawnData.push_back({"resource/Object/Crate.png", 800.0f, groundY, 1.2f, ObjectType::CRATE});
  spawnData.push_back({"resource/Object/Crate.png", 1200.0f, groundY, 0.8f, ObjectType::CRATE});

  spawnData.push_back({"resource/Object/Barrel.png", 650.0f, groundY, 1.0f, ObjectType::BARREL});
  spawnData.push_back({"resource/Object/Barrel.png", 1100.0f, groundY, 1.1f, ObjectType::BARREL});

  spawnData.push_back({"resource/Object/oldcar.png", 750.0f, groundY, 1.0f, ObjectType::OLDCAR});
  spawnData.push_back({"resource/Object/oldcar.png", 950.0f, groundY, 1.0f, ObjectType::OLDCAR});
}

void ObjectSpawner::SpawnObjectInRange(float cameraX, float spawnRange)
{
  // Calculate spawn boundaries based on camera position
  float leftBound = cameraX - spawnRange * 0.5f; // Spawn some objects behind camera
  float rightBound = cameraX + spawnRange;       // Spawn ahead of camera

  // Track which objects we've already spawned to avoid duplicates
  static std::unordered_set<int> spawnedObjects;

  // First, spawn pre-defined objects from spawnData
  for (size_t i = 0; i < spawnData.size(); ++i)
  {
    const auto &spawn = spawnData[i];

    // Check if this object is within spawn range and not already spawned
    if (spawn.x >= leftBound && spawn.x <= rightBound)
    {
      if (spawnedObjects.find(i) == spawnedObjects.end())
      {
        // Spawn the object at its world position (not relative to camera)
        objectManager->AddObject(spawn.texturePath.c_str(),
                                 spawn.x, spawn.y, spawn.scale, spawn.type);
        spawnedObjects.insert(i);
        TraceLog(LOG_INFO, "Spawned object at world position: %.2f, %.2f", spawn.x, spawn.y);
      }
    }
  }

  // Then spawn procedural objects in a pattern
  float gap = 150.0f; // Increased gap between objects
  float startX = std::max(leftBound, lastSpawnX);

  for (float x = startX; x <= rightBound; x += gap)
  {
    if (x > lastSpawnX + gap) // Only spawn if we haven't spawned here before
    {
      // Skip if too close to pre-defined objects
      bool tooClose = false;
      for (const auto &spawn : spawnData)
      {
        if (abs(x - spawn.x) < 100.0f) // 100 pixel buffer around pre-defined objects
        {
          tooClose = true;
          break;
        }
      }

      if (!tooClose)
      {
        int pattern = static_cast<int>(x / gap) % 3;
        std::string texture;
        ObjectType type;
        float yPos = 420.0f;

        switch (pattern)
        {
        case 0:
          texture = "resource/Object/Crate.png";
          type = ObjectType::CRATE;
          break;
        case 1:
          texture = "resource/Object/Barrel.png";
          type = ObjectType::BARREL;
          break;
        case 2:
          texture = "resource/Object/oldcar.png";
          type = ObjectType::OLDCAR;
          break;
        default:
          texture = "resource/Object/Crate.png";
          type = ObjectType::CRATE;
          break;
        }

        // Spawn object at world position
        objectManager->AddObject(texture.c_str(), x, yPos, 1.0f, type);
        TraceLog(LOG_INFO, "Spawned procedural object at world position: %.2f, %.2f", x, yPos);
      }
    }
  }

  lastSpawnX = rightBound;
}

void ObjectSpawner::SpawnCrate(float x, float y, float scale)
{
  objectManager->AddObject("resource/Object/Crate.png", x, y, scale, ObjectType::CRATE);
}

void ObjectSpawner::SpawnBarrel(float x, float y, float scale)
{
  objectManager->AddObject("resource/Object/Barrel.png", x, y, scale, ObjectType::BARREL);
}

void ObjectSpawner::SpawnOldCar(float x, float y, float scale)
{
  objectManager->AddObject("resource/Object/oldcar.png", x, y, scale, ObjectType::OLDCAR);
}

void ObjectSpawner::SpawnObstacle(const std::string &texturePath, float x, float y, float scale)
{
  objectManager->AddObject(texturePath.c_str(), x, y, scale, ObjectType::OBSTACLE);
}

void ObjectSpawner::ParseSpawnLine(const std::string &line)
{
  std::istringstream iss(line);
  std::string texturePath, typeStr;
  float x, y, scale;

  if (iss >> texturePath >> x >> y >> scale >> typeStr)
  {
    ObjectType type = ObjectType::OBSTACLE;

    if (typeStr == "CRATE")
      type = ObjectType::CRATE;
    else if (typeStr == "BARREL")
      type = ObjectType::BARREL;
    else if (typeStr == "OLDCAR")
      type = ObjectType::OLDCAR;

    spawnData.push_back({texturePath, x, y, scale, type});
  }
}

// Add method to reset spawner state (call when restarting game)
void ObjectSpawner::Reset()
{
  lastSpawnX = 0.0f;
  // Clear the static spawned objects set
  // Note: You might want to make spawnedObjects a member variable instead of static
}