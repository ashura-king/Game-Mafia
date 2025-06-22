#include "includes/Civillian.hpp"
#include "raylib.h"

CivilianBot::CivilianBot(float startX, float startY) : Bot(startX, startY)
{
  LoadTextures();
  SetProperties();
  isLoaded = true;
}

void CivilianBot::LoadTextures()
{
  // Load civilian-specific sprites
  idleTexture = LoadTexture("resource/civillian/civilIdle.png");
  idleLeftTexture = LoadTexture("resource/civillian/civilIdle2.png");
  walkTexture = LoadTexture("resource/civillian/civilWalk.png");
  runTexture = LoadTexture("resource/civillian/civilRun.png");
  attackTexture = LoadTexture("resource/civillian/civilIdle.png"); // Civilians use idle for "attack"
  LoadTexturesSafe();

  // Validate textures
  if (idleTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load civilian idle texture");
  if (idleLeftTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load civilian idle left texture");
  if (walkTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load civilian walk texture");
  if (runTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load civilian run texture");
}

void CivilianBot::SetProperties()
{
  // Initialize animations
  idleRightAnim = {0, 7, 0, 0.2f, 0.2f, 1, AnimationType::REPEATING}; // Slower animation
  idleLeftAnim = {0, 7, 0, 0.2f, 0.2f, 1, AnimationType::REPEATING};
  walkAnim = {0, 9, 0, 0.18f, 0.18f, 1, AnimationType::REPEATING};
  runAnim = {0, 9, 0, 0.12f, 0.12f, 1, AnimationType::REPEATING};
  attackAnim = {0, 5, 0, 0.2f, 0.2f, 1, AnimationType::ONESHOT};

  // Civilian properties - weak and passive
  speed = 60.0f;
  health = 50;
  maxHealth = 50;
  attackRange = 0.0f;      // Civilians don't attack
  chaseRange = 0.0f;       // Civilians don't chase
  fleeingRange = 150.0f;   // But they flee quickly
  attackCooldown = 999.0f; // Can't attack
  wanderTime = 10.0f;      // Longer peaceful wandering
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

void CivilianBot::UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  // Don't update AI if not spawned yet or not alive
  if (!isSpawned || !IsAlive())
    return;

  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  stateTimer += deltaTime;

  // Enhanced flee range when near danger
  float effectiveFleeRange = fleeingRange;
  if (IsNearDanger(playerPos, otherBots))
  {
    effectiveFleeRange *= CIVILIAN_PANIC_MULTIPLIER;
  }

  // Priority 1: Always flee from player or danger
  if (distanceToPlayer < effectiveFleeRange || IsNearDanger(playerPos, otherBots))
  {
    SetState(BotState::FLEEING);
    PanicFlee(playerPos, otherBots);
  }
  // Priority 2: Peaceful wandering when safe
  else if (state == BotState::IDLE || state == BotState::WANDERING)
  {
    if (state == BotState::IDLE && stateTimer >= wanderTime)
    {
      SetState(BotState::WANDERING);
      wanderTimer = 0.0f;
    }

    if (state == BotState::WANDERING)
    {
      // Civilians wander more peacefully and slowly
      Wander(deltaTime, otherBots);

      // Longer wandering periods
      if (stateTimer >= wanderTime * 1.5f)
      {
        SetState(BotState::IDLE);
      }
    }
  }
  else
  {
    // Default to idle when safe
    SetState(BotState::IDLE);
  }
}

void CivilianBot::Attack()
{

  TraceLog(LOG_INFO, "Civilian bot cannot attack - cowering in fear!");

  SetState(BotState::IDLE);
}

void CivilianBot::PanicFlee(Vector2 threat, const std::vector<Bot *> &otherBots)
{

  Vector2 fleeDirection = Vector2Subtract({x, y}, threat);

  if (Vector2Length(fleeDirection) < 50.0f)
  {
    float randomAngle = GetRandomValue(0, 360) * DEG2RAD;
    fleeDirection = {cosf(randomAngle), sinf(randomAngle)};
  }
  else
  {
    fleeDirection = Vector2Normalize(fleeDirection);
  }

  for (const Bot *otherBot : otherBots)
  {
    if (otherBot == this || !otherBot->IsAlive() || !otherBot->IsSpawned())
      continue;

    if (otherBot->GetBotType() == BotType::THUG ||
        otherBot->GetBotType() == BotType::GANGSTER ||
        otherBot->GetBotType() == BotType::SWAT)
    {
      float distanceToThreat = Vector2Distance({x, y}, {otherBot->x, otherBot->y});
      if (distanceToThreat < fleeingRange)
      {
        Vector2 additionalFleeDir = Vector2Normalize(
            Vector2Subtract({x, y}, {otherBot->x, otherBot->y}));
        fleeDirection = Vector2Add(fleeDirection, additionalFleeDir);
      }
    }
  }

  fleeDirection = Vector2Normalize(fleeDirection);

  float deltaTime = GetFrameTime();
  float panicSpeed = speed * CIVILIAN_FLEE_SPEED_BOOST;

  Vector2 nextPosition = {
      x + fleeDirection.x * panicSpeed * deltaTime,
      y + fleeDirection.y * panicSpeed * deltaTime};

  // Basic boundary checking (assuming screen bounds)
  if (nextPosition.x >= 0 && nextPosition.x <= GetScreenWidth() - width &&
      nextPosition.y >= 0 && nextPosition.y <= GetScreenHeight() - height)
  {
    x = nextPosition.x;
    y = nextPosition.y;
  }
  else
  {
    // If hitting boundary, try to move along the edge
    if (nextPosition.x < 0 || nextPosition.x > GetScreenWidth() - width)
    {
      fleeDirection.x = -fleeDirection.x; // Reverse X direction
    }
    if (nextPosition.y < 0 || nextPosition.y > GetScreenHeight() - height)
    {
      fleeDirection.y = -fleeDirection.y; // Reverse Y direction
    }

    // Try movement with corrected direction
    x += fleeDirection.x * panicSpeed * deltaTime * 0.5f;
    y += fleeDirection.y * panicSpeed * deltaTime * 0.5f;
  }

  // Update direction for sprite rendering
  if (fleeDirection.x > 0)
    direction = Direction::RIGHT;
  else if (fleeDirection.x < 0)
    direction = Direction::LEFT;

  // Store the flee direction for consistency
  lastValidDirection = fleeDirection;
}

bool CivilianBot::IsNearDanger(Vector2 playerPos, const std::vector<Bot *> &otherBots) const
{
  // Check distance to player
  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  if (distanceToPlayer < fleeingRange * 0.8f) // Slightly closer than flee range
    return true;

  // Check for nearby hostile bots
  for (const Bot *otherBot : otherBots)
  {
    if (otherBot == this || !otherBot->IsAlive() || !otherBot->IsSpawned())
      continue;

    // Consider certain bot types as dangerous
    BotType botType = otherBot->GetBotType();
    if (botType == BotType::THUG ||
        botType == BotType::GANGSTER ||
        botType == BotType::SWAT)
    {
      float distanceToThreat = Vector2Distance({x, y}, {otherBot->x, otherBot->y});

      // Civilians are very sensitive to danger
      if (distanceToThreat < fleeingRange * 0.7f)
        return true;
    }
  }

  return false;
}