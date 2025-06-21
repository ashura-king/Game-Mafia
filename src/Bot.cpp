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
      // Bot spawning system - ADD THESE
      spawnDelay(15.0f), // 15 second delay before bot becomes active
      spawnTimer(0.0f),  // Current spawn timer
      isSpawned(false),  // Whether bot is spawned/active
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
    // Load thug sprites - FIXED PATHS
    idleTexture = LoadTexture("resource/thug/thugIdle.png");
    idleLeftTexture = LoadTexture("resource/thug/thugIdle.png"); // Use same for both sides
    walkTexture = LoadTexture("resource/thug/thugWalk.png");     // Fixed case: Walk not walk
    runTexture = LoadTexture("resource/thug/thugRun.png");
    attackTexture = LoadTexture("resource/thug/thugAttack.png");

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
    // Load civilian sprites - FIXED PATHS
    idleTexture = LoadTexture("resource/civilian/civilIdle.png");
    idleLeftTexture = LoadTexture("resource/civilian/civilIdle2.png");
    walkTexture = LoadTexture("resource/civilian/civilWalk.png");
    runTexture = LoadTexture("resource/civilian/civilRun.png");
    attackTexture = LoadTexture("resource/civilian/civilIdle.png");
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
    idleTexture = LoadTexture("resource/gangster/gangsterIdle.png");
    idleLeftTexture = LoadTexture("resource/gangster/gangsterIdle2.png");
    walkTexture = LoadTexture("resource/gangster/gangsterWalk.png");
    runTexture = LoadTexture("resource/gangster/gangsterRun.png");
    attackTexture = LoadTexture("resource/gangster/gangsterAttack.png");

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

  case BotType::SWAT:
    // Load police sprites
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

  // FIXED validation code - proper order
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
  float deltaTime = GetFrameTime();

  // Handle spawn delay - bot doesn't become active until 15 seconds
  if (!isSpawned)
  {
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnDelay)
    {
      isSpawned = true;
      TraceLog(LOG_INFO, "Bot type %d spawned after %f seconds", (int)type, spawnTimer);
    }
    else
    {
      // Bot is not active yet, just update animations but don't do AI
      UpdateAnimations();
      return;
    }
  }

  // Update attack timer
  attackTimer -= deltaTime;
  attackTimer = std::max(attackTimer, 0.0f);

  // Keep bot within screen bounds
  float screenWidth = GetScreenWidth();
  float screenHeight = GetScreenHeight();

  x = Clamp(x, 0.0f, screenWidth - width);
  y = Clamp(y, 0.0f, screenHeight - height);

  UpdateAnimations();
}

void Bot::UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  // Don't update AI if not spawned yet or not alive
  if (!isSpawned || !IsAlive())
    return;

  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  stateTimer += deltaTime;

  // Priority 1: Attack if in range and can attack
  if (distanceToPlayer < attackRange && CanAttack() && attackRange > 0.0f)
  {
    SetState(BotState::ATTACK);
    Attack();
  }
  // Priority 2: Chase if player is in chase range but not attack range
  else if (distanceToPlayer < chaseRange && distanceToPlayer > attackRange && chaseRange > 0.0f)
  {
    SetState(BotState::CHASING);
    ChasePlayer(playerPos, otherBots); // Pass other bots to avoid overlap
  }
  // Priority 3: Flee if player is close and bot should flee
  else if (distanceToPlayer < fleeingRange && fleeingRange > 0.0f &&
           (type == BotType::CIVILIAN || health < maxHealth * 0.3f)) // Civilians always flee, others flee when low health
  {
    SetState(BotState::FLEEING);
    MoveAway(playerPos);
  }
  // Priority 4: Wander when idle
  else if (state == BotState::IDLE || state == BotState::WANDERING)
  {
    if (state == BotState::IDLE && stateTimer >= wanderTime)
    {
      SetState(BotState::WANDERING);
      wanderTimer = 0.0f; // Reset wander timer
    }

    if (state == BotState::WANDERING)
    {
      Wander(deltaTime, otherBots); // Pass other bots to avoid overlap

      // Return to idle after wandering for a while
      if (stateTimer >= wanderTime * 2.0f)
      {
        SetState(BotState::IDLE);
      }
    }
  }
  else
  {
    // Default back to idle if no other conditions are met
    SetState(BotState::IDLE);
  }
}

// FIXED AI Behaviors with collision avoidance
void Bot::ChasePlayer(Vector2 playerPos, const std::vector<Bot *> &otherBots)
{
  Vector2 directionToPlayer = Vector2Subtract(playerPos, {x, y});
  Vector2 normalizedDirection = Vector2Normalize(directionToPlayer);

  float deltaTime = GetFrameTime();
  float nextX = x + normalizedDirection.x * speed * deltaTime;
  float nextY = y + normalizedDirection.y * speed * deltaTime;

  // Check collision with other bots before moving
  Vector2 nextPos = {nextX, nextY};
  if (!WouldCollideWithBots(nextPos, otherBots))
  {
    // Maintain minimum distance to player to avoid overlapping
    float distanceToPlayer = Vector2Distance(nextPos, playerPos);
    if (distanceToPlayer > 70.0f)
    {
      x = nextX;
      y = nextY;
    }
  }
  else
  {
    // Try to move around the obstacle
    Vector2 avoidDirection = GetAvoidanceDirection(nextPos, otherBots);
    x += avoidDirection.x * speed * 0.5f * deltaTime; // Move slower when avoiding
    y += avoidDirection.y * speed * 0.5f * deltaTime;
  }

  // Update facing direction
  if (normalizedDirection.x < -0.1f)
    direction = Direction::LEFT;
  else if (normalizedDirection.x > 0.1f)
    direction = Direction::RIGHT;
}

void Bot::Wander(float deltaTime, const std::vector<Bot *> &otherBots)
{
  wanderTimer -= deltaTime;

  // Set new wander target
  if (wanderTimer <= 0.0f || Vector2Distance({x, y}, wanderTarget) < 15.0f)
  {
    int maxAttempts = 10; // Prevent infinite loop
    bool foundValidTarget = false;

    for (int attempt = 0; attempt < maxAttempts && !foundValidTarget; attempt++)
    {
      int wanderType = GetRandomValue(0, 2);
      float screenWidth = GetScreenWidth();
      float screenHeight = GetScreenHeight();

      if (wanderType == 0) // Random circular movement
      {
        float wanderDistance = GetRandomValue(100, 200);
        float angle = GetRandomValue(0, 360) * DEG2RAD;

        wanderTarget.x = x + cosf(angle) * wanderDistance;
        wanderTarget.y = y + sinf(angle) * wanderDistance;
      }
      else if (wanderType == 1) // Horizontal patrol
      {
        float patrolDistance = GetRandomValue(150, 300);
        wanderTarget.x = x + (GetRandomValue(0, 1) ? patrolDistance : -patrolDistance);
        wanderTarget.y = y + GetRandomValue(-50, 50);
      }
      else // Stay in place occasionally
      {
        wanderTarget = {x, y};
        wanderTimer = GetRandomValue(20, 40) / 10.0f;
        SetState(BotState::IDLE);
        return;
      }

      // Clamp to screen bounds
      wanderTarget.x = Clamp(wanderTarget.x, 100.0f, screenWidth - 100.0f);
      wanderTarget.y = Clamp(wanderTarget.y, 100.0f, screenHeight - 100.0f);

      // Check if target position would cause collision
      if (!WouldCollideWithBots(wanderTarget, otherBots))
      {
        foundValidTarget = true;
        wanderTimer = GetRandomValue(30, 80) / 10.0f;
      }
    }

    if (!foundValidTarget)
    {
      // If no valid target found, just stay idle
      SetState(BotState::IDLE);
      return;
    }
  }

  // Move towards wander target
  Vector2 directionToTarget = Vector2Subtract(wanderTarget, {x, y});
  float distance = Vector2Length(directionToTarget);

  if (distance > 15.0f)
  {
    Vector2 normalizedDirection = Vector2Normalize(directionToTarget);
    float wanderSpeed = speed * GetRandomValue(30, 60) / 100.0f;

    Vector2 nextPos = {
        x + normalizedDirection.x * wanderSpeed * deltaTime,
        y + normalizedDirection.y * wanderSpeed * deltaTime};

    // Check collision before moving
    if (!WouldCollideWithBots(nextPos, otherBots))
    {
      x = nextPos.x;
      y = nextPos.y;

      // Update facing direction
      if (fabsf(normalizedDirection.x) > 0.3f)
      {
        direction = (normalizedDirection.x < 0) ? Direction::LEFT : Direction::RIGHT;
      }
    }
    else
    {
      // Try to find alternative path
      Vector2 avoidDirection = GetAvoidanceDirection(nextPos, otherBots);
      x += avoidDirection.x * wanderSpeed * 0.3f * deltaTime;
      y += avoidDirection.y * wanderSpeed * 0.3f * deltaTime;
    }
  }
  else
  {
    // Reached target, decide what to do next
    if (GetRandomValue(0, 100) < 40)
    {
      SetState(BotState::IDLE);
    }
    else
    {
      wanderTimer = 0.0f; // Set new target immediately
    }
  }
}

// NEW: Collision avoidance methods
bool Bot::WouldCollideWithBots(Vector2 position, const std::vector<Bot *> &otherBots) const
{
  Rectangle thisRect = {position.x, position.y, width * 0.8f, height * 0.8f}; // Slightly smaller for better movement

  for (const Bot *otherBot : otherBots)
  {
    if (otherBot == this || !otherBot->IsAlive() || !otherBot->isSpawned)
      continue;

    Rectangle otherRect = {otherBot->x, otherBot->y, otherBot->width * 0.8f, otherBot->height * 0.8f};

    if (CheckCollisionRecs(thisRect, otherRect))
      return true;
  }

  return false;
}

Vector2 Bot::GetAvoidanceDirection(Vector2 blockedPosition, const std::vector<Bot *> &otherBots) const
{
  Vector2 avoidDirection = {0.0f, 0.0f};
  int collisionCount = 0;

  for (const Bot *otherBot : otherBots)
  {
    if (otherBot == this || !otherBot->IsAlive() || !otherBot->isSpawned)
      continue;

    Vector2 otherPos = {otherBot->x, otherBot->y};
    float distance = Vector2Distance(blockedPosition, otherPos);

    if (distance < width + 50.0f) // Within avoidance range
    {
      Vector2 awayFromOther = Vector2Subtract(blockedPosition, otherPos);
      if (Vector2Length(awayFromOther) > 0.1f) // Avoid division by zero
      {
        awayFromOther = Vector2Normalize(awayFromOther);
        avoidDirection = Vector2Add(avoidDirection, awayFromOther);
        collisionCount++;
      }
    }
  }

  if (collisionCount > 0)
  {
    avoidDirection = Vector2Scale(avoidDirection, 1.0f / collisionCount); // Average the directions
    return Vector2Normalize(avoidDirection);
  }

  // If no specific avoidance direction, try random perpendicular movement
  float randomAngle = GetRandomValue(0, 360) * DEG2RAD;
  return {cosf(randomAngle), sinf(randomAngle)};
}

// Rest of the methods remain the same but with minor fixes...

void Bot::Patrol()
{
  if (patrolWaypoints.empty() || !isSpawned)
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
    else if (newState == BotState::IDLE)
    {
      // Add some randomness to idle time
      wanderTime = GetRandomValue(20, 60) / 10.0f; // 2-6 seconds idle time
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
  return attackTimer <= 0.0f && IsAlive() && isSpawned;
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
  if (!isSpawned)
    return false;

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
  Texture2D *currentTexture = nullptr;
  Animation *currentAnim = nullptr;

  switch (state)
  {
  case BotState::IDLE:
    if (direction == Direction::RIGHT)
    {
      currentTexture = &idleTexture;
      currentAnim = &idleRightAnim;
    }
    else
    {
      currentTexture = &idleLeftTexture;
      currentAnim = &idleLeftAnim;
    }
    break;

  case BotState::WANDERING:
  case BotState::CHASING:
  case BotState::FLEEING:
    currentTexture = &walkTexture;
    currentAnim = &walkAnim;
    break;

  case BotState::ATTACK:
    currentTexture = &attackTexture;
    currentAnim = &attackAnim;
    break;

  default:
    currentTexture = &idleTexture;
    currentAnim = &idleRightAnim;
    break;
  }

  texture = *currentTexture;

  // Calculate frame dimensions dynamically from texture and animation
  int totalFrames = currentAnim->last - currentAnim->first + 1;
  int frameWidth = texture.width / totalFrames;
  int frameHeight = texture.height;

  source = animation_frame(currentAnim, frameWidth, frameHeight);

  // Flip sprite for left direction (except idle which has separate textures)
  if (direction == Direction::LEFT && state != BotState::IDLE)
  {
    source.width = -frameWidth;
    source.x += frameWidth;
  }
}

// Rendering
void Bot::Draw()
{
  if (!isLoaded)
    return;

  // Don't draw if not spawned yet (optional - you might want a spawn effect)
  if (!isSpawned)
  {
    // Optional: Draw a spawn indicator or countdown
    // DrawText(TextFormat("Spawning in: %.1f", spawnDelay - spawnTimer), x, y - 30, 20, RED);
    return;
  }

  Texture2D currentTexture;
  Rectangle source;
  GetTextureAndAnimation(currentTexture, source);

  Rectangle dest = {x, y, width, height};
  Vector2 origin = {0, 0};

  DrawTexturePro(currentTexture, source, dest, origin, 0.0f, WHITE);

  // Optional: Draw health bar for debugging
  if (health < maxHealth)
  {
    float healthPercent = (float)health / maxHealth;
    DrawRectangle(x, y - 10, width * healthPercent, 5, GREEN);
    DrawRectangle(x + width * healthPercent, y - 10, width * (1 - healthPercent), 5, RED);
  }
}