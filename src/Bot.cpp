#include "includes/Bot.hpp"
#include <algorithm>
#include <cmath>

int Bot::nextGroupId = 1;

Bot::Bot(float startX, float startY)
{
  // ... existing initialization code ...
  x = startX;
  y = startY;
  frameHeight = 128.0f;
  frameWidth = 128.0f;
  width = frameWidth;
  height = frameHeight;
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

void Bot::AssignESWATRole(const std::vector<Bot *> &allBots, Vector2 playerPos)
{
  // Count active bots in the same group
  std::vector<Bot *> activeBots;
  for (Bot *bot : allBots)
  {
    if (bot->IsAlive() && bot->IsSpawned() &&
        (bot->GetBotType() == BotType::THUG || bot->GetBotType() == BotType::GANGSTER))
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
        (bot->GetBotType() == BotType::THUG || bot->GetBotType() == BotType::GANGSTER))
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

  if (!(type == BotType::THUG || type == BotType::GANGSTER))
    return;
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
  Vector2 targetPos = GetESWATPosition(playerPos, allBots);

  // Only consider horizontal positioning
  Vector2 direction = Vector2Subtract(targetPos, {x, y});
  direction.y = 0; // Remove vertical movement

  float horizontalDistance = fabs(direction.x);

  if (horizontalDistance > 30.0f)
  {
    direction = Vector2Normalize(direction);

    Vector2 nextPos = {x + direction.x * speed * ESWAT_POSITIONING_SPEED * deltaTime, y};

    if (!WouldCollideWithBots(nextPos, allBots))
    {
      x = nextPos.x;
      isInPosition = false;
    }
  }
  else
  {
    isInPosition = true;
  }

  UpdateDirection(direction);
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
      Vector2 chargeDir = Vector2Subtract(playerPos, {x, y});
      chargeDir.y = 0;
      chargeDir = Vector2Normalize(chargeDir);
      float chargeSpeed = speed * ESWAT_ATTACK_SPEED;
      x += chargeDir.x * chargeSpeed * deltaTime;
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
      Vector2 ambushDir = Vector2Subtract(playerPos, {x, y});
      float ambushSpeed = speed * (ESWAT_ATTACK_SPEED * 1.2f);
      ambushDir.y = 0;
      ambushDir = Vector2Normalize(ambushDir);
      x += ambushDir.x * ambushSpeed * deltaTime;
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
      Vector2 retreatDir = Vector2Subtract({x, y}, playerPos);
      retreatDir.y = 0;
      retreatDir = Vector2Normalize(retreatDir);
      x += retreatDir.x * speed * deltaTime;
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
  Vector2 retreatDir = Vector2Subtract({x, y}, playerPos);
  retreatDir.y = 0; // ✨ Ignore vertical component
  retreatDir = Vector2Normalize(retreatDir);
  Vector2 retreatTarget = {
      x + retreatDir.x * circleRadius,
      y};
  float distanceToRetreat = Vector2Distance({x, y}, retreatTarget);

  if (distanceToRetreat > 30.0f)
  {

    Vector2 moveDir = Vector2Subtract(retreatTarget, {x, y});
    moveDir.y = 0;
    moveDir = Vector2Normalize(moveDir);
    x += moveDir.x * speed * deltaTime;
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
  circleCenter = playerPos;
  circleAngle += deltaTime * 2.0f; // Rotation speed

  // Calculate position on horizontal circle (ellipse)
  float horizontalRadius = circleRadius;

  Vector2 targetPos;
  targetPos.x = circleCenter.x + cos(circleAngle) * horizontalRadius;
  targetPos.y = y; // Keep current Y position

  // Move towards circle position horizontally
  Vector2 direction = Vector2Subtract(targetPos, {x, y});
  direction.y = 0;

  if (Vector2Length(direction) > 0)
  {
    direction = Vector2Normalize(direction);
    x += direction.x * speed * ESWAT_ATTACK_SPEED * deltaTime;

    UpdateDirection(direction);
  }
}

// Implement other necessary Bot methods (these would need to be adapted from your existing Bot.cpp)
void Bot::LoadTextures()
{
  idleRightAnim = {0, 0, 0, 0.0f, 0.0f, 0, AnimationType::REPEATING};
  idleLeftAnim = {0, 0, 0, 0.0f, 0.0f, 0, AnimationType::REPEATING};
  walkAnim = {0, 0, 0, 0.0f, 0.0f, 0, AnimationType::REPEATING};
  runAnim = {0, 0, 0, 0.0f, 0.0f, 0, AnimationType::REPEATING};
  attackAnim = {0, 0, 0, 0.0f, 0.0f, 0, AnimationType::ONESHOT};
};

void Bot::SetProperties()
{
  frameWidth = 128.0f;
  frameHeight = 128.0f;
  width = frameWidth;
  height = frameHeight;
}

BotType Bot::GetBotType() const
{
  return type;
}

void Bot::Update()
{

  UpdateAnimations();

  // Update spawn timer
  if (!isSpawned)
  {

    static const float GROUND_LEVEL = 270.0f; // Adjust this to match your ground level
    y = GROUND_LEVEL;

    // Keep bots within screen bounds horizontally
    const float SCREEN_MARGIN = 50.0f;
    if (x < SCREEN_MARGIN)
      x = SCREEN_MARGIN;
    if (x > GetScreenWidth() - SCREEN_MARGIN)
      x = GetScreenWidth() - SCREEN_MARGIN;
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

  const float verticalDrawOffset = 8.0f;

  // Scale factor for making sprites larger if needed
  const float SPRITE_SCALE = 1.5f; // Adjust this value to make sprites bigger/smaller

  Rectangle destRect = {
      x - (width * SPRITE_SCALE) / 2.0f,
      y - (height * SPRITE_SCALE) + verticalDrawOffset,
      width * SPRITE_SCALE,
      height * SPRITE_SCALE};

  DrawTexturePro(currentTexture, sourceRect, destRect, {0, 0}, 0.0f, WHITE);

  // Health bar (above head) - adjust position for scaled sprite
  if (health < maxHealth)
  {
    float barW = width * SPRITE_SCALE;
    float barH = 4.0f;
    float percent = (float)health / (float)maxHealth;

    float barX = x - barW / 2.0f;
    float barY = y - (height * SPRITE_SCALE) - 12.0f;

    DrawRectangle(barX, barY, barW, barH, RED);
    DrawRectangle(barX, barY, barW * percent, barH, GREEN);
  }
}

void Bot::ChasePlayer(Vector2 playerPos, const std::vector<Bot *> &otherBots)
{
  if (state != BotState::CHASING)
    return;

  Vector2 direction = Vector2Subtract(playerPos, {x, y});

  // Only consider horizontal distance
  direction.y = 0;

  float horizontalDistance = fabs(direction.x);

  if (horizontalDistance > 10.0f) // Minimum distance threshold
  {
    direction = Vector2Normalize(direction);

    // Check for bot collisions in horizontal movement
    Vector2 nextPos = {x + direction.x * speed * GetFrameTime(), y};

    if (!WouldCollideWithBots(nextPos, otherBots))
    {
      x = nextPos.x;
      // Y position stays the same
    }
    else
    {
      // If blocked, try to find alternative horizontal direction
      Vector2 avoidanceDir = GetAvoidanceDirection(nextPos, otherBots);
      avoidanceDir.y = 0; // Keep only horizontal component

      if (Vector2Length(avoidanceDir) > 0)
      {
        avoidanceDir = Vector2Normalize(avoidanceDir);
        x += avoidanceDir.x * speed * 0.5f * GetFrameTime();
      }
    }

    UpdateDirection(direction);
  }
}

void Bot::Wander(float deltaTime, const std::vector<Bot *> &otherBots)
{

  wanderTimer += deltaTime;

  // Set new horizontal wander target
  if (wanderTimer >= wanderTime || Vector2Distance({x, y}, wanderTarget) < 20.0f)
  {
    // Generate random horizontal target
    wanderTarget.x = x + GetRandomValue(-200, 200);
    wanderTarget.y = y; // Keep same Y level

    wanderTimer = 0.0f;
    wanderTime = GetRandomValue(2, 5); // Random wander duration
  }

  // Move towards horizontal target
  Vector2 direction = Vector2Subtract(wanderTarget, {x, y});
  direction.y = 0; // Remove vertical component

  if (Vector2Length(direction) > 5.0f)
  {
    direction = Vector2Normalize(direction);

    Vector2 nextPos = {x + direction.x * speed * 0.5f * deltaTime, y};

    if (!WouldCollideWithBots(nextPos, otherBots))
    {
      x = nextPos.x;
    }

    UpdateDirection(direction);
  }
}

void Bot::MoveTowards(Vector2 target)
{
  Vector2 direction = Vector2Subtract(target, {x, y});
  direction.y = 0;

  if (Vector2Length(direction) > 0)
  {
    direction = Vector2Normalize(direction);

    x += direction.x * speed * GetFrameTime();

    UpdateDirection(direction);
  }
}

void Bot::MoveAway(Vector2 threat)
{
  Vector2 direction = Vector2Subtract({x, y}, threat);
  direction.y = 0;

  if (Vector2Length(direction) > 0)
  {
    direction = Vector2Normalize(direction);
    x += direction.x * speed * GetFrameTime();

    UpdateDirection(direction);
  }
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
  switch (state)
  {
  case BotState::IDLE:
    if (direction == Direction::LEFT)
      Animation_Update(&idleLeftAnim);
    else
      Animation_Update(&idleRightAnim);
    break;

  case BotState::CHASING:
  case BotState::WANDERING:
  case BotState::RETREATING:
    Animation_Update(&walkAnim);
    break;

  case BotState::COORDINATED_ATTACK:
    Animation_Update(&runAnim);
    break;

  case BotState::ATTACK:
    Animation_Update(&attackAnim);
    break;

  default:
    break;
  }
}

void Bot::GetTextureAndAnimation(Texture2D &texture, Rectangle &source)
{

  switch (state)
  {
  case BotState::IDLE:
    texture = (direction == Direction::LEFT) ? idleLeftTexture : idleTexture;
    source = animation_frame((direction == Direction::LEFT) ? &idleLeftAnim : &idleRightAnim, frameWidth, frameHeight);
    break;

  case BotState::CHASING:
  case BotState::WANDERING:
  case BotState::RETREATING:
    texture = walkTexture;
    source = animation_frame(&walkAnim, frameWidth, frameHeight);
    break;

  case BotState::COORDINATED_ATTACK:
  case BotState::TACTICAL_POSITIONING:
    texture = runTexture;

    source = animation_frame(&runAnim, frameWidth, frameHeight);
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
  if (directionChangeTimer > 0.0f)
    return;

  if (movementVector.x != 0)
  {
    direction = (movementVector.x > 0) ? Direction::RIGHT : Direction::LEFT;
    lastValidDirection = movementVector;
    directionChangeTimer = DIRECTION_CHANGE_DELAY;
  }
}

Bot::~Bot()
{
  // Cleanup textures if needed
}