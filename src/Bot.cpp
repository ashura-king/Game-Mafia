#include "includes/Bot.hpp"
#include <algorithm>
#include <cmath>

// Static member initialization
int Bot::nextGroupId = 1;

Bot::Bot(float startX, float startY)
{
  // ... existing initialization code ...
  x = startX;
  y = startY;
  width = 32.0f;
  height = 32.0f;
  speed = 100.0f;
  direction = Direction::RIGHT;
  health = 100;
  maxHealth = 100;

  // Initialize basic AI properties
  state = BotState::IDLE;
  previousState = BotState::IDLE;
  stateTimer = 0.0f;
  chaseRange = 150.0f;
  attackRange = 50.0f;
  fleeingRange = 200.0f;
  wanderTime = 3.0f;
  wanderTimer = 0.0f;
  wanderTarget = {x, y};

  // Initialize combat properties
  isAttacking = false;
  attackTimer = 0.0f;
  attackCooldown = 1.0f;

  // Initialize spawn properties
  spawnDelay = 0.0f;
  spawnTimer = 0.0f;
  isSpawned = true;

  // Initialize direction properties
  lastValidDirection = {1.0f, 0.0f};
  directionChangeTimer = 0.0f;

  // Initialize status
  isLoaded = false;
  type = BotType::THUG;

  // Initialize E-SWAT tactical variables
  tacticalRole = TacticalRole::DIRECT_ASSAULT;
  tacticalPhase = TacticalPhase::POSITIONING;
  tacticalTarget = {0.0f, 0.0f};
  circleCenter = {0.0f, 0.0f};
  circleRadius = ESWAT_CIRCLE_RADIUS;
  circleAngle = 0.0f;
  tacticalTimer = 0.0f;
  isInPosition = false;
  waitingForSignal = false;
  groupId = nextGroupId++;
}

// E-SWAT style tactical role assignment
void Bot::AssignESWATRole(const std::vector<Bot *> &allBots, Vector2 playerPos)
{
  // Count active bots in the same group
  std::vector<Bot *> activeBots;
  for (Bot *bot : allBots)
  {
    if (bot->IsAlive() && bot->IsSpawned() &&
        (bot->GetBotType() == BotType::THUG || bot->GetBotType() == BotType::SWAT))
    {
      activeBots.push_back(bot);
    }
  }

  // Find this bot's index in active bots
  int botIndex = 0;
  for (size_t i = 0; i < activeBots.size(); i++)
  {
    if (activeBots[i] == this)
    {
      botIndex = i;
      break;
    }
  }

  int totalBots = activeBots.size();

  // E-SWAT style role distribution
  if (totalBots == 1)
  {
    tacticalRole = TacticalRole::DIRECT_ASSAULT;
  }
  else if (totalBots == 2)
  {
    tacticalRole = (botIndex == 0) ? TacticalRole::DIRECT_ASSAULT : TacticalRole::REAR_AMBUSH;
  }
  else if (totalBots == 3)
  {
    switch (botIndex)
    {
    case 0:
      tacticalRole = TacticalRole::DIRECT_ASSAULT;
      break;
    case 1:
      tacticalRole = TacticalRole::LEFT_FLANKER;
      break;
    case 2:
      tacticalRole = TacticalRole::RIGHT_FLANKER;
      break;
    }
  }
  else
  {
    // 4+ bots - full tactical deployment
    switch (botIndex % 5)
    {
    case 0:
      tacticalRole = TacticalRole::DIRECT_ASSAULT;
      break;
    case 1:
      tacticalRole = TacticalRole::LEFT_FLANKER;
      break;
    case 2:
      tacticalRole = TacticalRole::RIGHT_FLANKER;
      break;
    case 3:
      tacticalRole = TacticalRole::REAR_AMBUSH;
      break;
    case 4:
      tacticalRole = TacticalRole::SUPPORT_FIRE;
      break;
    }
  }
}

// Calculate E-SWAT tactical positions
Vector2 Bot::GetESWATPosition(Vector2 playerPos, const std::vector<Bot *> &allBots)
{
  circleCenter = playerPos;
  Vector2 targetPos = playerPos;

  switch (tacticalRole)
  {
  case TacticalRole::DIRECT_ASSAULT:
    // Position directly in front, but maintain distance
    circleAngle = 0.0f;
    targetPos.x = playerPos.x + cosf(circleAngle) * (circleRadius * 0.7f);
    targetPos.y = playerPos.y + sinf(circleAngle) * (circleRadius * 0.7f);
    break;

  case TacticalRole::LEFT_FLANKER:
    // Circle to the left side
    circleAngle = PI * 0.5f; // 90 degrees
    targetPos.x = playerPos.x + cosf(circleAngle) * circleRadius;
    targetPos.y = playerPos.y + sinf(circleAngle) * circleRadius;
    break;

  case TacticalRole::RIGHT_FLANKER:
    // Circle to the right side
    circleAngle = PI * 1.5f; // 270 degrees
    targetPos.x = playerPos.x + cosf(circleAngle) * circleRadius;
    targetPos.y = playerPos.y + sinf(circleAngle) * circleRadius;
    break;

  case TacticalRole::REAR_AMBUSH:
    // Move behind player
    circleAngle = PI; // 180 degrees
    targetPos.x = playerPos.x + cosf(circleAngle) * circleRadius;
    targetPos.y = playerPos.y + sinf(circleAngle) * circleRadius;
    break;

  case TacticalRole::SUPPORT_FIRE:
    // Stay at maximum range
    circleAngle = PI * 0.25f; // 45 degrees
    targetPos.x = playerPos.x + cosf(circleAngle) * (circleRadius * 1.5f);
    targetPos.y = playerPos.y + sinf(circleAngle) * (circleRadius * 1.5f);
    break;
  }

  return targetPos;
}

// E-SWAT coordination check
bool Bot::CheckESWATCoordination(const std::vector<Bot *> &allBots)
{
  int botsInPosition = 0;
  int totalActiveBots = 0;

  for (const Bot *bot : allBots)
  {
    if (bot->IsAlive() && bot->IsSpawned() &&
        (bot->GetBotType() == BotType::THUG || bot->GetBotType() == BotType::SWAT))
    {
      totalActiveBots++;
      if (bot->isInPosition)
      {
        botsInPosition++;
      }
    }
  }

  // Need at least 60% of bots in position for coordinated attack
  return (totalActiveBots > 1) ? (botsInPosition >= totalActiveBots * 0.6f) : true;
}

// Main E-SWAT tactical behavior
void Bot::ExecuteESWATTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  tacticalTimer += deltaTime;

  // Assign role based on current situation
  AssignESWATRole(allBots, playerPos);

  // Get tactical position
  tacticalTarget = GetESWATPosition(playerPos, allBots);

  switch (tacticalPhase)
  {
  case TacticalPhase::POSITIONING:
    ExecutePositioning(playerPos, deltaTime, allBots);
    break;

  case TacticalPhase::COORDINATED_ATTACK:
    ExecuteCoordinatedAttack(playerPos, deltaTime, allBots);
    break;

  case TacticalPhase::RETREAT_REGROUP:
    ExecuteRetreatRegroup(playerPos, deltaTime, allBots);
    break;
  }
}

// Positioning phase - move to tactical positions
void Bot::ExecutePositioning(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  float distanceToTarget = Vector2Distance({x, y}, tacticalTarget);
  isInPosition = (distanceToTarget < 50.0f);

  if (!isInPosition)
  {
    // Move to position with E-SWAT precision
    Vector2 direction = Vector2Normalize(Vector2Subtract(tacticalTarget, {x, y}));
    float positioningSpeed = speed * ESWAT_POSITIONING_SPEED;

    x += direction.x * positioningSpeed * deltaTime;
    y += direction.y * positioningSpeed * deltaTime;

    UpdateDirection(direction);
    SetState(BotState::TACTICAL_POSITIONING);
  }
  else
  {
    // Wait for coordination signal
    waitingForSignal = true;

    // Check if all units are ready for coordinated attack
    if (CheckESWATCoordination(allBots) && tacticalTimer > 1.0f)
    {
      tacticalPhase = TacticalPhase::COORDINATED_ATTACK;
      tacticalTimer = 0.0f;
    }
  }
}

// Coordinated attack phase - synchronized assault
void Bot::ExecuteCoordinatedAttack(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  SetState(BotState::COORDINATED_ATTACK);

  // Different attack patterns based on role
  switch (tacticalRole)
  {
  case TacticalRole::DIRECT_ASSAULT:
    // Charge directly at player
    if (distanceToPlayer > attackRange)
    {
      Vector2 chargeDir = Vector2Normalize(Vector2Subtract(playerPos, {x, y}));
      float chargeSpeed = speed * ESWAT_ATTACK_SPEED;
      x += chargeDir.x * chargeSpeed * deltaTime;
      y += chargeDir.y * chargeSpeed * deltaTime;
      UpdateDirection(chargeDir);
    }
    else if (CanAttack())
    {
      Attack();
    }
    break;

  case TacticalRole::LEFT_FLANKER:
  case TacticalRole::RIGHT_FLANKER:
    // Circle around player while attacking
    CircleStrikePlayer(playerPos, deltaTime);
    break;

  case TacticalRole::REAR_AMBUSH:
    // Quick strike from behind
    if (distanceToPlayer > attackRange)
    {
      Vector2 ambushDir = Vector2Normalize(Vector2Subtract(playerPos, {x, y}));
      float ambushSpeed = speed * (ESWAT_ATTACK_SPEED * 1.2f);
      x += ambushDir.x * ambushSpeed * deltaTime;
      y += ambushDir.y * ambushSpeed * deltaTime;
      UpdateDirection(ambushDir);
    }
    else if (CanAttack())
    {
      Attack();
    }
    break;

  case TacticalRole::SUPPORT_FIRE:
    // Maintain distance and provide covering fire
    if (distanceToPlayer < circleRadius * 1.2f)
    {
      // Move back to maintain distance
      Vector2 retreatDir = Vector2Normalize(Vector2Subtract({x, y}, playerPos));
      x += retreatDir.x * speed * deltaTime;
      y += retreatDir.y * speed * deltaTime;
    }
    else if (CanAttack())
    {
      Attack();
    }
    break;
  }

  // Check if attack phase should end
  if (tacticalTimer > ESWAT_COORDINATION_TIME || health < maxHealth * 0.3f)
  {
    tacticalPhase = TacticalPhase::RETREAT_REGROUP;
    tacticalTimer = 0.0f;
  }
}

// Retreat and regroup phase
void Bot::ExecuteRetreatRegroup(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  SetState(BotState::RETREATING);

  // Calculate retreat position (opposite direction from player)
  Vector2 retreatDir = Vector2Normalize(Vector2Subtract({x, y}, playerPos));
  Vector2 retreatTarget = {
      x + retreatDir.x * circleRadius,
      y + retreatDir.y * circleRadius};

  float distanceToRetreat = Vector2Distance({x, y}, retreatTarget);

  if (distanceToRetreat > 30.0f)
  {
    // Move to retreat position
    Vector2 moveDir = Vector2Normalize(Vector2Subtract(retreatTarget, {x, y}));
    x += moveDir.x * speed * deltaTime;
    y += moveDir.y * speed * deltaTime;
    UpdateDirection(moveDir);
  }

  // Reset to positioning phase after retreat
  if (tacticalTimer > 2.0f)
  {
    tacticalPhase = TacticalPhase::POSITIONING;
    tacticalTimer = 0.0f;
    isInPosition = false;
    waitingForSignal = false;
  }
}

// Circle strike movement (for flankers)
void Bot::CircleStrikePlayer(Vector2 playerPos, float deltaTime)
{
  float distanceToPlayer = Vector2Distance({x, y}, playerPos);

  if (distanceToPlayer > ESWAT_ATTACK_DISTANCE)
  {
    // Move closer while circling
    circleAngle += deltaTime * 2.0f; // Adjust circle speed

    Vector2 circlePos = {
        playerPos.x + cosf(circleAngle) * ESWAT_ATTACK_DISTANCE,
        playerPos.y + sinf(circleAngle) * ESWAT_ATTACK_DISTANCE};

    Vector2 moveDir = Vector2Normalize(Vector2Subtract(circlePos, {x, y}));
    x += moveDir.x * speed * ESWAT_ATTACK_SPEED * deltaTime;
    y += moveDir.y * speed * ESWAT_ATTACK_SPEED * deltaTime;
    UpdateDirection(moveDir);
  }
  else if (CanAttack())
  {
    Attack();
  }
}

// Implement other necessary Bot methods (these would need to be adapted from your existing Bot.cpp)
void Bot::LoadTextures()
{
  // Default implementation - override in derived classes
}

void Bot::SetProperties()
{
  // Default implementation - override in derived classes
}

BotType Bot::GetBotType() const
{
  return type;
}

void Bot::Update()
{
  // Update animations and timers
  UpdateAnimations();

  // Update spawn timer
  if (!isSpawned)
  {
    spawnTimer += GetFrameTime();
    if (spawnTimer >= spawnDelay)
    {
      isSpawned = true;
    }
  }

  // Update attack timer
  if (attackTimer > 0.0f)
  {
    attackTimer -= GetFrameTime();
  }

  // Update direction change timer
  if (directionChangeTimer > 0.0f)
  {
    directionChangeTimer -= GetFrameTime();
  }
}

void Bot::UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  // Base implementation - override in derived classes
  if (!isSpawned || !IsAlive())
    return;

  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  stateTimer += deltaTime;

  // Basic AI behavior
  if (distanceToPlayer <= attackRange && CanAttack())
  {
    SetState(BotState::ATTACK);
    Attack();
  }
  else if (distanceToPlayer < chaseRange)
  {
    SetState(BotState::CHASING);
    ChasePlayer(playerPos, otherBots);
  }
  else
  {
    SetState(BotState::WANDERING);
    Wander(deltaTime, otherBots);
  }
}

void Bot::Draw()
{
  if (!isSpawned || !IsAlive())
    return;

  Texture2D currentTexture;
  Rectangle sourceRect;
  GetTextureAndAnimation(currentTexture, sourceRect);

  Rectangle destRect = {x, y, width, height};
  Vector2 origin = {width / 2.0f, height / 2.0f};

  DrawTexturePro(currentTexture, sourceRect, destRect, origin, 0.0f, WHITE);

  // Draw health bar
  if (health < maxHealth)
  {
    float healthBarWidth = width;
    float healthBarHeight = 4.0f;
    float healthPercent = (float)health / (float)maxHealth;

    DrawRectangle(x - healthBarWidth / 2, y - height / 2 - 10, healthBarWidth, healthBarHeight, RED);
    DrawRectangle(x - healthBarWidth / 2, y - height / 2 - 10, healthBarWidth * healthPercent, healthBarHeight, GREEN);
  }
}

void Bot::ChasePlayer(Vector2 playerPos, const std::vector<Bot *> &otherBots)
{
  Vector2 direction = Vector2Normalize(Vector2Subtract(playerPos, {x, y}));

  // Check for collision avoidance
  Vector2 nextPos = {x + direction.x * speed * GetFrameTime(),
                     y + direction.y * speed * GetFrameTime()};

  if (!WouldCollideWithBots(nextPos, otherBots))
  {
    x = nextPos.x;
    y = nextPos.y;
    UpdateDirection(direction);
  }
  else
  {
    // Find alternative path
    Vector2 avoidDir = GetAvoidanceDirection(nextPos, otherBots);
    x += avoidDir.x * speed * GetFrameTime();
    y += avoidDir.y * speed * GetFrameTime();
    UpdateDirection(avoidDir);
  }
}

void Bot::Wander(float deltaTime, const std::vector<Bot *> &otherBots)
{
  wanderTimer += deltaTime;

  // Set new wander target periodically
  if (wanderTimer >= wanderTime)
  {
    wanderTarget.x = x + (GetRandomValue(-200, 200));
    wanderTarget.y = y + (GetRandomValue(-200, 200));
    wanderTimer = 0.0f;
  }

  // Move towards wander target
  float distanceToTarget = Vector2Distance({x, y}, wanderTarget);
  if (distanceToTarget > 10.0f)
  {
    Vector2 direction = Vector2Normalize(Vector2Subtract(wanderTarget, {x, y}));
    Vector2 nextPos = {x + direction.x * speed * 0.5f * deltaTime,
                       y + direction.y * speed * 0.5f * deltaTime};

    if (!WouldCollideWithBots(nextPos, otherBots))
    {
      x = nextPos.x;
      y = nextPos.y;
      UpdateDirection(direction);
    }
  }
}

void Bot::MoveTowards(Vector2 target)
{
  Vector2 direction = Vector2Normalize(Vector2Subtract(target, {x, y}));
  x += direction.x * speed * GetFrameTime();
  y += direction.y * speed * GetFrameTime();
  UpdateDirection(direction);
}

void Bot::MoveAway(Vector2 threat)
{
  Vector2 direction = Vector2Normalize(Vector2Subtract({x, y}, threat));
  x += direction.x * speed * GetFrameTime();
  y += direction.y * speed * GetFrameTime();
  UpdateDirection(direction);
}

void Bot::Patrol()
{
  if (patrolWaypoints.empty())
    return;

  Vector2 currentWaypoint = patrolWaypoints[currentWaypointIndex];
  float distanceToWaypoint = Vector2Distance({x, y}, currentWaypoint);

  if (distanceToWaypoint <= waypointReachDistance)
  {
    currentWaypointIndex = (currentWaypointIndex + 1) % patrolWaypoints.size();
  }
  else
  {
    MoveTowards(currentWaypoint);
  }
}

void Bot::Attack()
{
  if (!CanAttack())
    return;

  isAttacking = true;
  attackTimer = attackCooldown;
  SetState(BotState::ATTACK);

  // Attack animation and damage would be handled here
}

void Bot::TakeDamage(int damage)
{
  health -= damage;
  if (health <= 0)
  {
    health = 0;
    SetState(BotState::DEAD);
  }
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

bool Bot::CanAttack() const
{
  return attackTimer <= 0.0f && IsAlive() && isSpawned;
}

float Bot::DistanceTo(Vector2 target) const
{
  return Vector2Distance({x, y}, target);
}

bool Bot::IsPlayerInRange(Vector2 playerPosition, float range) const
{
  return Vector2Distance({x, y}, playerPosition) <= range;
}

bool Bot::CheckCollisionWithPlayer(Vector2 playerPos, float playerWidth, float playerHeight)
{
  Rectangle botRect = {x - width / 2, y - height / 2, width, height};
  Rectangle playerRect = {playerPos.x - playerWidth / 2, playerPos.y - playerHeight / 2, playerWidth, playerHeight};
  return CheckCollisionRecs(botRect, playerRect);
}

bool Bot::WouldCollideWithBots(Vector2 position, const std::vector<Bot *> &otherBots) const
{
  Rectangle futureRect = {position.x - width / 2, position.y - height / 2, width, height};

  for (const Bot *other : otherBots)
  {
    if (other == this || !other->IsAlive() || !other->IsSpawned())
      continue;

    Rectangle otherRect = {other->x - other->width / 2, other->y - other->height / 2,
                           other->width, other->height};

    if (CheckCollisionRecs(futureRect, otherRect))
    {
      return true;
    }
  }
  return false;
}

Vector2 Bot::GetAvoidanceDirection(Vector2 blockedPosition, const std::vector<Bot *> &otherBots) const
{
  // Simple avoidance - try perpendicular directions
  Vector2 directions[] = {
      {0, -1}, {1, 0}, {0, 1}, {-1, 0}, // Cardinal directions
      {0.707f, -0.707f},
      {0.707f, 0.707f},
      {-0.707f, 0.707f},
      {-0.707f, -0.707f} // Diagonal
  };

  for (const Vector2 &dir : directions)
  {
    Vector2 testPos = {x + dir.x * 50.0f, y + dir.y * 50.0f};
    if (!WouldCollideWithBots(testPos, otherBots))
    {
      return dir;
    }
  }

  return {0, 0}; // No clear direction found
}

void Bot::UpdateAnimations()
{
  // Update current animation frame
  // This would be implemented based on your animation system
}

void Bot::GetTextureAndAnimation(Texture2D &texture, Rectangle &source)
{
  // Default implementation - override in derived classes
  texture = idleTexture;
  source = {0, 0, (float)texture.width, (float)texture.height};
}

void Bot::UpdateDirection(Vector2 movementVector)
{
  if (directionChangeTimer > 0.0f)
    return;

  if (abs(movementVector.x) > abs(movementVector.y))
  {
    direction = (movementVector.x > 0) ? Direction::RIGHT : Direction::LEFT;
  }
  else
  {
    direction = (movementVector.y > 0) ? Direction::DOWN : Direction::UP;
  }

  lastValidDirection = movementVector;
  directionChangeTimer = DIRECTION_CHANGE_DELAY;
}

Bot::~Bot()
{
  // Cleanup textures if needed
}