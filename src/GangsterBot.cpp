#include "includes/GangsterBot.hpp"

GangsterBot::GangsterBot(float startX, float startY) : Bot(startX, startY)
{
  type = BotType::GANGSTER;
  frameWidth = 128.0f;
  frameHeight = 128.0f;

  LoadTextures();
  SetProperties();
}

void GangsterBot::LoadTextures()
{
  idleTexture = LoadTexture("resource/gangster/gangsterIdle.png");
  idleLeftTexture = LoadTexture("resource/gangster/gangsterIdle2.png");
  walkTexture = LoadTexture("resource/gangster/gangsterWalk.png");
  runTexture = LoadTexture("resource/gangster/gangsterRun.png");
  attackTexture = LoadTexture("resource/gangster/gangsterAttack.png");

  idleRightAnim = {0, 7, 0, 0.25f, 0.25f, 1, AnimationType::REPEATING};
  idleLeftAnim = {0, 7, 0, 0.25f, 0.25f, 1, AnimationType::REPEATING};
  walkAnim = {0, 9, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  runAnim = {0, 9, 0, 0.1f, 0.1f, 1, AnimationType::REPEATING};
  attackAnim = {0, 4, 0, 0.12f, 0.12f, 1, AnimationType::ONESHOT};

  isLoaded = true;
}

void GangsterBot::SetProperties()
{
  height = frameHeight;
  width = frameWidth;
  speed = 90.0f;
  health = 100;
  maxHealth = 100;
  chaseRange = 100.0f;
  attackRange = 35.0f;
  fleeingRange = 90.0f;

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
  SetState(BotState::PATROLLING);

  if (Vector2Distance({x, y}, tacticalTarget) < 50.0f && tacticalTimer > 1.0f)
  {
    tacticalPhase = TacticalPhase::COORDINATED_ATTACK;
    tacticalTimer = 0.0f;
  }
}

void GangsterBot::GangAssault(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  SetState(BotState::ATTACK);

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
