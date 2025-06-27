#pragma once

#include "raylib.h"
#include "raymath.h"
#include "includes/GameType.hpp"
#include <vector>

class Bot
{
public:
  Bot(float spawnX, float spawnY, BotType botType = BotType::STREET_THUG);
  virtual ~Bot();

  // Core lifecycle methods
  virtual void LoadTextures();
  virtual void Update(float deltaTime);
  virtual void Draw();

  // Direct combat AI behaviors
  virtual void UpdateAI(Vector2 playerPos, float deltaTime);
  virtual void SpawnBehavior(float deltaTime);
  virtual void DirectCombatBehavior(Vector2 playerPos, float deltaTime);
  virtual void RangedCombatBehavior(Vector2 playerPos, float deltaTime);
  virtual void IdleBehavior(float deltaTime);

  // Direct movement patterns
  virtual void RunTowardPlayer(Vector2 playerPos);
  virtual void StopAndAttack(Vector2 playerPos);
  virtual void PaceAround();
  virtual void CallForGang();

  // Combat system
  virtual void Punch(Vector2 targetPos);
  virtual void Kick(Vector2 targetPos);
  virtual void Grab(Vector2 targetPos);
  virtual void Shoot(Vector2 targetPos);       // firearms
  virtual void ThrowWeapon(Vector2 targetPos); // bottles, pipes, etc.
  virtual void Block();
  virtual void TakeDamage(int damage);
  virtual void GetKnockedDown();
  virtual void Die();

  // Getters
  BotType GetBotType() const { return type; }
  BotState GetState() const { return state; }
  Vector2 GetPosition() const { return {x, y}; }
  bool IsAlive() const { return health > 0 && !isKnockedOut; }
  bool IsOnScreen() const;
  bool IsReadyToFight() const { return isOnScreen && state != BotState::SPAWNING; }
  bool CanAttack() const { return attackCooldown <= 0 && !isStunned; }
  bool IsInAttackRange() const;
  bool IsInShootRange() const;

  // Public properties for game logic
  float x, y;
  float width, height;
  int health, maxHealth;
  float speed;
  Direction facing;
  bool isStunned;
  bool isKnockedOut;

protected:
  // Bot identity and type
  BotType type;
  BotState state;
  float stateTimer;

  // Spawn system
  Vector2 spawnPoint;
  Vector2 targetPosition;
  float spawnTimer;
  bool isOnScreen;
  bool hasEnteredCombat;

  // Direct combat AI parameters
  float detectionRange; // How far they can see player
  float attackRange;    // Melee range
  float shootRange;     // Ranged weapon range
  float alertTime;
  float attackCooldown;
  float attackTimer;
  float stunTimer;
  float knockdownTimer;
  float idleTimer; // For pacing behavior

  // Movement and animation
  float frameWidth, frameHeight;
  float animTimer;
  int currentFrame;
  int maxFrames;

  // Textures
  Texture2D idleTexture;
  Texture2D walkTexture;
  Texture2D runTexture;
  Texture2D punchTexture;
  Texture2D kickTexture;
  Texture2D grabTexture;
  Texture2D shootTexture;
  Texture2D throwTexture;
  Texture2D blockTexture;
  Texture2D hurtTexture;
  Texture2D knockdownTexture;
  Texture2D deathTexture;

  // Animation data
  Animation idleAnim;
  Animation walkAnim;
  Animation runAnim;
  Animation punchAnim;
  Animation kickAnim;
  Animation grabAnim;
  Animation shootAnim;
  Animation throwAnim;
  Animation blockAnim;
  Animation hurtAnim;
  Animation knockdownAnim;
  Animation deathAnim;

  // Combat behavior
  bool isAggressive;
  bool playerSpotted;
  Vector2 lastKnownPlayerPos;
  float aggroLevel;
  int comboCount;
  float comboTimer;

  // Combat stats
  int punchDamage;
  int kickDamage;
  int grabDamage;
  int shootDamage;
  int throwDamage;
  float blockChance;
  float counterAttackChance;

  // Pacing behavior
  Vector2 paceStartPos;
  Vector2 paceEndPos;
  bool pacingRight;
  float paceDistance;

  // Combat constants
  static constexpr float SPAWN_MARGIN = 120.0f;
  static constexpr float RUN_SPEED = 120.0f;
  static constexpr float WALK_SPEED = 60.0f;
  static constexpr float DETECTION_RANGE = 300.0f;
  static constexpr float ATTACK_RANGE = 45.0f;
  static constexpr float SHOOT_RANGE = 250.0f;
  static constexpr float PACE_DISTANCE = 80.0f;

private:
  // Helper methods
  void InitializeByType();
  void UpdateAnimation(float deltaTime);
  void CheckScreenBounds();
  Vector2 GetRandomSpawnPoint();
  bool CanSeePlayer(Vector2 playerPos);
  void SetStateWithTimer(BotState newState, float duration = 0.0f);
  float GetDistanceToPlayer(Vector2 playerPos);

  // Direct combat behaviors
  void ExecuteDirectAttack(Vector2 playerPos, float deltaTime);
  void ExecuteRangedAttack(Vector2 playerPos, float deltaTime);
  void ExecuteMovement(Vector2 playerPos, float deltaTime);
  void ExecutePacing(float deltaTime);
  void ExecuteStunned(float deltaTime);
  void ExecuteKnockdown(float deltaTime);

  // Combat logic
  void ChooseDirectAttack(Vector2 playerPos);
  void ChooseRangedAttack(Vector2 playerPos);
  void AttemptBlock();
  void AttemptCounterAttack(Vector2 playerPos);
  void SetupPacingArea();

  // Audio and effects
  void PlaySpawnSound();
  void PlayPunchSound();
  void PlayKickSound();
  void PlayShootSound();
  void PlayThrowSound();
  void PlayHurtSound();
  void PlayKnockdownSound();
  void CreateHitEffect();
  void CreateMuzzleFlash();
};

// Specialized bot types with direct combat focus
class StreetThugBot : public Bot
{
public:
  StreetThugBot(float spawnX, float spawnY);
  void UpdateAI(Vector2 playerPos, float deltaTime) override;
  void LoadTextures() override;
  // Direct melee rushes, basic attacks
};

class ShooterBot : public Bot
{
public:
  ShooterBot(float spawnX, float spawnY);
  void UpdateAI(Vector2 playerPos, float deltaTime) override;
  void LoadTextures() override;
  void Shoot(Vector2 targetPos) override;
  // Stops and shoots, moves to new positions
};

class BrawlerBot : public Bot
{
public:
  BrawlerBot(float spawnX, float spawnY);
  void UpdateAI(Vector2 playerPos, float deltaTime) override;
  void LoadTextures() override;
  void Grab(Vector2 targetPos) override;
  // Rushes in for grappling attacks
};

class HeavyBot : public Bot
{
public:
  HeavyBot(float spawnX, float spawnY);
  void UpdateAI(Vector2 playerPos, float deltaTime) override;
  void LoadTextures() override;
  void Punch(Vector2 targetPos) override;
  // Slow approach, devastating attacks
};

class ThrowerBot : public Bot
{
public:
  ThrowerBot(float spawnX, float spawnY);
  void UpdateAI(Vector2 playerPos, float deltaTime) override;
  void LoadTextures() override;
  void ThrowWeapon(Vector2 targetPos) override;
  // Throws street objects like bottles, rocks, pipes
};

class RusherBot : public Bot
{
public:
  RusherBot(float spawnX, float spawnY);
  void UpdateAI(Vector2 playerPos, float deltaTime) override;
  void LoadTextures() override;
  void RunTowardPlayer(Vector2 playerPos) override;
  // Fast direct attacks, hit-and-run
};

// Bot spawner for managing encounters
class BotSpawner
{
public:
  BotSpawner();
  ~BotSpawner();

  void Update(float deltaTime, Vector2 playerPos);
  void SpawnBot(BotType type);
  void SpawnSquad(int count, BotType primaryType = BotType::STREET_THUG);
  void SpawnMixedEncounter();
  void ClearAllBots();

  std::vector<Bot *> &GetBots() { return activeBots; }

  // Spawn configuration
  void SetEncounterRate(float rate) { encounterRate = rate; }
  void SetMaxBots(int max) { maxActiveBots = max; }
  void SetAggressionLevel(float level) { aggressionLevel = level; }
  void EnableRandomEncounters(bool enable) { randomEncounters = enable; }

private:
  std::vector<Bot *> activeBots;
  float encounterTimer;
  float encounterRate;
  int maxActiveBots;
  float aggressionLevel;
  bool randomEncounters;

  Vector2 GetSpawnPosition();
  void CleanupDefeatedBots();
  BotType GetRandomBotType();
};