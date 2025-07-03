#include "Bots/StreetThugBot.hpp"
#include "raymath.h"

StreetThugBot::StreetThugBot(float spawnX, float spawnY)
    : Bot(spawnX, spawnY, BotType::STREET_THUG),
      callsForBackup(true),
      backupCallCooldown(10.0f),
      backupCallTimer(0.0f),
      aggressionBoost(1.25f),
      alwaysAdvances(true),
      retreatThreshold(25.0f),
      comboChance(0.4f),
      canDoCombo(false),
      maxComboHits(3)
{
  LoadTextures(); // Load specific textures for this bot
                  // Optional: inherited pacing setup
}

StreetThugBot::~StreetThugBot()
{
  // If you dynamically allocate textures or other memory, unload them here.
  // Example: UnloadTexture(idleTexture); (only if you're managing them manually)
}

void StreetThugBot::LoadTextures()
{
  /* idleTexture = LoadTexture("assets/streetthug/idle.png");
   walkTexture = LoadTexture("assets/streetthug/walk.png");
   runTexture = LoadTexture("assets/streetthug/run.png");

   punchTexture = LoadTexture("assets/streetthug/punch.png");
   kickTexture = LoadTexture("assets/streetthug/kick.png");
   hurtTexture = LoadTexture("assets/streetthug/hurt.png");
   deathTexture = LoadTexture("assets/streetthug/death.png");

   // Assign default animation frames per texture if needed
   idleAnim = {0, 4, 0, 0.2f, 0.0f, 0, AnimationType::REPEATING};
   walkAnim = {0, 6, 0, 0.15f, 0.0f, 0, AnimationType::REPEATING};
   runAnim = {0, 8, 0, 0.1f, 0.0f, 0, AnimationType::REPEATING};
   smashAnim = {0, 8, 0, 0.1f, 0.0f, 0, AnimationType::REPEATING};
   hurtAnim = {0, 3, 0, 0.1f, 0.0f, 0, AnimationType::ONESHOT};
   knockdownAnim = {0, 6, 0, 0.15f, 0.0f, 0, AnimationType::ONESHOT};
   deathAnim = {0, 8, 0, 0.12f, 0.0f, 0, AnimationType::ONESHOT};
   */
}

void StreetThugBot::UpdateAI(Vector2 playerPos, float deltaTime)
{
  stateTimer += deltaTime;

  if (state == BotState::SPAWNING)
  {
    SpawnBehavior(deltaTime);
    return;
  }

  if (!IsAlive())
  {
    Die();
    return;
  }
}