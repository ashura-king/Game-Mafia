#include "includes/GangsterBot.hpp"

GangsterBot::GangsterBot(float startX, float startY) : Bot(startX, startY)
{
  type = BotType::GANGSTER;
  SetProperties();
  LoadTextures();
}

void GangsterBot::LoadTextures()
{
  idleTexture = LoadTexture("resource/gangster/idle.png");
  walkTexture = LoadTexture("resource/gangster/walk.png");
  runTexture = LoadTexture("resource/gangster/run.png");
  isLoaded = true;
}

void GangsterBot::SetProperties()
{
  speed = 105.0f;
  health = 100;
  maxHealth = 100;
  chaseRange = 260.0f;
  attackRange = 55.0f;
  fleeingRange = 180.0f;
  attackCooldown = 0.7f;
  spawnDelay = 1.0f;
}

void GangsterBot::UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  if (!isSpawned || !IsAlive())
    return;

  float dist = Vector2Distance({x, y}, playerPos);
  stateTimer += deltaTime;

  if (dist < chaseRange)
  {
    SetState(BotState::CHASING);
    ExecuteGangTactics(playerPos, deltaTime, allBots);
  }
  else
  {
    Wander(deltaTime, allBots);
  }
}

void GangsterBot::ExecuteESWATTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{

  ExecuteGangTactics(playerPos, deltaTime, allBots);
}

void GangsterBot::ExecuteGangTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  tacticalTimer += deltaTime;
  tacticalTarget = playerPos;

  switch (tacticalPhase)
  {
  case TacticalPhase::POSITIONING:
    PositionLikeGang(playerPos, deltaTime, allBots);
    break;
  case TacticalPhase::COORDINATED_ATTACK:
    GangAssault(playerPos, deltaTime, allBots);
    break;
  case TacticalPhase::RETREAT_REGROUP:
    GangRetreat(playerPos, deltaTime, allBots);
    break;
  }
}

void GangsterBot::PositionLikeGang(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  Vector2 dir = Vector2Subtract(tacticalTarget, {x, y});
  dir.y = 0;
  dir = Vector2Normalize(dir);

  x += dir.x * speed * deltaTime;
  UpdateDirection(dir);
  SetState(BotState::TACTICAL_POSITIONING);

  if (Vector2Distance({x, y}, tacticalTarget) < 50.0f && tacticalTimer > 1.0f)
  {
    tacticalPhase = TacticalPhase::COORDINATED_ATTACK;
    tacticalTimer = 0.0f;
  }
}

void GangsterBot::GangAssault(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  SetState(BotState::COORDINATED_ATTACK);

  Vector2 dir = Vector2Normalize(Vector2Subtract(playerPos, {x, y}));
  float dist = Vector2Distance({x, y}, playerPos);

  if (dist > attackRange)
  {
    x += dir.x * speed * 1.2f * deltaTime;
    UpdateDirection(dir);
  }
  else if (CanAttack())
  {
    Attack();
  }

  if (tacticalTimer > 4.0f || health < maxHealth * 0.3f)
  {
    tacticalPhase = TacticalPhase::RETREAT_REGROUP;
    tacticalTimer = 0.0f;
  }
}

void GangsterBot::GangRetreat(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  Vector2 dir = Vector2Normalize(Vector2Subtract({x, y}, playerPos));
  x += dir.x * speed * 0.9f * deltaTime;
  UpdateDirection(dir);
  SetState(BotState::RETREATING);

  if (tacticalTimer > 3.0f)
  {
    tacticalPhase = TacticalPhase::POSITIONING;
    tacticalTimer = 0.0f;
    waitingForSignal = false;
    isInPosition = false;
    SetState(BotState::IDLE);
  }
}

void GangsterBot::Attack()
{
  if (!CanAttack())
    return;

  SetState(BotState::ATTACK);
  attackTimer = GetTime();
  Bot::Attack();
}
