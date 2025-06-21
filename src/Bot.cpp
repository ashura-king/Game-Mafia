#include "includes/Bot.hpp"
#include "includes/GameType.hpp"
#include "raylib.h"
#include "raymath.h"
#include <algorithm>

// Constructor
Bot::Bot(BotType botType, float startX, float startY)
    : type(botType),
      // Initialize textures to empty
      idleTexture({0}),
      idleLeftTexture({0}),
      walkTexture({0}),
      runTexture({0}),
      attackTexture({0}),
      // Initialize animations
      idleRightAnim({0, 7, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING}),
      idleLeftAnim({0, 7, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING}),
      walkAnim({0, 9, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING}),
      runAnim({0, 9, 0, 0.1f, 0.1f, 1, AnimationType::REPEATING}),
      attackAnim({0, 5, 0, 0.1f, 0.1f, 1, AnimationType::ONESHOT}),
      // Transform
      x(startX),
      y(startY),
      width(256.0f),
      height(256.0f),
      speed(0.0f),
      direction(Direction::RIGHT),
      // State management
      state(BotState::IDLE),
      previousState(BotState::IDLE),
      stateTimer(0.0f),
      // AI parameters (will be set by SetBotProperties)
      chaseRange(0.0f),
      attackRange(0.0f),
      fleeingRange(0.0f),
      wanderTime(0.0f),
      wanderTarget({0.0f, 0.0f}),
      wanderTimer(0.0f),
      // Patrol system
      patrolWaypoints(),
      currentWaypointIndex(0),
      waypointReachDistance(50.0f),
      // Combat
      isAttacking(false),
      attackTimer(0.0f),
      attackCooldown(1.0f),
      health(100),
      maxHealth(100),
      // Initialization
      isLoaded(false)
{
  SetBotProperties(botType);
  isLoaded = true;
}

// Destructor
Bot::~Bot()
{
  if (idleTexture.id != 0)
    UnloadTexture(idleTexture);
  if (idleLeftTexture.id != 0)
    UnloadTexture(idleLeftTexture);
  if (walkTexture.id != 0)
    UnloadTexture(walkTexture);
  if (runTexture.id != 0)
    UnloadTexture(runTexture);
  if (attackTexture.id != 0)
    UnloadTexture(attackTexture);
}

// Bot type configuration
void Bot::SetBotProperties(BotType botType)
{
  switch (botType)
  {
  case BotType::THUG:
    // Load thug sprites
    idleTexture = LoadTexture("resource/thugIdle.png");
    idleLeftTexture = LoadTexture("resource/thugIdle.png");
    walkTexture = LoadTexture("resource/thugWalk.png");
    runTexture = LoadTexture("resource/thugRun.png");
    attackTexture = LoadTexture("resource/thugAttack.png");

    // Thug properties - fast and aggressive
    speed = 120.0f;
    health = 100;
    maxHealth = 100;
    attackRange = 100.0f;
    chaseRange = 300.0f;
    fleeingRange = 200.0f;
    attackCooldown = 0.6f;
    wanderTime = 4.0f;
    break;

  case BotType::CIVILIAN:
    // Load civilian sprites
    idleTexture = LoadTexture("resource/civilianIdle.png");
    idleLeftTexture = LoadTexture("resource/civilianIdle2.png");
    walkTexture = LoadTexture("resource/civilianWalk.png");
    runTexture = LoadTexture("resource/civilianRun.png");
    attackTexture = LoadTexture("resource/civilianIdle.png");

    // Civilian properties - weak and passive
    speed = 60.0f;
    health = 50;
    maxHealth = 50;
    attackRange = 0.0f;      // Civilians don't attack
    chaseRange = 0.0f;       // Civilians don't chase
    fleeingRange = 150.0f;   // But they flee quickly
    attackCooldown = 999.0f; // Can't attack
    wanderTime = 10.0f;
    break;

  case BotType::GANGSTER:
    // Load gangster sprites
    idleTexture = LoadTexture("resource/enemyIdle.png");
    idleLeftTexture = LoadTexture("resource/enemyIdle2.png");
    walkTexture = LoadTexture("resource/enemyWalk.png");
    runTexture = LoadTexture("resource/enemyRun.png");
    attackTexture = LoadTexture("resource/enemyAttack.png");

    // Gangster properties - tough and persistent
    speed = 110.0f;
    health = 130;
    maxHealth = 130;
    attackRange = 110.0f;
    chaseRange = 350.0f;
    fleeingRange = 400.0f;
    attackCooldown = 0.7f;
    wanderTime = 3.0f;
    break;

  case BotType::POLICE:
    // Load police sprites (using gangster sprites as placeholder)
    idleTexture = LoadTexture("resource/policeIdle.png");
    idleLeftTexture = LoadTexture("resource/policeIdleLeft.png");
    walkTexture = LoadTexture("resource/policeWalk.png");
    runTexture = LoadTexture("resource/policeRun.png");
    attackTexture = LoadTexture("resource/policeAttack.png");

    // Police properties - balanced and disciplined
    speed = 100.0f;
    health = 120;
    maxHealth = 120;
    attackRange = 130.0f;
    chaseRange = 400.0f;
    fleeingRange = 300.0f;
    attackCooldown = 0.5f;
    wanderTime = 6.0f;
    break;

  default:
    TraceLog(LOG_WARNING, "Unknown bot type in SetBotProperties");
    break;
  }

  // Validate texture loading
  if (idleTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load idle texture for bot type %d", (int)botType);
  if (idleLeftTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load idle left texture for bot type %d", (int)botType);
  if (walkTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load walk texture for bot type %d", (int)botType);
  if (runTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load run texture for bot type %d", (int)botType);
  if (attackTexture.id == 0)
    TraceLog(LOG_WARNING, "Failed to load attack texture for bot type %d", (int)botType);
}

// Main update loop
void Bot::Update()
{
  // Update attack timer
  attackTimer -= GetFrameTime();
  attackTimer = std::max(attackTimer, 0.0f);

  // Keep bot within screen bounds
  float screenWidth = GetScreenWidth();
  float screenHeight = GetScreenHeight();

  x = Clamp(x, 0.0f, screenWidth - width);
  y = Clamp(y, 0.0f, screenHeight - height);

  UpdateAnimations();
}

void Bot::UpdateAI(Vector2 playerPos, float deltaTime)
{
  if (!IsAlive())
    return;

  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  stateTimer += deltaTime;

  // Priority: Attack if in range and can attack
  if (distanceToPlayer < attackRange && CanAttack())
  {
    SetState(BotState::ATTACK);
    Attack();
  }
  // Chase if player is in chase range but not attack range
  else if (distanceToPlayer < chaseRange && distanceToPlayer > attackRange)
  {
    SetState(BotState::CHASING);
    chasePlayer(playerPos);
  }
  // Flee if player is too far and was chasing
  else if (distanceToPlayer > fleeingRange && state == BotState::CHASING)
  {
    SetState(BotState::FLEEING);
    wander(deltaTime);
  }
  // Default wandering behavior
  else if (state == BotState::IDLE || stateTimer > wanderTime)
  {
    SetState(BotState::WANDERING);
    wander(deltaTime);
  }
}

// AI Behaviors
void Bot::chasePlayer(Vector2 playerPos)
{
  Vector2 directionToPlayer = Vector2Subtract(playerPos, {x, y});
  Vector2 normalizedDirection = Vector2Normalize(directionToPlayer);

  float deltaTime = GetFrameTime();
  float nextX = x + normalizedDirection.x * speed * deltaTime;
  float nextY = y + normalizedDirection.y * speed * deltaTime;

  // Maintain minimum distance to avoid overlapping
  Vector2 nextPos = {nextX, nextY};
  float distanceToPlayer = Vector2Distance(nextPos, playerPos);

  if (distanceToPlayer > 70.0f)
  {
    x = nextX;
    y = nextY;
  }

  // Update facing direction
  if (normalizedDirection.x < -0.1f)
    direction = Direction::LEFT;
  else if (normalizedDirection.x > 0.1f)
    direction = Direction::RIGHT;
}

void Bot::wander(float deltaTime)
{
  wanderTimer -= deltaTime;

  // Set new wander target
  if (wanderTimer <= 0.0f)
  {
    int wanderType = GetRandomValue(0, 2);

    if (wanderType == 0) // Random circular movement
    {
      float wanderDistance = GetRandomValue(200, 400);
      float angle = GetRandomValue(0, 360) * DEG2RAD;

      wanderTarget.x = x + cosf(angle) * wanderDistance;
      wanderTarget.y = y + sinf(angle) * wanderDistance;
      wanderTimer = GetRandomValue(80, 100) / 10.0f;
    }
    else if (wanderType == 1) // Horizontal patrol
    {
      float patrolDistance = GetRandomValue(150, 300);
      wanderTarget.x = x + (GetRandomValue(0, 1) ? patrolDistance : -patrolDistance);
      wanderTarget.y = y;
      wanderTimer = GetRandomValue(30, 60) / 10.0f;
    }
    else // Stay in place
    {
      wanderTarget = {x, y};
      wanderTimer = GetRandomValue(20, 40) / 10.0f;
    }

    // Clamp to screen bounds
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    wanderTarget.x = Clamp(wanderTarget.x, 100.0f, screenWidth - 100.0f);
    wanderTarget.y = Clamp(wanderTarget.y, 100.0f, screenHeight - 100.0f);
  }

  // Move towards wander target
  Vector2 directionToTarget = Vector2Subtract(wanderTarget, {x, y});
  float distance = Vector2Length(directionToTarget);

  if (distance > 20.0f)
  {
    Vector2 normalizedDirection = Vector2Normalize(directionToTarget);
    float wanderSpeed = speed * GetRandomValue(40, 70) / 100.0f;

    x += normalizedDirection.x * wanderSpeed * deltaTime;
    y += normalizedDirection.y * wanderSpeed * deltaTime;

    // Update facing direction
    if (fabsf(normalizedDirection.x) > 0.1f)
    {
      direction = (normalizedDirection.x < 0) ? Direction::LEFT : Direction::RIGHT;
    }
  }
  else if (wanderTimer > 1.0f)
  {
    // Reached target early, set new timer
    wanderTimer = GetRandomValue(5, 15) / 10.0f;
  }
}

void Bot::Patrol()
{
  if (patrolWaypoints.empty())
    return;

  Vector2 currentWaypoint = patrolWaypoints[currentWaypointIndex];
  float distanceToWaypoint = Vector2Distance({x, y}, currentWaypoint);

  if (distanceToWaypoint < waypointReachDistance)
  {
    currentWaypointIndex = (currentWaypointIndex + 1) % patrolWaypoints.size();
  }
  else
  {
    MoveTowards(currentWaypoint);
  }
}

// Movement methods
void Bot::MoveTowards(Vector2 target)
{
  Vector2 directionToTarget = Vector2Subtract(target, {x, y});
  float distance = Vector2Length(directionToTarget);

  if (distance > 5.0f)
  {
    Vector2 normalizedDirection = Vector2Normalize(directionToTarget);
    float deltaTime = GetFrameTime();

    x += normalizedDirection.x * speed * deltaTime;
    y += normalizedDirection.y * speed * deltaTime;

    // Update facing direction
    if (normalizedDirection.x < -0.1f)
      direction = Direction::LEFT;
    else if (normalizedDirection.x > 0.1f)
      direction = Direction::RIGHT;
  }
}

void Bot::MoveAway(Vector2 threat)
{
  Vector2 directionFromThreat = Vector2Subtract({x, y}, threat);
  Vector2 normalizedDirection = Vector2Normalize(directionFromThreat);
  float deltaTime = GetFrameTime();

  // Move faster when fleeing
  x += normalizedDirection.x * speed * 1.5f * deltaTime;
  y += normalizedDirection.y * speed * 1.5f * deltaTime;

  // Update facing direction
  if (normalizedDirection.x < -0.1f)
    direction = Direction::LEFT;
  else if (normalizedDirection.x > 0.1f)
    direction = Direction::RIGHT;
}

// State management
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

// Combat system
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

// Utility functions
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

// Animation system
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

void Bot::GetTextureAndAnimation(Texture2D &texture, Rectangle &source)
{
  const int frameWidth = 128;
  const int frameHeight = 128;

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
      source.x += frameWidth;     // Shift x before flipping
      source.width = -frameWidth; // Flip horizontally
    }
    break;

  case BotState::WANDERING:
  case BotState::CHASING:
  case BotState::FLEEING:
    texture = walkTexture;
    source = animation_frame(&walkAnim, frameWidth, frameHeight);
    if (direction == Direction::LEFT)
    {
      source.x += frameWidth;
      source.width = -frameWidth;
    }
    break;

  case BotState::ATTACK:
    texture = attackTexture;
    source = animation_frame(&attackAnim, frameWidth, frameHeight);
    if (direction == Direction::LEFT)
    {
      source.x += frameWidth;
      source.width = -frameWidth;
    }
    break;
  }
}

// Rendering
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