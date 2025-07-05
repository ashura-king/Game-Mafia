#include "Platform/ObjectSpawner.hpp"
#include <fstream>
#include <sstream>
#include <raylib.h>

ObjectSpawner::ObjectSpawner(GameObject *manage)
    : objectManager(manage), lastSpawnX(0.0f) {}

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
  // Crate
  spawnData.push_back({"resource/Object/Crate.png", 500.0f, 400.0f, 1.0f, ObjectType::CRATE});
  spawnData.push_back({"resource/Object/Crate.png", 800.0f, 350.0f, 1.2f, ObjectType::CRATE});
  spawnData.push_back({"resource/Object/Crate.png", 1200.0f, 380.0f, 0.8f, ObjectType::CRATE});

  // Barrel
  spawnData.push_back({"resource/Object/Barrel.png", 650.0f, 420.0f, 1.0f, ObjectType::BARREL});
  spawnData.push_back({"resource/Object/Barrel.png", 1100.0f, 400.0f, 1.1f, ObjectType::BARREL});

  // Old Car
  spawnData.push_back({"resource/Object/oldcar.png", 750.0f, 300.0f, 0.5f, ObjectType::OLDCAR});
  spawnData.push_back({"resource/Object/oldcar.png", 950.0f, 320.0f, 0.5f, ObjectType::OLDCAR});
}

void ObjectSpawner::SpawnObjectInRange(float cameraX, float spawnRange)
{
  for (const auto &spawn : spawnData)
  {
    // Check if object is within spawn range and hasn't been spawned yet
    if (spawn.x > cameraX - 100 && spawn.x < cameraX + spawnRange && spawn.x > lastSpawnX)

    {
      objectManager->AddObject(spawn.texturePath.c_str(), spawn.x, spawn.y, spawn.scale, spawn.type);
    }
  }

  lastSpawnX = cameraX + spawnRange;
}

void ObjectSpawner::SpawnCrate(float x, float y, float scale)
{
  objectManager->AddObject("resource/texture/crate.png", x, y, scale, ObjectType::CRATE);
}

void ObjectSpawner::SpawnBarrel(float x, float y, float scale)
{
  objectManager->AddObject("resource/texture/barrel.png", x, y, scale, ObjectType::BARREL);
}

void ObjectSpawner::SpawnOldCar(float x, float y, float scale)
{
  objectManager->AddObject("resource/texture/oldcar.png", x, y, scale, ObjectType::OLDCAR);
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

  // Expected format: "texture_path x y scale type"
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
