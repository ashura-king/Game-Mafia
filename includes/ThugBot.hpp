#pragma once
#include "Bot.hpp"

class StreetThugBot : public Bot
{
public:
  // Constructor - calls base Bot constructor
  StreetThugBot(float spawnX, float spawnY);

  // Destructor
  ~StreetThugBot();

  // OVERRIDE base class virtual methods
  void UpdateAI(Vector2 playerPos, float deltaTime) override;
  void LoadTextures() override;

  // StreetThug-specific methods
  void BerserkMode();
  void CallForBackup();

private:
  // StreetThug-specific properties
  bool isBerserk;
  float berserkTimer;
  int backupCallCount;
};