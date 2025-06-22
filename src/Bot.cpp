#include "includes/Bot.hpp"
#include <raylib.h>
#include <cmath>
#include <algorithm>

bool TextureIsValid(Texture2D tex)
{
  return tex.id > 0;
}

Bot::Bot(float startX, float startY)
{
  x = startX;
  y = startY;

  // Initialize default bot values
  width = 64.0f;
  height = 64.0f;
  speed = 100.0f;
  health = 100;
  maxHealth = 100;

  direction = Direction::RIGHT;
  state = BotState::IDLE;
  previousState = BotState::IDLE;
  stateTimer = 0.0f;
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

  // Initialize AI parameters
  chaseRange = 200.0f;
  attackRange = 50.0f;
  fleeingRange = 150.0f;
  wanderTime = 3.0f;
  attackCooldown = 1.0f;
  spawnDelay = 0.5f;
  isLoaded = false;
  type = BotType::CIVILIAN; // Set default type

  idleRightAnim = {0, 0, 0, 0.2f, 0.2f, 1, AnimationType::REPEATING};
  idleLeftAnim = {0, 0, 0, 0.2f, 0.2f, 1, AnimationType::REPEATING};
  walkAnim = {0, 0, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  runAnim = {0, 0, 0, 0.1f, 0.1f, 1, AnimationType::REPEATING};
  attackAnim = {0, 0, 0, 0.1f, 0.1f, 1, AnimationType::ONESHOT};
}

// Fixed destructor - removed redefinition
Bot::~Bot()
{
  if (TextureIsValid(idleTexture))
    UnloadTexture(idleTexture);
  if (TextureIsValid(walkTexture))
    UnloadTexture(walkTexture);
  if (TextureIsValid(runTexture))
    UnloadTexture(runTexture);
  if (TextureIsValid(attackTexture))
    UnloadTexture(attackTexture);
  if (TextureIsValid(idleLeftTexture))
    UnloadTexture(idleLeftTexture);
}

// Virtual function - can be overridden by derived classes
void Bot::LoadTextures()
{
  // Base implementation - load default textures or leave empty for derived classes
  // This is a virtual function that derived classes should override
}

void Bot::SetProperties()
{
  // Base implementation - set default properties
  // This is a virtual function that derived classes should override
}

void Bot::LoadTexturesSafe()
{
  if (!TextureIsValid(idleTexture))
    TraceLog(LOG_WARNING, "Idle texture invalid");
  if (!TextureIsValid(walkTexture))
    TraceLog(LOG_WARNING, "Walk texture invalid");
  if (!TextureIsValid(runTexture))
    TraceLog(LOG_WARNING, "Run texture invalid");
  if (!TextureIsValid(attackTexture))
    TraceLog(LOG_WARNING, "Attack texture invalid");
  if (!TextureIsValid(idleLeftTexture))
    TraceLog(LOG_WARNING, "Idle left texture invalid");
}

// Parameterless Update function as declared in header
void Bot::Update()
{
  if (!isSpawned || !IsAlive())
    return;

  float deltaTime = GetFrameTime();
  UpdateAnimations();
}

// The more detailed Update function
void Bot::UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  if (!isSpawned || !IsAlive())
    return;

  stateTimer += deltaTime;
  attackTimer -= deltaTime;

  switch (state)
  {
  case BotState::IDLE:
    // Handle idle behavior
    wanderTimer += deltaTime;
    if (wanderTimer >= wanderTime)
    {
      SetState(BotState::WANDERING);
      wanderTimer = 0.0f;
    }
    break;

  case BotState::WANDERING:
    Wander(deltaTime, otherBots);
    break;

  case BotState::CHASING:
    ChasePlayer(playerPos, otherBots);
    break;

  case BotState::FLEEING:
    MoveAway(playerPos);
    break;

  case BotState::ATTACK:
    if (CanAttack())
    {
      Attack();
    }
    break;

  case BotState::PATROL:
    Patrol();
    break;
  }
}

void Bot::Draw()
{
  if (!isSpawned || !IsAlive())
    return;

  Texture2D currentTexture;
  Rectangle sourceRec;
  GetTextureAndAnimation(currentTexture, sourceRec);

  if (TextureIsValid(currentTexture))
  {
    Rectangle destRec = {x, y, width, height};
    DrawTexturePro(currentTexture, sourceRec, destRec, {0, 0}, 0.0f, WHITE);
  }
  else
  {
    // Draw a simple rectangle if no texture is available
    DrawRectangle((int)x, (int)y, (int)width, (int)height, RED);
  }
}

void Bot::ChasePlayer(Vector2 playerPos, const std::vector<Bot *> &otherBots)
{
  MoveTowards(playerPos);

  // Check if close enough to attack
  if (DistanceTo(playerPos) <= attackRange && CanAttack())
  {
    SetState(BotState::ATTACK);
  }
}

void Bot::Wander(float deltaTime, const std::vector<Bot *> &otherBots)
{
  // Simple wandering behavior
  wanderTimer += deltaTime;

  if (wanderTimer >= wanderTime || Vector2Distance({x, y}, wanderTarget) < 10.0f)
  {
    // Set new random wander target
    wanderTarget.x = x + (float)(GetRandomValue(-200, 200));
    wanderTarget.y = y + (float)(GetRandomValue(-200, 200));
    wanderTimer = 0.0f;
  }

  MoveTowards(wanderTarget);
}

void Bot::MoveTowards(Vector2 target)
{
  Vector2 direction = Vector2Subtract(target, {x, y});
  direction = Vector2Normalize(direction);

  float deltaTime = GetFrameTime();
  Vector2 newPos = {
      x + direction.x * speed * deltaTime,
      y + direction.y * speed * deltaTime};

  x = newPos.x;
  y = newPos.y;

  UpdateDirection(direction);
}

void Bot::MoveAway(Vector2 threat)
{
  Vector2 direction = Vector2Subtract({x, y}, threat);
  direction = Vector2Normalize(direction);

  float deltaTime = GetFrameTime();
  Vector2 newPos = {
      x + direction.x * speed * deltaTime,
      y + direction.y * speed * deltaTime};

  x = newPos.x;
  y = newPos.y;

  UpdateDirection(direction);
}

void Bot::Patrol()
{
  if (patrolWaypoints.empty())
    return;

  Vector2 currentWaypoint = patrolWaypoints[currentWaypointIndex];

  if (DistanceTo(currentWaypoint) <= waypointReachDistance)
  {
    currentWaypointIndex = (currentWaypointIndex + 1) % patrolWaypoints.size();
  }

  MoveTowards(currentWaypoint);
}

void Bot::Attack()
{
  if (!CanAttack())
    return;

  isAttacking = true;
  attackTimer = attackCooldown;

  // Play attack animation
  attackAnim.curr = attackAnim.first;
  attackAnim.duration_left = attackAnim.speed;
}

void Bot::TakeDamage(int damage)
{
  health -= damage;
  if (health < 0)
    health = 0;
}

float Bot::DistanceTo(Vector2 target) const
{
  return Vector2Distance({x, y}, target);
}

bool Bot::IsPlayerInRange(Vector2 playerPosition, float range) const
{
  return DistanceTo(playerPosition) <= range;
}

bool Bot::CheckCollisionWithPlayer(Vector2 playerPos, float playerWidth, float playerHeight)
{
  Rectangle botRect = {x, y, width, height};
  Rectangle playerRect = {playerPos.x, playerPos.y, playerWidth, playerHeight};
  return CheckCollisionRecs(botRect, playerRect);
}

bool Bot::WouldCollideWithBots(Vector2 position, const std::vector<Bot *> &otherBots) const
{
  Rectangle testRect = {position.x, position.y, width, height};

  for (const Bot *otherBot : otherBots)
  {
    if (otherBot == this || !otherBot->IsAlive() || !otherBot->IsSpawned())
      continue;

    Rectangle otherRect = {otherBot->x, otherBot->y, otherBot->width, otherBot->height};
    if (CheckCollisionRecs(testRect, otherRect))
      return true;
  }

  return false;
}

Vector2 Bot::GetAvoidanceDirection(Vector2 blockedPosition, const std::vector<Bot *> &otherBots) const
{
  // Simple avoidance - try different directions
  Vector2 directions[] = {
      {1.0f, 0.0f}, {-1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, -1.0f}, {0.707f, 0.707f}, {-0.707f, 0.707f}, {0.707f, -0.707f}, {-0.707f, -0.707f}};

  for (const Vector2 &dir : directions)
  {
    Vector2 testPos = Vector2Add(blockedPosition, Vector2Scale(dir, 50.0f));
    if (!WouldCollideWithBots(testPos, otherBots))
    {
      return dir;
    }
  }

  return lastValidDirection;
}

void Bot::UpdateAnimations()
{
  switch (state)
  {
  case BotState::IDLE:
    if (direction == Direction::RIGHT)
      Animation_Update(&idleRightAnim);
    else
      Animation_Update(&idleLeftAnim);
    break;

  case BotState::WANDERING:
  case BotState::CHASING:
  case BotState::FLEEING:
  case BotState::PATROL:
    Animation_Update(&walkAnim);
    break;

  case BotState::ATTACK:
    Animation_Update(&attackAnim);
    break;
  }
}

void Bot::GetTextureAndAnimation(Texture2D &texture, Rectangle &source)
{
  int frameWidth = 64; // Adjust based on your sprite sheet
  int frameHeight = 64;

  switch (state)
  {
  case BotState::IDLE:
    if (direction == Direction::RIGHT)
    {
      texture = idleTexture;
      source = animation_frame(&idleRightAnim, frameWidth, frameHeight);
    }
    else
    {
      texture = idleLeftTexture;
      source = animation_frame(&idleLeftAnim, frameWidth, frameHeight);
    }
    break;

  case BotState::WANDERING:
  case BotState::CHASING:
  case BotState::FLEEING:
  case BotState::PATROL:
    texture = walkTexture;
    source = animation_frame(&walkAnim, frameWidth, frameHeight);
    break;

  case BotState::ATTACK:
    texture = attackTexture;
    source = animation_frame(&attackAnim, frameWidth, frameHeight);
    break;

  default:
    texture = idleTexture;
    source = animation_frame(&idleRightAnim, frameWidth, frameHeight);
    break;
  }
}

void Bot::UpdateDirection(Vector2 movementVector)
{
  directionChangeTimer += GetFrameTime();

  if (directionChangeTimer >= DIRECTION_CHANGE_DELAY)
  {
    if (fabsf(movementVector.x) > fabsf(movementVector.y))
    {
      direction = (movementVector.x > 0) ? Direction::RIGHT : Direction::LEFT;
    }

    if (movementVector.x != 0 || movementVector.y != 0)
    {
      lastValidDirection = movementVector;
    }

    directionChangeTimer = 0.0f;
  }
}

// Removed custom Animate function - using global Animation_Update instead

// Removed GetFrameRec - using global animation_frame function instead

bool Bot::CanAttack() const
{
  return attackTimer <= 0.0f;
}

// IsAlive() and IsSpawned() are implemented inline in the header

BotType Bot::GetBotType() const
{
  return type;
}

void Bot::SetState(BotState newState)
{
  if (state != newState)
  {
    previousState = state;
    state = newState;
    stateTimer = 0.0f;
  }
}