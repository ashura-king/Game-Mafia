#include "includes/Civillian.hpp"
#include <cmath>
#include <algorithm>

CivilianBot::CivilianBot(float startX, float startY) : Bot(startX, startY)
{
  type = BotType::CIVILIAN;

  frameWidth = 128.0f;
  frameHeight = 128.0f;
  width = frameWidth;
  height = frameHeight;

  LoadTextures();
  SetProperties();
}

void CivilianBot::LoadTextures()
{
  // Set texture path - note: this variable doesn't exist in base Bot class
  // You may need to add it to Bot.hpp or handle texture loading differently
  // texturePath = "assets/textures/civilian.png";
  idleTexture = LoadTexture("resource/civillian/civilIdle.png");
  idleLeftTexture = LoadTexture("resource/civillian/civilIdle2.png");
  walkTexture = LoadTexture("resource/civillian/civilWalk.png");
  runTexture = LoadTexture("resource/civillian/civilRun.png");

  idleRightAnim = {0, 10, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  idleLeftAnim = {0, 5, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  walkAnim = {0, 7, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  runAnim = {0, 7, 0, 0.1f, 0.1f, 1, AnimationType::REPEATING};

  Bot::LoadTextures();
}

void CivilianBot::SetProperties()
{

  frameWidth = 128.0f;
  frameHeight = 128.0f;
  width = frameWidth;
  height = frameHeight;

  maxHealth = 50;
  health = maxHealth;
  speed = 80.0f;
  chaseRange = 150.0f;
  // Civilian-specific behavioral flags
  // isAggressive doesn't exist in base Bot - you may need to add it
  // isAggressive = false;

  // CanAttack doesn't exist - there's a CanAttack() method but no member variable
  // CanAttack = false;

  // fleeWhenDamaged doesn't exist - you may need to add it
  // fleeWhenDamaged = true;
}

void CivilianBot::UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  // Check if civilian is near any danger
  if (IsNearDanger(playerPos, otherBots))
  {
    ExecutePanicBehavior(playerPos, deltaTime, otherBots);
  }
  else
  {

    Bot::UpdateAI(playerPos, deltaTime, otherBots);

    // Override any aggressive behaviors that might be in base class
    // currentTarget doesn't exist in base Bot - you may need to add it
    // currentTarget = nullptr;

    SetState(BotState::PATROLLING); // Use SetState method from base class
  }

  // UpdateMovement doesn't exist in base Bot - you may need to implement it
  // UpdateMovement(deltaTime);

  // Heal slowly over time if not in combat
  if (!IsNearDanger(playerPos, otherBots) && health < maxHealth)
  {
    health = std::min(maxHealth, health + (int)(5.0f * deltaTime));
  }
}

void CivilianBot::Attack()
{
  // Civilians cannot and will not attack
  // This method intentionally does nothing
  return;
}

void CivilianBot::ExecuteESWATTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots)
{
  // Civilians don't use E-SWAT tactics
  // Instead, they just panic and flee
  if (IsNearDanger(playerPos, allBots))
  {
    ExecutePanicBehavior(playerPos, deltaTime, allBots);
  }
}

void CivilianBot::PanicFlee(Vector2 threat, const std::vector<Bot *> &otherBots)
{
  // Calculate direction away from threat
  Vector2 fleeDirection = {x - threat.x, 0.0f};

  float length = std::sqrt(fleeDirection.x * fleeDirection.x);
  if (length > 0.0f)
  {
    fleeDirection.x /= length;
  }
  else
  {

    fleeDirection.x = (GetRandomValue(0, 1) == 0) ? -1.0f : 1.0f;
  }

  // Check for other civilians to avoid clustering
  for (const Bot *bot : otherBots)
  {
    if (bot == this || bot->GetBotType() != BotType::CIVILIAN)
      continue;

    // Use bot->x and bot->y directly since GetPosition() doesn't exist
    float dist = std::abs(x - bot->x);

    if (dist < 50.0f && dist > 0.0f)
    {
      // Add separation force to avoid bunching up
      float separation = (x - bot->x) / dist;
      fleeDirection.x += separation * 0.3f;
    }
  }

  // Apply panic speed boost
  float panicSpeed = speed * CIVILIAN_FLEE_SPEED_BOOST * CIVILIAN_PANIC_MULTIPLIER;

  x += fleeDirection.x * panicSpeed * GetFrameTime();

  MoveTowards(threat);
  SetState(BotState::FLEEING);
}

bool CivilianBot::IsNearDanger(Vector2 playerPos, const std::vector<Bot *> &otherBots) const
{
  // Check distance to player (assuming player is always a threat)
  float playerDist = std::sqrt(std::pow(x - playerPos.x, 2) + std::pow(y - playerPos.y, 2));
  if (playerDist <= CIVILIAN_DANGER_DETECTION_RANGE)
  {
    return true;
  }

  // Check for hostile bots nearby
  for (const Bot *bot : otherBots)
  {
    if (bot == this)
      continue;

    // Check if this bot is hostile (not another civilian)
    BotType botType = bot->GetBotType();
    if (botType == BotType::CIVILIAN)
      continue; // Other civilians aren't threats

    Vector2 botPos = {bot->x, bot->y};
    float dist = std::sqrt(std::pow(x - botPos.x, 2) + std::pow(y - botPos.y, 2));

    if (dist <= CIVILIAN_DANGER_DETECTION_RANGE)
    {
      // Additional check: is this bot currently aggressive or attacking?
      // IsAggressive() and GetCurrentTarget() don't exist in base Bot
      // You may need to add these methods or check state instead
      if (bot->GetState() == BotState::ATTACK)
      {
        return true;
      }
    }
  }

  // Check if civilian is currently damaged (indicates recent combat)
  if (health < maxHealth * 0.9f)
  {
    return true;
  }

  return false;
}

void CivilianBot::ExecutePanicBehavior(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots)
{
  // Find the closest threat to flee from
  Vector2 closestThreat = playerPos;
  float closestDist = std::sqrt(std::pow(x - playerPos.x, 2) + std::pow(y - playerPos.y, 2));

  // Check for closer hostile bots
  for (const Bot *bot : otherBots)
  {
    if (bot == this || bot->GetBotType() == BotType::CIVILIAN)
      continue;

    Vector2 botPos = {bot->x, bot->y};
    float dist = std::sqrt(std::pow(x - botPos.x, 2) + std::pow(y - botPos.y, 2));

    if (dist < closestDist && dist <= CIVILIAN_DANGER_DETECTION_RANGE)
    {
      // Check if bot is threatening (you may need to adjust this check)
      if (bot->GetState() == BotState::ATTACK)
      {
        closestThreat = botPos;
        closestDist = dist;
      }
    }
  }

  // Execute panic flee behavior
  PanicFlee(closestThreat, otherBots);

  // Play panic sound effects or animations here if your system supports it
  // PlayPanicSound();
  // SetAnimation("panic_run");

  // Reduce accuracy of any systems that might depend on civilian calmness
  // This could affect things like civilian testimony, quest completion, etc.
}