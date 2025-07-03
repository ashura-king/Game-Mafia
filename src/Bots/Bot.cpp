#include "Bots/Bot.hpp"
#include <raylib.h>
#include <cmath>

Bot::Bot(float spawnX, float spawnY, BotType botType)
    : x(spawnX), y(spawnY),
      width(0), height(0),
      health(100), maxHealth(100),
      facing(Direction::RIGHT),
      isStunned(false), isKnockedOut(false),

      type(botType),
      state(BotState::SPAWNING),
      stateTimer(0.0f),

      speed(WALK_SPEED),
      detectionRange(DETECTION_RANGE),
      attackRange(ATTACK_RANGE),
      shootRange(SHOOT_RANGE),
      alertTime(0.0f),
      attackCooldown(0.0f),
      attackTimer(0.0f),
      stunTimer(0.0f),
      knockdownTimer(0.0f),
      idleTimer(0.0f),
      spawnTimer(2.0f),

      isAggressive(false),
      playerSpotted(false),
      lastKnownPlayerPos({0.0f, 0.0f}),
      aggroLevel(0.0f),
      comboCount(0),
      comboTimer(0.0f),

      frameWidth(64), frameHeight(64),
      animTimer(0.0f),
      currentFrame(0),
      maxFrames(1),
      spawnPoint({spawnX, spawnY}),
      targetPosition({spawnX, spawnY}),
      isOnScreen(false),
      hasEnteredCombat(false),

      paceStartPos({0.0f, 0.0f}),
      paceEndPos({0.0f, 0.0f}),
      pacingRight(true),
      paceDistance(PACE_DISTANCE),

      punchDamage(0), kickDamage(0), grabDamage(0),
      shootDamage(0), throwDamage(0),
      blockChance(0.0f), counterAttackChance(0.0f),

      idleTexture({0}), walkTexture({0}), runTexture({0}),
      punchTexture({0}), kickTexture({0}), grabTexture({0}),
      smashTexture({0}), shootTexture({0}), throwTexture({0}),
      blockTexture({0}), hurtTexture({0}),
      knockdownTexture({0}), deathTexture({0}),

      idleAnim(), walkAnim(), runAnim(),
      punchAnim(), kickAnim(), grabAnim(), smashAnim(),
      shootAnim(), throwAnim(), blockAnim(),
      hurtAnim(), knockdownAnim(), deathAnim()
{
  InitializeByType();
  SetupPacingArea();
}

Bot::~Bot()
{
  UnloadTexture(idleTexture);
  UnloadTexture(walkTexture);
  UnloadTexture(runTexture);
  UnloadTexture(punchTexture);
  UnloadTexture(kickTexture);
  UnloadTexture(grabTexture);
  UnloadTexture(smashTexture);
  UnloadTexture(shootTexture);
  UnloadTexture(throwTexture);
  UnloadTexture(blockTexture);
  UnloadTexture(hurtTexture);
  UnloadTexture(knockdownTexture);
  UnloadTexture(deathTexture);
}

void Bot::LoadTextures() {}
void Bot::Update(float deltaTime) {}
void Bot::Draw() {}
void Bot::UpdateAI(Vector2 playerPos, float deltaTime) {}
void Bot::SpawnBehavior(float deltaTime) {}
void Bot::DirectCombatBehavior(Vector2 playerPos, float deltaTime) {}
void Bot::RangedCombatBehavior(Vector2 playerPos, float deltaTime) {}
void Bot::IdleBehavior(float deltaTime) {}

void Bot::RunTowardPlayer(Vector2 playerPos) {}
void Bot::StopAndAttack(Vector2 playerPos) {}
void Bot::PaceAround() {}
void Bot::CallForGang() {}

void Bot::Punch(Vector2 targetPos) {}
void Bot::Kick(Vector2 targetPos) {}
void Bot::Smash(Vector2 targetPos) {}
void Bot::Shoot(Vector2 targetPos) {}
void Bot::ThrowWeapon(Vector2 targetPos) {}
bool Bot::Block() { return false; }
void Bot::TakeDamage(int damage) {}
void Bot::GetKnockedDown() {}
void Bot::Die() {}

bool Bot::IsOnScreen() const { return isOnScreen; }
bool Bot::IsInAttackRange() const { return false; }
bool Bot::IsInShootRange() const { return false; }

float Bot::GetDistanceToPlayer(Vector2 playerPos) { return Vector2Distance({x, y}, playerPos); }
void Bot::ChooseDirectAttack(Vector2 playerPos) {}
void Bot::ChooseRangedAttack(Vector2 playerPos) {}

void Bot::InitializeByType() {}
void Bot::UpdateAnimation(float deltaTime) {}
void Bot::CheckScreenBounds() {}
Vector2 Bot::GetRandomSpawnPoint() { return {x, y}; }
bool Bot::CanSeePlayer(Vector2 playerPos) { return true; }
void Bot::SetStateWithTimer(BotState newState, float duration) {}

void Bot::ExecuteDirectAttack(Vector2 playerPos, float deltaTime) {}
void Bot::ExecuteRangedAttack(Vector2 playerPos, float deltaTime) {}
void Bot::ExecuteMovement(Vector2 playerPos, float deltaTime) {}
void Bot::ExecutePacing(float deltaTime) {}
void Bot::ExecuteStunned(float deltaTime) {}
void Bot::ExecuteKnockdown(float deltaTime) {}
void Bot::AttemptBlock() {}
void Bot::AttemptCounterAttack(Vector2 playerPos) {}
void Bot::SetupPacingArea() {}

void Bot::PlaySpawnSound() {}
void Bot::PlayPunchSound() {}
void Bot::PlayKickSound() {}
void Bot::PlayShootSound() {}
void Bot::PlayThrowSound() {}
void Bot::PlayHurtSound() {}
void Bot::PlayKnockdownSound() {}
void Bot::CreateHitEffect() {}
void Bot::CreateMuzzleFlash() {}
