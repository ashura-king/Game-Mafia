#include "includes/ThugBot.hpp"
#include <algorithm>

ThugBot::ThugBot(float startX, float startY) : Bot(startX, startY)
{
  type = BotType::THUG;
  SetProperties();
}

void ThugBot::LoadTextures()
{
  // Load thug-specific textures
  // idleTexture = LoadTexture("assets/thug/idle.png");
  // walkTexture = LoadTexture("assets/thug/walk.png");
  // runTexture = LoadTexture("assets/thug/run.png");
  // attackTexture = LoadTexture("assets/thug/attack.png");

  isLoaded = true;
}

void ThugBot::SetProperties()
{
  // Thug-specific properties
  speed = 90.0f;
  health = 80;
  maxHealth = 80;

  chaseRange = 180.0f;
  attackRange = 45.0f;
  fleeingRange = 150.0f;
  wanderTime = 2.5f;

  attackCooldown = 0.8f;
  spawnDelay = 0.5f;

  // Initialize patrol waypoints if needed
  currentWaypointIndex = 0;
  waypointReachDistance = 30.0f;
}

void ThugBot::UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  if (!isSpawned || !IsAlive())
    return;

  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  stateTimer += deltaTime;

  float aggressiveChaseRange = chaseRange * THUG_AGGRESSION_MULTIPLIER;

  // Count nearby thugs for pack behavior
  int nearbyThugs = CountNearbyThugs(otherBots);

  // Use pack tactics when multiple thugs are present
  if (nearbyThugs >= THUG_MIN_PACK_SIZE - 1 && distanceToPlayer < aggressiveChaseRange)
  {
    ExecutePackTactics(playerPos, deltaTime, otherBots);
  }
  else
  {
    ExecuteIndividualBehavior(playerPos, deltaTime, otherBots);
  }
}

int ThugBot::CountNearbyThugs(const std::vector<Bot *> &otherBots) const
{
  int count = 0;
  for (const Bot *bot : otherBots)
  {
    if (bot != this && bot->IsAlive() && bot->IsSpawned() &&
        bot->GetBotType() == BotType::THUG)
    {
      if (Vector2Distance({x, y}, {bot->x, bot->y}) < THUG_PACK_DETECTION_RANGE)
      {
        count++;
      }
    }
  }
  return count;
}

void ThugBot::ExecutePackTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  // Use E-SWAT tactics when in pack formation
  SetState(BotState::CHASING);
  ExecuteESWATTactics(playerPos, deltaTime, otherBots);
}

void ThugBot::ExecuteIndividualBehavior(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  float aggressiveChaseRange = chaseRange * THUG_AGGRESSION_MULTIPLIER;

  if (distanceToPlayer <= attackRange && CanAttack())
  {
    SetState(BotState::ATTACK);
    Attack();
  }
  else if (distanceToPlayer < aggressiveChaseRange)
  {
    SetState(BotState::CHASING);

    // Enhanced chase behavior with aggression boost
    Vector2 direction = Vector2Normalize(Vector2Subtract(playerPos, {x, y}));
    float chaseSpeed = speed * THUG_CHASE_SPEED_BOOST;

    Vector2 nextPos = {x + direction.x * chaseSpeed * deltaTime,
                       y + direction.y * chaseSpeed * deltaTime};

    if (!WouldCollideWithBots(nextPos, otherBots))
    {
      x = nextPos.x;
      y = nextPos.y;
      UpdateDirection(direction);
    }
    else
    {
      // Aggressive avoidance - thugs are more likely to push through
      Vector2 avoidDir = GetAvoidanceDirection(nextPos, otherBots);
      x += avoidDir.x * chaseSpeed * deltaTime;
      y += avoidDir.y * chaseSpeed * deltaTime;
      UpdateDirection(avoidDir);
    }
  }
  else if (distanceToPlayer < fleeingRange && health < maxHealth * 0.15f)
  {
    SetState(BotState::FLEEING);
    MoveAway(playerPos);
  }
  else
  {
    // Default wandering behavior
    if (state == BotState::IDLE && stateTimer >= wanderTime * 0.8f)
    {
      SetState(BotState::WANDERING);
      wanderTimer = 0.0f;
    }
    if (state == BotState::WANDERING)
    {
      Wander(deltaTime, otherBots);
      if (stateTimer >= wanderTime * 1.5f)
      {
        SetState(BotState::IDLE);
      }
    }
  }
}

void ThugBot::ExecuteESWATTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  // Override base E-SWAT tactics with thug-specific aggressive behavior
  tacticalTimer += deltaTime;

  // Assign role based on current situation
  AssignESWATRole(allBots, playerPos);

  // Get tactical position with reduced coordination time (thugs are more chaotic)
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

void ThugBot::Attack()
{
  if (!CanAttack())
    return;

  isAttacking = true;
  attackTimer = attackCooldown;
  SetState(BotState::ATTACK);

  // Thug-specific attack behavior - more damage but longer cooldown
  // Deal damage to player if in range
  // PlaySound(thugAttackSound); // If you have sound effects
}