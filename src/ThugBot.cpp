#include "includes/ThugBot.hpp"
#include <algorithm>

ThugBot::ThugBot(float startX, float startY) : Bot(startX, startY)
{
  type = BotType::THUG;
  SetProperties();
}

void ThugBot::LoadTextures()
{
  idleTexture = LoadTexture("resource/thug/thugIdle.png");
  idleLeftTexture = LoadTexture("resource/thug/thugIdle.png");
  walkTexture = LoadTexture("resource/thug/thugwalk.png");
  runTexture = LoadTexture("resource/thug/thugRun.png");
  attackTexture = LoadTexture("resource/thug/thugAttack.png");

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

void ThugBot::ExecutePositioning(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  float distanceToTarget = Vector2Distance({x, y}, tacticalTarget);
  isInPosition = (distanceToTarget < 40.0f);

  if (!isInPosition)
  {
    Vector2 direction = Vector2Subtract(tacticalTarget, {x, y});
    direction.y = 0; // 🔒 Lock vertical movement
    direction = Vector2Normalize(direction);

    float positioningSpeed = speed * THUG_CHASE_SPEED_BOOST;

    // Optional: chaotic thug spacing logic
    Vector2 formationAdjustment = {0, 0};
    for (const Bot *other : allBots)
    {
      if (other != this && other->IsAlive() && other->IsSpawned() &&
          other->GetBotType() == BotType::THUG)
      {
        float distance = Vector2Distance({x, y}, {other->x, other->y});
        if (distance < THUG_PACK_DETECTION_RANGE && distance > 0)
        {
          Vector2 separation = Vector2Normalize(Vector2Subtract({x, y}, {other->x, other->y}));
          separation.y = 0; // 🔒 Keep horizontal-only
          formationAdjustment = Vector2Add(formationAdjustment,
                                           Vector2Scale(separation, 0.3f));
        }
      }
    }

    direction = Vector2Normalize(Vector2Add(direction, formationAdjustment));

    x += direction.x * positioningSpeed * deltaTime;
    UpdateDirection(direction);
    SetState(BotState::TACTICAL_POSITIONING);
  }
  else
  {
    waitingForSignal = true;
    if (CheckESWATCoordination(allBots) && tacticalTimer > 1.2f)
    {
      tacticalPhase = TacticalPhase::COORDINATED_ATTACK;
      tacticalTimer = 0.0f;
    }
  }
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

    Vector2 direction = Vector2Subtract(playerPos, {x, y});
    direction.y = 0;
    direction = Vector2Normalize(direction);

    float chaseSpeed = speed * THUG_CHASE_SPEED_BOOST;

    Vector2 nextPos = {
        x + direction.x * chaseSpeed * deltaTime,
        y // keep Y unchanged
    };

    if (!WouldCollideWithBots(nextPos, otherBots))
    {
      x = nextPos.x;
      UpdateDirection(direction);
    }
    else
    {

      Vector2 avoidDir = GetAvoidanceDirection(nextPos, otherBots);
      avoidDir.y = 0;
      avoidDir = Vector2Normalize(avoidDir);

      x += avoidDir.x * chaseSpeed * deltaTime;
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