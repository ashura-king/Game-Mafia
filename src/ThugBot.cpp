

// ThugBot.cpp
#include "includes/ThugBot.hpp"
#include "raylib.h"

ThugBot::ThugBot(float startX, float startY) : Bot(startX, startY)
{
  LoadTextures();
  SetProperties();
  isLoaded = true;
}

void ThugBot::LoadTextures()
{
  // Load thug-specific sprites
  idleTexture = LoadTexture("resource/thug/thugIdle.png");
  idleLeftTexture = LoadTexture("resource/thug/thugIdle.png");
  walkTexture = LoadTexture("resource/thug/thugWalk.png");
  runTexture = LoadTexture("resource/thug/thugRun.png");
  attackTexture = LoadTexture("resource/thug/thugAttack.png");

  // Validate textures
  if (idleTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load thug idle texture");
  if (walkTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load thug walk texture");
  if (runTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load thug run texture");
  if (attackTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load thug attack texture");
}

void ThugBot::SetProperties()
{
  // Initialize animations
  idleRightAnim = {0, 7, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  idleLeftAnim = {0, 7, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  walkAnim = {0, 9, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  runAnim = {0, 9, 0, 0.1f, 0.1f, 1, AnimationType::REPEATING};
  attackAnim = {0, 5, 0, 0.1f, 0.1f, 1, AnimationType::ONESHOT};

  // Thug-specific properties - fast and aggressive
  speed = 120.0f;
  health = 100;
  maxHealth = 100;
  attackRange = 100.0f;
  chaseRange = 300.0f;
  fleeingRange = 200.0f;
  attackCooldown = 0.6f;
  wanderTime = 4.0f;
  spawnDelay = 15.0f;

  // Transform
  width = 256.0f;
  height = 256.0f;
  direction = Direction::RIGHT;

  // State
  state = BotState::IDLE;
  previousState = BotState::IDLE;
  stateTimer = 0.0f;

  // Initialize other variables
  wanderTarget = {0.0f, 0.0f};
  wanderTimer = 0.0f;
  currentWaypointIndex = 0;
  waypointReachDistance = 50.0f;
  isAttacking = false;
  attackTimer = 0.0f;
  spawnTimer = 0.0f;
  isSpawned = false;
  lastValidDirection = {1.0f, 0.0f};
  directionChangeTimer = 0.0f;
}

void ThugBot::UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  // Don't update AI if not spawned yet or not alive
  if (!isSpawned || !IsAlive())
    return;

  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  stateTimer += deltaTime;

  // Thugs are more aggressive - they chase from farther away
  float aggressiveChaseRange = chaseRange * THUG_AGGRESSION_MULTIPLIER;

  // Priority 1: Attack if in range
  if (distanceToPlayer < attackRange && CanAttack())
  {
    SetState(BotState::ATTACK);
    Attack();
  }
  // Priority 2: Aggressive chasing
  else if (distanceToPlayer < aggressiveChaseRange && distanceToPlayer > attackRange)
  {
    SetState(BotState::CHASING);
    ChasePlayer(playerPos, otherBots);
  }
  // Priority 3: Thugs don't flee easily - only when very low health
  else if (distanceToPlayer < fleeingRange && health < maxHealth * 0.15f)
  {
    SetState(BotState::FLEEING);
    MoveAway(playerPos);
  }
  // Priority 4: Short wandering periods - thugs are restless
  else if (state == BotState::IDLE || state == BotState::WANDERING)
  {
    if (state == BotState::IDLE && stateTimer >= wanderTime * 0.8f) // Shorter idle time
    {
      SetState(BotState::WANDERING);
      wanderTimer = 0.0f;
    }

    if (state == BotState::WANDERING)
    {
      Wander(deltaTime, otherBots);

      // Shorter wander time - thugs are always looking for trouble
      if (stateTimer >= wanderTime * 1.5f)
      {
        SetState(BotState::IDLE);
      }
    }
  }
  else
  {
    SetState(BotState::IDLE);
  }
}

void ThugBot::Attack()
{
  if (CanAttack())
  {
    isAttacking = true;
    attackTimer = attackCooldown;
    attackAnim.curr = attackAnim.first;
    attackAnim.duration_left = attackAnim.speed;

    // Thugs do slightly more damage or have special attack effects
    TraceLog(LOG_INFO, "Thug bot attacking!");
  }
}