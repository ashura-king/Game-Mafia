#include "includes/Bot.hpp"
#include "includes/GameType.hpp"
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
      TraceLog(LOG_ERROR, "Failed to load run texture %s", runPath.c_str()); // Fixed: LOG_ERROr -> LOG_ERROR
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
  idleRightAnim = {0, 7, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  idleLeftAnim = {0, 7, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  walkAnim = {0, 9, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  runAnim = {0, 9, 0, 0.1f, 0.1f, 1, AnimationType::REPEATING};
  attackAnim = {0, 5, 0, 0.1f, 0.1f, 1, AnimationType::ONESHOT};

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

  if (!IsAlive())
    return;

  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  stateTimer += deltaTime;

  if (distanceToPlayer < attackRange && CanAttack())
  {
    SetState(BotState::ATTACK);
    Attack();
  }

  else if (distanceToPlayer < chaseRange && distanceToPlayer > attackRange)
  {
    SetState(BotState::CHASING);
    chasePlayer(playerPos);
  }

  else if (distanceToPlayer > fleeingRange && state == BotState::CHASING)
  {
    SetState(BotState::FLEEING);
    wander(deltaTime);
  }
  // Default wandering
  else if (state == BotState::IDLE || stateTimer > wanderTime)
  {
    SetState(BotState::WANDERING);
    wander(deltaTime);
  }
}

bool Bot::CheckCollisionWithPlayer(Vector2 playerPos, float playerWidth, float playerHeight)
{
  Rectangle botRect = {x, y, width, height};
  Rectangle playerRect = {playerPos.x, playerPos.y, playerWidth, playerHeight};

  return CheckCollisionRecs(botRect, playerRect);
}

void Bot::chasePlayer(Vector2 playerPos)
{
  Vector2 direction_to_player = Vector2Subtract(playerPos, {x, y});
  Vector2 normalized_direction = Vector2Normalize(direction_to_player);

  float chaseSpeed = speed;
  float nextX = x + normalized_direction.x * chaseSpeed;
  float nextY = y + normalized_direction.y * chaseSpeed;

  Vector2 nextPos = {nextX, nextY};
  float distanceToPlayer = Vector2Distance(nextPos, playerPos);

  if (distanceToPlayer > 70.0f)
    x = nextX;
  y = nextY;

  if (normalized_direction.x < 0)
  {
    direction = Direction::LEFT;
  }
  else if (normalized_direction.x > 0)
  {
    direction = Direction::RIGHT;
  }
}

void Bot::wander(float deltaTime)
{
  wanderTimer -= deltaTime;

  if (wanderTimer <= 0.0f)
  {

    int wanderType = GetRandomValue(0, 2);

    if (wanderType == 0)
    {

      float wanderDistance = GetRandomValue(200, 400);
      float angle = GetRandomValue(0, 360) * DEG2RAD;

      wanderTarget.x = x + cosf(angle) * wanderDistance;
      wanderTarget.y = y + sinf(angle) * wanderDistance;

      wanderTimer = GetRandomValue(80, 100) / 10.0f;
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
      wanderTarget = {x, y};
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
void Bot::Attack()
{
  if (CanAttack())
  {
    isAttacking = true;
    attackTimer = attackCooldown;
    attackAnim.curr = attackAnim.first;
    attackAnim.duration_left = attackAnim.speed;
  }
}
bool Bot::CanAttack() const
{
  return attackTimer <= 0.0f && IsAlive();
}

void Bot::TakeDamage(int damage)
{
  health -= damage;
  if (health < 0)
    health = 0;

  TraceLog(LOG_INFO, "Bot took %d damage, health: %d", damage, health);
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
  Vector2 direction_to_target = Vector2Subtract(target, {x, y});
  float distance = Vector2Length(direction_to_target);

  if (distance > 5.0f)
  {
    Vector2 normalized_direction = Vector2Normalize(direction_to_target);

    x += normalized_direction.x * speed;
    y += normalized_direction.y * speed;

    if (normalized_direction.x < 0)
    {
      direction = Direction::LEFT;
    }
    else if (normalized_direction.x > 0)
    {
      direction = Direction::RIGHT;
    }
  }
}

void Bot::MoveAway(Vector2 threat)
{
  Vector2 direction_from_threat = Vector2Subtract({x, y}, threat);
  Vector2 normalized_direction = Vector2Normalize(direction_from_threat);

  x += normalized_direction.x * speed * 1.5f;
  y += normalized_direction.y * speed * 1.5f;

  if (normalized_direction.x < 0)

  {
    direction = Direction::LEFT;
  }
  else if (normalized_direction.x > 0)
  {
    direction = Direction::RIGHT;
  }
}
void Bot::Update()
{
  attackTimer -= GetFrameTime();
  attackTimer = std::max(attackTimer, 0.0f);
  float screenWidth = GetScreenWidth();
  if (x < 0)
    x = 0;
  if (x + width > screenWidth)
    x = screenWidth - width;
  UpdateAnimations();
}
void Bot::UpdateAnimations()
{
  switch (state)
  {
  case BotState::IDLE:
    if (direction == Direction::RIGHT)
    {
      Animation_Update(&idleRightAnim);
    }
    else
    {
      Animation_Update(&idleLeftAnim);
    }
    break;

  case BotState::WANDERING:
  case BotState::CHASING:
  case BotState::FLEEING:
    Animation_Update(&walkAnim);
    break;
  case BotState::ATTACK:
    Animation_Update(&attackAnim);

    if (attackAnim.curr >= attackAnim.last)
    {
      isAttacking = false;
      SetState(BotState::IDLE);
    }
    break;
  }
}

float Bot::DistanceTo(Vector2 target) const
{
  return Vector2Distance({x, y}, target);
}

bool Bot::IsPlayerInRange(Vector2 playerPosition, float range) const
{
  return DistanceTo(playerPosition) <= range;
}

void Bot::Draw()
{
  if (!isLoaded)
    return;

  Texture2D currentTexture;
  Rectangle source;

  GetTextureAndAnimation(currentTexture, source);

  Rectangle dest = {x, y, width, height};
  Vector2 origin = {0, 0};

  DrawTexturePro(currentTexture, source, dest, origin, 0.0f, WHITE);
}

void Bot::GetTextureAndAnimation(Texture2D &texture, Rectangle &source)
{
  switch (state)
  {
  case BotState::IDLE:
    if (direction == Direction::RIGHT)
    {
      texture = idleTexture;
      source = animation_frame(&idleRightAnim, 128, 128);
    }
    else
    {
      texture = idleLeftTexture;
      source = animation_frame(&idleLeftAnim, 128, 128);
    }
    break;

  case BotState::WANDERING:
  case BotState::CHASING:
  case BotState::FLEEING:
    texture = walkTexture;
    source = animation_frame(&walkAnim, 128, 128);
    if (direction == Direction::LEFT)
    {
      source.width = -source.width;
    }
    break;
  case BotState::ATTACK:
    texture = attackTexture;
    source = animation_frame(&attackAnim, 128, 128);
    if (direction == Direction::LEFT)
    {
      source.width = -source.width;
    }
    break;
  }
}
