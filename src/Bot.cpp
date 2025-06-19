#include "includes/Bot.hpp"
#include "raylib.h"
#include "raymath.h"
#include <string>

Bot::Bot(const std::string &idlePath,
         const std::string &idleLeftPath,
         const std::string &walkPath,
         const std::string &runPath,
         const std::string &attackPath,
         float startX,
         float startY,
         float botSpeed)
    : x(startX), y(startY), speed(botSpeed), direction(Direction::RIGHT),
      state(BotState::IDLE), previousState(BotState::IDLE), stateTimer(0.0f),
      wanderTimer(0.0f), currentWaypointIndex(0), waypointReachDistance(50.0f),
      isAttacking(false), attackTimer(0.0f), attackCooldown(1.0f), health(100),
      maxHealth(100), isLoaded(false)
{
  if (!idlePath.empty())
  {
    idleTexture = LoadTexture(idlePath.c_str());
    if (idleTexture.id == 0)
      TraceLog(LOG_ERROR, "Failed to load Idle texture: %s", idlePath.c_str());
  }

  if (!idleLeftPath.empty())
  {
    idleLeftTexture = LoadTexture(idleLeftPath.c_str());
    if (idleLeftTexture.id == 0)
      TraceLog(LOG_ERROR, "Failed to load IdleLeft texture: %s", idleLeftPath.c_str());
  }

  if (!walkPath.empty())
  {
    walkTexture = LoadTexture(walkPath.c_str());
    if (walkTexture.id == 0)
      TraceLog(LOG_ERROR, "Failed to load Walk texture: %s", walkPath.c_str());
  }
  if (!runPath.empty())
  {
    runTexture = LoadTexture(runPath.c_str());
    if (runTexture.id == 0)
    {
      TraceLog(LOG_ERROr, "failed to load run texture %s", runPath.c_str());
    }
  }
  if (!attackPath.empty())
  {
    attackTexture = LoadTexture(attackPath.c_str());
    if (attackTexture.id == 0)
      TraceLog(LOG_ERROR, "Failed to load Attack texture: %s", attackPath.c_str());
  }
  else
  {
    attackTexture = {0};
  }

  width = 128 * 2;
  height = 128 * 2;

  // Initialize animations
  idleRightAnim = {0, 4, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  idleLeftAnim = {0, 4, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  walkAnim = {0, 4, 0, 0.8f, 0.8f, 1, AnimationType::REPEATING};
  attackAnim = {0, 5, 0, 0.05f, 0.05f, 1, AnimationType::ONESHOT};

  isLoaded = true;
}

Bot::~Bot()
{
  if (idleTexture.id != 0)
    UnloadTexture(idleTexture);
  if (idleLeftTexture.id != 0)
    UnloadTexture(idleLeftTexture);
  if (walkTexture.id != 0)
    UnloadTexture(walkTexture);
  if (attackTexture.id != 0)
    UnloadTexture(attackTexture);
}

void Bot::UpdateAI(Vector2 playerPos, float deltaTime)
{
  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  stateTimer += deltaTime;

  if (distanceToPlayer < chaseRange)
  {
    SetState(BotState::CHASING);
    chasePlayer(playerPos);
  }
  else if (distanceToPlayer > fleeingRange && state == BotState::CHASING)
  {
    SetState(BotState::FLEEING);
    wander(deltaTime);
  }
  else if (state == BotState::IDLE || stateTimer > wanderTime)
  {
    SetState(BotState::WANDERING);
    wander(deltaTime);
  }
}
void Bot::chasePlayer(Vector2 playerPos)
{
  Vector2 direction_to_Player = Vector2Subtract(playerPos{x, y});
  Vector2 normalized_direction = Vector2Normalize(direction_to_Player);

  float chaseSpeed = speed * 1.5f;
  x += normalize_direction.x * chaseSpeed;
  y += normalize_direction.y * chaseSpeed;

  if (noramlize_direction.x < 0)
  {
    direction = LEFT;
  }
  else if (normalize_direction.y > 0)
  {
    direction = RIGHT;
  }
  else
  {
    printf("Error");
  }
}
void Bot::wander(float deltaTime)
{
  wanderTimer -= deltaTime;

  // If we don't have a wander target or timer expired, pick a new one
  if (wanderTimer <= 0.0f)
  {
    // Choose between different wander patterns
    int wanderType = GetRandomValue(0, 2);

    if (wanderType == 0)
    {
      // Long distance wander
      float wanderDistance = GetRandomValue(200, 400);
      float angle = GetRandomValue(0, 360) * DEG2RAD;

      wanderTarget.x = x + cosf(angle) * wanderDistance;
      wanderTarget.y = y + sinf(angle) * wanderDistance;

      wanderTimer = GetRandomValue(40, 80) / 10.0f;
    }
    else if (wanderType == 1)
    {

      float patrolDistance = GetRandomValue(150, 300);
      wanderTarget.x = x + (GetRandomValue(0, 1) ? patrolDistance : -patrolDistance);
      wanderTarget.y = y;

      wanderTimer = GetRandomValue(30, 60) / 10.0f;
    }
    else
    {

      wanderTarget = {x, y}; // Don't move
      wanderTimer = GetRandomValue(20, 40) / 10.0f;
    }

    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    wanderTarget.x = Clamp(wanderTarget.x, 100.0f, screenWidth - 100.0f);
    wanderTarget.y = Clamp(wanderTarget.y, 100.0f, screenHeight - 100.0f);
  }

  Vector2 direction_to_target = Vector2Subtract(wanderTarget, {x, y});
  float distance = Vector2Length(direction_to_target);

  if (distance > 20.0f)
  { // Increased threshold to avoid micro-movements
    Vector2 normalized_direction = Vector2Normalize(direction_to_target);

    float wanderSpeed = speed * GetRandomValue(40, 70) / 100.0f;

    x += normalized_direction.x * wanderSpeed;
    y += normalized_direction.y * wanderSpeed;

    if (fabsf(normalized_direction.x) > 0.1f)
    {
      if (normalized_direction.x < 0)
      {
        direction = Direction::LEFT;
      }
      else
      {
        direction = Direction::RIGHT;
      }
    }
  }
  else
  {
    // Reached target, pick new one soon
    if (wanderTimer > 1.0f)
    {
      wanderTimer = GetRandomValue(5, 15) / 10.0f;
    }
  }
}

void Bot::SetState(BotState newState)
{
  if (state != newState)
  {
    previousState = state;
    state = newState;
    stateTimer = 0.0f;

    if (newState == BotState::WANDERING)
    {
      wanderTimer = 0.0f;
    }
  }
}
void Bot::Patrol()
{
  if (patrolWaypoints.empty())
    return;
  Vector2 currentWaypoint = patrolWaypoints[currentWaypointIndex];
  float distanceWayPoint = Vector2Distance({x, y}, currentWaypoint);

  if (distanceWayPoint < waypointReachDistance)
  {
    currentWaypointIndex = (currentWaypointIndex + 1) % patrolWaypoints.size();
  }
  else
  {
    MoveTowards(currentWaypoint);
  }
}

void Bot::MoveTowards(Vector2 target)
{
  Vector2 distance_to_Target = Vector2Subtract(target, {x, y});
  float distance = Vector2Distance(direction_to_Target);
  if (distance > 5.0f)
  {
    Vector2 normalized_direction = Vector2Noramlize(direction_to_target);

    x += normalized_direction.x * speed;
    y += normalized _direction.y * speed;
    if (normalized_direction.x < 0)
    {
      direction = LEFT;
    }
    else if (normalized.y > 0)
    {
      direction = RIGHT;
    }
  }
}