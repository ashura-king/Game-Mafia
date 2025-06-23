#include "includes/Swat.hpp"
#include <algorithm>

SwatBot::SwatBot(float startX, float startY) : Bot(startX, startY)
{
  type = BotType::SWAT;
  SetProperties();
}

void SwatBot::LoadTextures()
{
  // Load SWAT-specific textures
  // idleTexture = LoadTexture("assets/swat/idle.png");
  // walkTexture = LoadTexture("assets/swat/walk.png");
  // runTexture = LoadTexture("assets/swat/run.png");
  // attackTexture = LoadTexture("assets/swat/attack.png");

  isLoaded = true;
}

void SwatBot::SetProperties()
{
  // SWAT-specific properties - more professional and coordinated
  speed = 110.0f;
  health = 120;
  maxHealth = 120;

  chaseRange = 250.0f;
  attackRange = 60.0f;
  fleeingRange = 200.0f;
  wanderTime = 4.0f;

  attackCooldown = 0.6f;
  spawnDelay = 1.0f;

  // Initialize patrol waypoints
  currentWaypointIndex = 0;
  waypointReachDistance = 25.0f;

  // Set up patrol points (example - adjust based on your map)
  patrolWaypoints.push_back({x - 100, y});
  patrolWaypoints.push_back({x + 100, y});
  patrolWaypoints.push_back({x, y - 100});
  patrolWaypoints.push_back({x, y + 100});
}

void SwatBot::UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  if (!isSpawned || !IsAlive())
    return;

  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  stateTimer += deltaTime;

  float effectiveChaseRange = chaseRange;
  if (IsNearOtherSwat(otherBots))
  {
    effectiveChaseRange *= SWAT_COORDINATION_BONUS;
  }

  // SWAT always uses professional E-SWAT tactics when engaging
  if (distanceToPlayer < effectiveChaseRange)
  {
    SetState(BotState::CHASING);
    ExecuteProfessionalTactics(playerPos, deltaTime, otherBots);
  }
  else if (distanceToPlayer < fleeingRange &&
           (health < maxHealth * SWAT_MIN_HEALTH_THRESHOLD || !IsNearOtherSwat(otherBots)))
  {
    SetState(BotState::FLEEING);
    MoveAway(playerPos);
  }
  else
  {
    // Professional patrol behavior
    if (state == BotState::IDLE && stateTimer >= wanderTime)
    {
      SetState(BotState::PATROLLING);
      wanderTimer = 0.0f;
    }
    if (state == BotState::PATROLLING)
    {
      if (!patrolWaypoints.empty())
      {
        Patrol();
      }
      else
      {
        Wander(deltaTime, otherBots);
      }
      if (stateTimer >= wanderTime * 2.0f)
      {
        SetState(BotState::IDLE);
      }
    }
  }
}

void SwatBot::Attack()
{
  if (!CanAttack())
    return;

  // SWAT uses more precise, professional attacks
  SetState(BotState::ATTACK);
  attackTimer = GetTime();

  // Add SWAT-specific attack logic here
  // For example: more accurate shots, coordinated timing, etc.

  // Call parent attack method for basic functionality
  Bot::Attack();
}

void SwatBot::ExecuteProfessionalTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  // SWAT uses enhanced E-SWAT tactics with better coordination
  ExecuteESWATTactics(playerPos, deltaTime, otherBots);
  MaintainFormation(otherBots);
}

void SwatBot::ExecuteESWATTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  // Enhanced E-SWAT tactics with professional SWAT coordination
  tacticalTimer += deltaTime;

  AssignESWATRole(allBots, playerPos);
  tacticalTarget = GetESWATPosition(playerPos, allBots);

  switch (tacticalPhase)
  {
  case TacticalPhase::POSITIONING:
    ExecuteAdvancedPositioning(playerPos, deltaTime, allBots);
    break;

  case TacticalPhase::COORDINATED_ATTACK:
    ExecuteCoordinatedAssault(playerPos, deltaTime, allBots);
    break;

  case TacticalPhase::RETREAT_REGROUP:
    ExecuteTacticalRetreat(playerPos, deltaTime, allBots);
    break;
  }
}

void SwatBot::ExecuteAdvancedPositioning(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  float distanceToTarget = Vector2Distance({x, y}, tacticalTarget);
  isInPosition = (distanceToTarget < 40.0f); // More precise than thugs

  if (!isInPosition)
  {
    Vector2 direction = Vector2Normalize(Vector2Subtract(tacticalTarget, {x, y}));
    float positioningSpeed = speed * ESWAT_POSITIONING_SPEED;

    // SWAT maintains formation while moving
    Vector2 formationAdjustment = {0, 0};
    for (const Bot *other : allBots)
    {
      if (other != this && other->IsAlive() && other->IsSpawned() &&
          other->GetBotType() == BotType::SWAT)
      {
        float distance = Vector2Distance({x, y}, {other->x, other->y});
        if (distance < SWAT_FORMATION_DISTANCE && distance > 0)
        {
          Vector2 separation = Vector2Normalize(Vector2Subtract({x, y}, {other->x, other->y}));
          formationAdjustment = Vector2Add(formationAdjustment,
                                           Vector2Scale(separation, 0.3f));
        }
      }
    }

    direction = Vector2Normalize(Vector2Add(direction, formationAdjustment));

    x += direction.x * positioningSpeed * deltaTime;
    y += direction.y * positioningSpeed * deltaTime;

    UpdateDirection(direction);
    SetState(BotState::TACTICAL_POSITIONING);
  }
  else
  {
    waitingForSignal = true;

    // SWAT waits for proper coordination
    if (CheckESWATCoordination(allBots) && tacticalTimer > 1.2f)
    {
      tacticalPhase = TacticalPhase::COORDINATED_ATTACK;
      tacticalTimer = 0.0f;
    }
  }
}

void SwatBot::ExecuteCoordinatedAssault(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  SetState(BotState::COORDINATED_ATTACK);

  // Professional SWAT assault patterns
  switch (tacticalRole)
  {
  case TacticalRole::DIRECT_ASSAULT:
    // Controlled advance
    if (distanceToPlayer > attackRange)
    {
      Vector2 chargeDir = Vector2Normalize(Vector2Subtract(playerPos, {x, y}));
      float chargeSpeed = speed * ESWAT_ATTACK_SPEED;

      // Maintain formation spacing
      Vector2 nextPos = {x + chargeDir.x * chargeSpeed * deltaTime,
                         y + chargeDir.y * chargeSpeed * deltaTime};

      if (!WouldCollideWithBots(nextPos, allBots))
      {
        x = nextPos.x;
        y = nextPos.y;
        UpdateDirection(chargeDir);
      }
    }
    else if (CanAttack())
    {
      Attack();
    }
    break;

  case TacticalRole::LEFT_FLANKER:
  case TacticalRole::RIGHT_FLANKER:
    // Precise flanking maneuvers
    CircleStrikePlayer(playerPos, deltaTime);
    break;

  case TacticalRole::REAR_AMBUSH:
    // Coordinated ambush
    if (distanceToPlayer > attackRange)
    {
      Vector2 ambushDir = Vector2Normalize(Vector2Subtract(playerPos, {x, y}));
      float ambushSpeed = speed * ESWAT_ATTACK_SPEED;
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
    // Maintain optimal range
    if (distanceToPlayer < circleRadius)
    {
      Vector2 retreatDir = Vector2Normalize(Vector2Subtract({x, y}, playerPos));
      x += retreatDir.x * speed * 0.8f * deltaTime;
      y += retreatDir.y * speed * 0.8f * deltaTime;
    }
    else if (distanceToPlayer > circleRadius * 1.3f)
    {
      Vector2 approachDir = Vector2Normalize(Vector2Subtract(playerPos, {x, y}));
      x += approachDir.x * speed * 0.6f * deltaTime;
      y += approachDir.y * speed * 0.6f * deltaTime;
    }
    else if (CanAttack())
    {
      Attack();
    }
    break;
  }

  // Professional timing
  if (tacticalTimer > ESWAT_COORDINATION_TIME || health < maxHealth * SWAT_MIN_HEALTH_THRESHOLD)
  {
    tacticalPhase = TacticalPhase::RETREAT_REGROUP;
    tacticalTimer = 0.0f;
  }
}

void SwatBot::ExecuteTacticalRetreat(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  SetState(BotState::RETREATING);

  // Calculate tactical retreat position
  Vector2 retreatDir = Vector2Normalize(Vector2Subtract({x, y}, playerPos));
  Vector2 retreatTarget = Vector2Add({x, y}, Vector2Scale(retreatDir, 150.0f));

  // Move towards retreat position while maintaining formation
  Vector2 direction = Vector2Normalize(Vector2Subtract(retreatTarget, {x, y}));
  float retreatSpeed = speed * 0.9f; // Slightly slower retreat for control

  // Apply formation adjustment during retreat
  Vector2 formationAdjustment = {0, 0};
  for (const Bot *other : allBots)
  {
    if (other != this && other->IsAlive() && other->IsSpawned() &&
        other->GetBotType() == BotType::SWAT)
    {
      float distance = Vector2Distance({x, y}, {other->x, other->y});
      if (distance < SWAT_FORMATION_DISTANCE && distance > 0)
      {
        Vector2 separation = Vector2Normalize(Vector2Subtract({x, y}, {other->x, other->y}));
        formationAdjustment = Vector2Add(formationAdjustment,
                                         Vector2Scale(separation, 0.2f));
      }
    }
  }

  direction = Vector2Normalize(Vector2Add(direction, formationAdjustment));

  x += direction.x * retreatSpeed * deltaTime;
  y += direction.y * retreatSpeed * deltaTime;
  UpdateDirection(direction);

  // Check if retreat is complete
  float distanceToPlayer = Vector2Distance({x, y}, playerPos);
  if (distanceToPlayer > SWAT_TACTICAL_RANGE || tacticalTimer > 3.0f)
  {
    // Reset tactical phase and prepare for next engagement
    tacticalPhase = TacticalPhase::POSITIONING;
    tacticalTimer = 0.0f;
    waitingForSignal = false;
    isInPosition = false;

    // Brief cooldown before re-engaging
    SetState(BotState::IDLE);
    stateTimer = 0.0f;
  }
}

void SwatBot::MaintainFormation(const std::vector<Bot *> &otherBots)
{
  // SWAT bots maintain professional spacing and formation
  for (const Bot *other : otherBots)
  {
    if (other != this && other->IsAlive() && other->IsSpawned() &&
        other->GetBotType() == BotType::SWAT)
    {
      float distance = Vector2Distance({x, y}, {other->x, other->y});

      // Maintain optimal formation distance
      if (distance < SWAT_FORMATION_DISTANCE * 0.7f)
      {
        // Too close - create separation
        Vector2 separation = Vector2Normalize(Vector2Subtract({x, y}, {other->x, other->y}));
        float separationForce = (SWAT_FORMATION_DISTANCE * 0.7f - distance) / (SWAT_FORMATION_DISTANCE * 0.7f);

        x += separation.x * separationForce * 30.0f * GetFrameTime();
        y += separation.y * separationForce * 30.0f * GetFrameTime();
      }
    }
  }
}

bool SwatBot::IsNearOtherSwat(const std::vector<Bot *> &otherBots) const
{
  int nearbySwatCount = 0;

  for (const Bot *other : otherBots)
  {
    if (other != this && other->IsAlive() && other->IsSpawned() &&
        other->GetBotType() == BotType::SWAT)
    {
      float distance = Vector2Distance({x, y}, {other->x, other->y});
      if (distance <= SWAT_COORDINATION_BONUS * 100.0f)
      {
        nearbySwatCount++;
      }
    }
  }

  return nearbySwatCount >= 1; // At least one other SWAT nearby for coordination
}