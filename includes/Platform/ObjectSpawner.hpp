#pragma once
#include "GameObject.hpp"
#include <vector>
#include <string>

// Struct for loading object spawn data
struct ObjectSpawn
{
  std::string texturePath;
  float x, y;
  float scale;
  ObjectType type;
};

class ObjectSpawner
{
public:
  ObjectSpawner(GameObject *manager);

  void LoadObject(const std::string &filename); // Load from specific file
  void LoadObjectFile();                        // Default behavior
  void Reset();
  void SpawnObjectInRange(float cameraX, float spawnRange = 1000.0f + 200.0f);

  void SpawnCrate(float x, float y, float scale = 1.0f);
  void SpawnBarrel(float x, float y, float scale = 1.0f);
  void SpawnOldCar(float x, float y, float scale = 1.0f);
  void SpawnObstacle(const std::string &texturePath, float x, float y, float scale = 1.0f);

private:
  GameObject *objectManager;
  float lastSpawnX;
  std::vector<ObjectSpawn> spawnData;

  void ParseSpawnLine(const std::string &line);
};
