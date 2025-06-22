#include "includes/Swat.hpp"
#include "raylib.h"
#include <algorithm>

SwatBot::SwatBot(float startX, float startY) : Bot(startX, startY)
{
  LoadTextures();
  SetProperties();
  isLoaded = true;
}

void SwatBot::LoadTextures()
{
  // Load SWAT/Police-specific sprites
  idleTexture = LoadTexture("resource/police/policeIdle.png");
  idleLeftTexture = LoadTexture("resource/police/policeIdle.png");
  walkTexture = LoadTexture("resource/police/policeWalk.png");
  runTexture = LoadTexture("resource/police/policeRun.png");
  attackTexture = LoadTexture("resource/police/policeAttack.png");

  // Validate textures
  if (idleTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load SWAT idle texture");
  if (walkTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load SWAT walk texture");
  if (runTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load SWAT run texture");
  if (attackTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load SWAT attack texture");
}

void SwatBot::SetProperties()
{
  // Initialize animations
  idleRightAnim = {0, 7, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  idleLeftAnim = {0, 7, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  walkAnim = {0, 9, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  runAnim = {0, 9, 0, 0.1f, 0.1f, 1, AnimationType::REPEATING};
  attackAnim = {0, 5, 0, 0.1f, 0.1f, 1, AnimationType::ONESHOT};

  // SWAT properties - balanced and disciplined
  speed = 100.0f;
  health = 120;
  maxHealth = 120;
  attackRange = 130.0f;
  chaseRange = 400.0f;
  fleeingRange = 300.0f;
  attackCooldown = 0.5f;
  wanderTime = 6.0f;
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

void SwatBot::UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  // Don't update AI if not spawned yet or not alive
  if (!isSpawned || !IsAlive())
    return;

  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  stateTimer += deltaTime;

  // SWAT coordination bonus when near other SWAT
  float effectiveChaseRange = chaseRange;
  float effectiveAttackRange = attackRange;

  if (IsNearOtherSwat(otherBots))
  {
    effectiveChaseRange *= SWAT_COORDINATION_BONUS;
    effectiveAttackRange *= 1.1f;
  }

  // Priority 1: Attack if in range
  if (distanceToPlayer < effectiveAttackRange && CanAttack())
  {
    SetState(BotState::ATTACK);
    Attack();
  }
  // Priority 2: Coordinated chasing
  else if (distanceToPlayer < effectiveChaseRange && distanceToPlayer > effectiveAttackRange)
  {
    SetState(BotState::CHASING);
    ChasePlayer(playerPos, otherBots);
    MaintainFormation(otherBots); // SWAT-specific formation behavior
  }
  // Priority 3: Strategic retreat when outnumbered or low health
  else if (distanceToPlayer < fleeingRange &&
           (health < maxHealth * 0.4f || !IsNearOtherSwat(otherBots)))
  {
    SetState(BotState::FLEEING);
    MoveAway(playerPos);
  }
  // Priority 4: Disciplined patrolling
  else if (state == BotState::IDLE || state == BotState::WANDERING)
  {
    if (state == BotState::IDLE && stateTimer >= wanderTime)
    {
      SetState(BotState::WANDERING);
      wanderTimer = 0.0f;
    }

    if (state == BotState::WANDERING)
    {
      // SWAT units prefer patrolling in formation
      if (!patrolWaypoints.empty())
      {
        Patrol();
      }
      else
      {
        Wander(deltaTime, otherBots);
      }

      if (stateTimer >= wanderTime * 2.0f)
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

void SwatBot::Attack()
{
  if (CanAttack())
  {
    isAttacking = true;
    attackTimer = attackCooldown;
    attackAnim.curr = attackAnim.first;
    attackAnim.duration_left = attackAnim.speed;

    // SWAT units have precise, coordinated attacks
    TraceLog(LOG_INFO, "SWAT bot executing tactical strike!");
  }
}

void SwatBot::MaintainFormation(const std::vector<Bot *> &otherBots)
{
  // SWAT units try to maintain formation with other SWAT units
  Vector2 averageSwatPosition = {0.0f, 0.0f};
  int swatCount = 0;

  for (const Bot *otherBot : otherBots)
  {
    if (otherBot != this && otherBot->IsAlive() && otherBot->IsSpawned() &&
        otherBot->GetBotType() == BotType::SWAT)
    {
      averageSwatPosition.x += otherBot->x;
      averageSwatPosition.y += otherBot->y;
      swatCount++;
    }
  }

  if (swatCount > 0)
  {
    averageSwatPosition.x /= swatCount;
    averageSwatPosition.y /= swatCount;

    float distanceToFormation = Vector2Distance({x, y}, averageSwatPosition);

    // If too far from formation, move closer
    if (distanceToFormation > SWAT_FORMATION_DISTANCE)
    {
      Vector2 formationDirection = Vector2Normalize(
          Vector2Subtract(averageSwatPosition, {x, y}));

      float deltaTime = GetFrameTime();
      x += formationDirection.x * speed * 0.3f * deltaTime;
      y += formationDirection.y * speed * 0.3f * deltaTime;
    }
  }
}

bool SwatBot::IsNearOtherSwat(const std::vector<Bot *> &otherBots) const
{
  for (const Bot *otherBot : otherBots)
  {
    if (otherBot != this && otherBot->IsAlive() && otherBot->IsSpawned() &&
        otherBot->GetBotType() == BotType::SWAT)
    {
      float distance = Vector2Distance({x, y}, {otherBot->x, otherBot->y});
      if (distance < SWAT_FORMATION_DISTANCE * 1.5f)
      {
        return true;
      }
    }
  }
  return false;
}