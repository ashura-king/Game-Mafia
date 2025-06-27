#include "includes/Bot.hpp"
#include <algorithm>
#include <cmath>
#include <random>

Bot::Bot(float spawnX, float spawnY, BotType botType)
{
  x = spawnX;
  y = spawnY;
  width = 64.0f;
  height = 64.0f;
  frameWidth = 64.0f;
  frameHeight = 64.0f;

  // Bot type and state
  type = botType;
  state = BotState::SPAWNING;
  stateTimer = 0.0f;

  // Health and basic stats
  health = 100;
  maxHealth = 100;
  speed = WALK_SPEED;
  facing = Direction::RIGHT;

  // Combat properties
  isStunned = false;
  isKnockedOut = false;
  attackCooldown = 0.0f;
  attackTimer = 0.0f;
  stunTimer = 0.0f;
  knockdownTimer = 0.0f;
  idleTimer = 0.0f;

  // Spawn system
  spawnPoint = {spawnX, spawnY};
  targetPosition = {spawnX, spawnY};
  spawnTimer = 0.0f;
  isOnScreen = false;
  hasEnteredCombat = false;

  // Combat AI parameters
  detectionRange = DETECTION_RANGE;
  attackRange = ATTACK_RANGE;
  shootRange = SHOOT_RANGE;
  alertTime = 0.0f;

  // Animation
  animTimer = 0.0f;
  currentFrame = 0;
  maxFrames = 4;

  // Combat behavior
  isAggressive = false;
  playerSpotted = false;
  lastKnownPlayerPos = {0, 0};
  aggroLevel = 0.0f;
  comboCount = 0;
  comboTimer = 0.0f;

  // Pacing behavior
  paceStartPos = {x - PACE_DISTANCE, y};
  paceEndPos = {x + PACE_DISTANCE, y};
  pacingRight = true;
  paceDistance = PACE_DISTANCE;

  // Initialize by type
  InitializeByType();
}

Bot::~Bot()
{
  // Cleanup textures
  if (idleTexture.id > 0)
    UnloadTexture(idleTexture);
  if (walkTexture.id > 0)
    UnloadTexture(walkTexture);
  if (runTexture.id > 0)
    UnloadTexture(runTexture);
  if (punchTexture.id > 0)
    UnloadTexture(punchTexture);
  if (kickTexture.id > 0)
    UnloadTexture(kickTexture);
  if (grabTexture.id > 0)
    UnloadTexture(grabTexture);
  if (shootTexture.id > 0)
    UnloadTexture(shootTexture);
  if (throwTexture.id > 0)
    UnloadTexture(throwTexture);
  if (blockTexture.id > 0)
    UnloadTexture(blockTexture);
  if (hurtTexture.id > 0)
    UnloadTexture(hurtTexture);
  if (knockdownTexture.id > 0)
    UnloadTexture(knockdownTexture);
  if (deathTexture.id > 0)
    UnloadTexture(deathTexture);
}

void Bot::InitializeByType()
{
  switch (type)
  {
  case BotType::STREET_THUG:
    health = maxHealth = 80;
    speed = WALK_SPEED;
    punchDamage = 15;
    kickDamage = 20;
    grabDamage = 12;
    shootDamage = 0;
    throwDamage = 0;
    blockChance = 0.2f;
    counterAttackChance = 0.1f;
    isAggressive = true;
    break;

  case BotType::SHOOTER:
    health = maxHealth = 60;
    speed = WALK_SPEED * 0.8f;
    punchDamage = 10;
    kickDamage = 12;
    grabDamage = 8;
    shootDamage = 25;
    throwDamage = 0;
    blockChance = 0.1f;
    counterAttackChance = 0.05f;
    isAggressive = false;
    break;

  case BotType::BRAWLER:
    health = maxHealth = 120;
    speed = WALK_SPEED * 1.2f;
    punchDamage = 20;
    kickDamage = 25;
    grabDamage = 30;
    shootDamage = 0;
    throwDamage = 0;
    blockChance = 0.3f;
    counterAttackChance = 0.2f;
    isAggressive = true;
    break;

  case BotType::HEAVY:
    health = maxHealth = 200;
    speed = WALK_SPEED * 0.6f;
    punchDamage = 35;
    kickDamage = 40;
    grabDamage = 45;
    shootDamage = 0;
    throwDamage = 0;
    blockChance = 0.4f;
    counterAttackChance = 0.15f;
    isAggressive = true;
    break;

  case BotType::THROWER:
    health = maxHealth = 70;
    speed = WALK_SPEED;
    punchDamage = 12;
    kickDamage = 15;
    grabDamage = 10;
    shootDamage = 0;
    throwDamage = 20;
    blockChance = 0.15f;
    counterAttackChance = 0.1f;
    isAggressive = false;
    break;

  case BotType::RUSHER:
    health = maxHealth = 50;
    speed = RUN_SPEED;
    punchDamage = 18;
    kickDamage = 22;
    grabDamage = 15;
    shootDamage = 0;
    throwDamage = 0;
    blockChance = 0.1f;
    counterAttackChance = 0.05f;
    isAggressive = true;
    break;
  }
}

void Bot::LoadTextures()
{
  // Base implementation - load placeholder textures
  // These would be overridden in specialized bot classes
  idleTexture = LoadTexture("assets/bots/idle.png");
  walkTexture = LoadTexture("assets/bots/walk.png");
  runTexture = LoadTexture("assets/bots/run.png");
  punchTexture = LoadTexture("assets/bots/punch.png");
  kickTexture = LoadTexture("assets/bots/kick.png");
  grabTexture = LoadTexture("assets/bots/grab.png");
  shootTexture = LoadTexture("assets/bots/shoot.png");
  throwTexture = LoadTexture("assets/bots/throw.png");
  blockTexture = LoadTexture("assets/bots/block.png");
  hurtTexture = LoadTexture("assets/bots/hurt.png");
  knockdownTexture = LoadTexture("assets/bots/knockdown.png");
  deathTexture = LoadTexture("assets/bots/death.png");

  // Initialize animations
  idleAnim = {0, 4, 0, 0.2f, 0.0f, 0, AnimationType::REPEATING};
  walkAnim = {0, 6, 0, 0.15f, 0.0f, 0, AnimationType::REPEATING};
  runAnim = {0, 8, 0, 0.1f, 0.0f, 0, AnimationType::REPEATING};
  punchAnim = {0, 4, 0, 0.1f, 0.0f, 0, AnimationType::ONESHOT};
  kickAnim = {0, 4, 0, 0.12f, 0.0f, 0, AnimationType::ONESHOT};
  grabAnim = {0, 6, 0, 0.08f, 0.0f, 0, AnimationType::ONESHOT};
  shootAnim = {0, 3, 0, 0.15f, 0.0f, 0, AnimationType::ONESHOT};
  throwAnim = {0, 5, 0, 0.1f, 0.0f, 0, AnimationType::ONESHOT};
  blockAnim = {0, 2, 0, 0.2f, 0.0f, 0, AnimationType::REPEATING};
  hurtAnim = {0, 3, 0, 0.1f, 0.0f, 0, AnimationType::ONESHOT};
  knockdownAnim = {0, 6, 0, 0.15f, 0.0f, 0, AnimationType::ONESHOT};
  deathAnim = {0, 8, 0, 0.12f, 0.0f, 0, AnimationType::ONESHOT};
}

void Bot::Update(float deltaTime)
{
  if (!IsAlive())
    return;
  stateTimer += deltaTime;
  animTimer += deltaTime;

  if (attackCooldown > 0)
    attackCooldown -= deltaTime;
  if (stunTimer > 0)
    stunTimer -= deltaTime;
  if (knockdownTimer > 0)
    knockdownTimer -= deltaTime;
  if (comboTimer > 0)
    comboTimer -= deltaTime;

  // Update status effects
  if (stunTimer <= 0)
    isStunned = false;
  if (knockdownTimer <= 0)
    isKnockedOut = false;

  // Update animation
  UpdateAnimation(deltaTime);

  // Check screen bounds
  CheckScreenBounds();
}

void Bot::UpdateAI(Vector2 playerPos, float deltaTime)
{
  if (!IsAlive() || isStunned || isKnockedOut)
    return;

  float distanceToPlayer = GetDistanceToPlayer(playerPos);

  // Update player detection
  if (CanSeePlayer(playerPos) && distanceToPlayer <= detectionRange)
  {
    playerSpotted = true;
    lastKnownPlayerPos = playerPos;
    alertTime = 3.0f; // Stay alert for 3 seconds
  }

  if (alertTime > 0)
    alertTime -= deltaTime;
  if (alertTime <= 0)
    playerSpotted = false;

  // Main AI state machine
  switch (state)
  {
  case BotState::SPAWNING:
    SpawnBehavior(deltaTime);
    break;

  case BotState::IDLE:
    IdleBehavior(deltaTime);
    break;

  case BotState::DIRECT_COMBAT:
    DirectCombatBehavior(playerPos, deltaTime);
    break;

  case BotState::RANGED_COMBAT:
    RangedCombatBehavior(playerPos, deltaTime);
    break;

  case BotState::STUNNED:
  case BotState::KNOCKED_DOWN:
  case BotState::DEAD:
    // These states are handled by timers
    break;
  }

  // State transitions
  if (playerSpotted && state == BotState::IDLE)
  {
    if (type == BotType::SHOOTER || type == BotType::THROWER)
    {
      SetStateWithTimer(BotState::RANGED_COMBAT);
    }
    else
    {
      SetStateWithTimer(BotState::DIRECT_COMBAT);
    }
  }
  else if (!playerSpotted && (state == BotState::DIRECT_COMBAT || state == BotState::RANGED_COMBAT))
  {
    SetStateWithTimer(BotState::IDLE);
  }
}

void Bot::SpawnBehavior(float deltaTime)
{
  spawnTimer += deltaTime;

  // Simple spawn-in animation or delay
  if (spawnTimer >= 1.0f)
  {
    SetStateWithTimer(BotState::IDLE);
    isOnScreen = true;
    PlaySpawnSound();
  }
}

void Bot::DirectCombatBehavior(Vector2 playerPos, float deltaTime)
{
  float distance = GetDistanceToPlayer(playerPos);

  if (distance <= attackRange && CanAttack())
  {
    StopAndAttack(playerPos);
  }
  else if (distance > attackRange)
  {
    RunTowardPlayer(playerPos);
  }

  if (GetRandomValue(0, 1000) < 5)
  {
    CallForGang();
  }
}

void Bot::RangedCombatBehavior(Vector2 playerPos, float deltaTime)
{
  float distance = GetDistanceToPlayer(playerPos);

  if (distance <= shootRange && distance > attackRange && CanAttack())
  {
    if (type == BotType::SHOOTER)
    {
      Shoot(playerPos);
    }
    else if (type == BotType::THROWER)
    {
      ThrowWeapon(playerPos);
    }
  }
  else if (distance > shootRange)
  {
    RunTowardPlayer(playerPos);
  }
  else if (distance <= attackRange)
  {
    Vector2 awayDir = Vector2Subtract({x, y}, playerPos);
    awayDir = Vector2Normalize(awayDir);
    x += awayDir.x * speed * deltaTime;
    facing = (awayDir.x > 0) ? Direction::RIGHT : Direction::LEFT;
  }
}

void Bot::IdleBehavior(float deltaTime)
{
  idleTimer += deltaTime;

  if (idleTimer >= 2.0f)
  {
    PaceAround();
    idleTimer = 0.0f;
  }
}

void Bot::RunTowardPlayer(Vector2 playerPos)
{
  Vector2 direction = Vector2Subtract(playerPos, {x, y});
  direction = Vector2Normalize(direction);

  x += direction.x * speed * GetFrameTime();

  facing = (direction.x > 0) ? Direction::RIGHT : Direction::LEFT;
}

void Bot::StopAndAttack(Vector2 playerPos)
{
  if (!CanAttack())
    return;

  switch (type)
  {
  case BotType::STREET_THUG:
  case BotType::RUSHER:
    if (GetRandomValue(0, 1))
      Punch(playerPos);
    else
      Kick(playerPos);
    break;

  case BotType::BRAWLER:
    if (GetRandomValue(0, 2) == 0)
      Grab(playerPos);
    else if (GetRandomValue(0, 1))
      Punch(playerPos);
    else
      Kick(playerPos);
    break;

  case BotType::HEAVY:
    Punch(playerPos);
    break;

  default:
    Punch(playerPos);
    break;
  }
}

void Bot::PaceAround()
{
  if (pacingRight)
  {
    x += speed * 0.5f * GetFrameTime();
    if (x >= paceEndPos.x)
    {
      pacingRight = false;
      facing = Direction::LEFT;
    }
    else
    {
      facing = Direction::RIGHT;
    }
  }
  else
  {
    x -= speed * 0.5f * GetFrameTime();
    if (x <= paceStartPos.x)
    {
      pacingRight = true;
      facing = Direction::RIGHT;
    }
    else
    {
      facing = Direction::LEFT;
    }
  }
}

void Bot::CallForGang()
{

  aggroLevel += 0.5f;
  PlaySpawnSound();
}

// Combat methods
void Bot::Punch(Vector2 targetPos)
{
  if (!CanAttack())
    return;

  attackCooldown = 1.0f;
  PlayPunchSound();
  CreateHitEffect();

  // Deal damage if in range
  if (GetDistanceToPlayer(targetPos) <= attackRange)
  {
    // Damage would be applied to player here
  }
}

void Bot::Kick(Vector2 targetPos)
{
  if (!CanAttack())
    return;

  attackCooldown = 1.2f;
  PlayKickSound();
  CreateHitEffect();

  if (GetDistanceToPlayer(targetPos) <= attackRange)
  {
    // Deal kick damage
  }
}

void Bot::Grab(Vector2 targetPos)
{
  if (!CanAttack())
    return;

  attackCooldown = 1.5f;
  CreateHitEffect();

  if (GetDistanceToPlayer(targetPos) <= attackRange)
  {
    // Deal grab damage and potentially stun player
  }
}

void Bot::Shoot(Vector2 targetPos)
{
  if (!CanAttack() || type != BotType::SHOOTER)
    return;

  attackCooldown = 2.0f;
  PlayShootSound();
  CreateMuzzleFlash();

  if (GetDistanceToPlayer(targetPos) <= shootRange)
  {
    // Deal ranged damage
  }
}

void Bot::ThrowWeapon(Vector2 targetPos)
{
  if (!CanAttack() || type != BotType::THROWER)
    return;

  attackCooldown = 1.8f;
  PlayThrowSound();

  if (GetDistanceToPlayer(targetPos) <= shootRange)
  {
    // Deal thrown weapon damage
  }
}

void Bot::Block()
{
  if (GetRandomValue(0, 100) < (int)(blockChance * 100))
  {
    // Block incoming attack
    isStunned = false;

    // Chance for counter-attack
    if (GetRandomValue(0, 100) < (int)(counterAttackChance * 100))
    {
      attackCooldown = 0.5f; // Quick counter
    }
  }
}

void Bot::TakeDamage(int damage)
{
  if (!Block())
  {
    health -= damage;
    PlayHurtSound();

    if (health <= 0)
    {
      Die();
    }
    else if (GetRandomValue(0, 100) < 30)
    {
      isStunned = true;
      stunTimer = 1.0f;
      SetStateWithTimer(BotState::STUNNED, 1.0f);
    }
  }
}

void Bot::GetKnockedDown()
{
  isKnockedOut = true;
  knockdownTimer = 3.0f;
  SetStateWithTimer(BotState::KNOCKED_DOWN, 3.0f);
  PlayKnockdownSound();
}

void Bot::Die()
{
  health = 0;
  SetStateWithTimer(BotState::DEAD);
}

void Bot::Draw()
{
  if (!isOnScreen || !IsAlive())
    return;

  Texture2D currentTexture = idleTexture;
  Rectangle sourceRect = {0, 0, frameWidth, frameHeight};

  switch (state)
  {
  case BotState::IDLE:
    currentTexture = idleTexture;
    break;
  case BotState::DIRECT_COMBAT:
  case BotState::RANGED_COMBAT:
    currentTexture = runTexture;
    break;
  case BotState::STUNNED:
    currentTexture = hurtTexture;
    break;
  case BotState::KNOCKED_DOWN:
    currentTexture = knockdownTexture;
    break;
  case BotState::DEAD:
    currentTexture = deathTexture;
    break;
  }

  sourceRect.x = currentFrame * frameWidth;
  if (facing == Direction::LEFT)
  {
    sourceRect.width = -frameWidth;
  }
  else
  {
    sourceRect.width = frameWidth;
  }

  Rectangle destRect = {x - width / 2, y - height / 2, width, height};

  DrawTexturePro(currentTexture, sourceRect, destRect, {0, 0}, 0.0f, WHITE);

  if (health < maxHealth)
  {
    float barWidth = width;
    float barHeight = 4.0f;
    float healthPercent = (float)health / (float)maxHealth;

    Rectangle bgRect = {x - barWidth / 2, y - height / 2 - 10, barWidth, barHeight};
    Rectangle healthRect = {x - barWidth / 2, y - height / 2 - 10, barWidth * healthPercent, barHeight};

    DrawRectangleRec(bgRect, RED);
    DrawRectangleRec(healthRect, GREEN);
  }
}

bool Bot::IsOnScreen() const
{
  return (x >= -width && x <= GetScreenWidth() + width &&
          y >= -height && y <= GetScreenHeight() + height);
}

bool Bot::CanSeePlayer(Vector2 playerPos)
{
  float distance = GetDistanceToPlayer(playerPos);
  return distance <= detectionRange;
}

float Bot::GetDistanceToPlayer(Vector2 playerPos)
{
  return Vector2Distance({x, y}, playerPos);
}

void Bot::SetStateWithTimer(BotState newState, float duration)
{
  state = newState;
  stateTimer = 0.0f;
  if (duration > 0)
    stateTimer = -duration;
}

void Bot::UpdateAnimation(float deltaTime)
{
  animTimer += deltaTime;

  if (animTimer >= 0.1f)
  {
    currentFrame = (currentFrame + 1) % maxFrames;
    animTimer = 0.0f;
  }
}

void Bot::CheckScreenBounds()
{
  isOnScreen = IsOnScreen();
  if (x < -100)
    x = -100;
  if (x > GetScreenWidth() + 100)
    x = GetScreenWidth() + 100;
}

void Bot::PlaySpawnSound() { /* PlaySound(spawnSound); */ }
void Bot::PlayPunchSound() { /* PlaySound(punchSound); */ }
void Bot::PlayKickSound() { /* PlaySound(kickSound); */ }
void Bot::PlayShootSound() { /* PlaySound(shootSound); */ }
void Bot::PlayThrowSound() { /* PlaySound(throwSound); */ }
void Bot::PlayHurtSound() { /* PlaySound(hurtSound); */ }
void Bot::PlayKnockdownSound() { /* PlaySound(knockdownSound); */ }

// Effect placeholder implementations
void Bot::CreateHitEffect() { /* Create particle effect */ }
void Bot::CreateMuzzleFlash() { /* Create muzzle flash effect */ }

StreetThugBot::StreetThugBot(float spawnX, float spawnY) : Bot(spawnX, spawnY, BotType::STREET_THUG)
{
  // Street thug specific initialization
  isAggressive = true;
  speed = WALK_SPEED * 1.1f;
}

void StreetThugBot::UpdateAI(Vector2 playerPos, float deltaTime)
{

  Bot::UpdateAI(playerPos, deltaTime);

  if (playerSpotted && GetDistanceToPlayer(playerPos) > attackRange)
  {
    speed = RUN_SPEED * 0.8f;
  }
}

void StreetThugBot::LoadTextures()
{
  Bot::LoadTextures();
}

ShooterBot::ShooterBot(float spawnX, float spawnY) : Bot(spawnX, spawnY, BotType::SHOOTER)
{

  isAggressive = false;
  detectionRange = SHOOT_RANGE;
}

void ShooterBot::UpdateAI(Vector2 playerPos, float deltaTime)
{
  Bot::UpdateAI(playerPos, deltaTime);
  float distance = GetDistanceToPlayer(playerPos);
  if (distance < shootRange * 0.7f && playerSpotted)
  {
    Vector2 awayDir = Vector2Subtract({x, y}, playerPos);
    awayDir = Vector2Normalize(awayDir);
    x += awayDir.x * speed * deltaTime;
  }
}

void ShooterBot::LoadTextures()
{
  Bot::LoadTextures();
}

void ShooterBot::Shoot(Vector2 targetPos)
{
  Bot::Shoot(targetPos);
}

BrawlerBot::BrawlerBot(float spawnX, float spawnY) : Bot(spawnX, spawnY, BotType::BRAWLER)
{
  isAggressive = true;
  attackRange = ATTACK_RANGE * 1.2f;
}

void BrawlerBot::UpdateAI(Vector2 playerPos, float deltaTime)
{
  Bot::UpdateAI(playerPos, deltaTime);

  if (playerSpotted && GetDistanceToPlayer(playerPos) <= attackRange && CanAttack())
  {
    if (GetRandomValue(0, 100) < 60)
    {
      Grab(playerPos);
    }
  }
}

void BrawlerBot::LoadTextures()
{
  Bot::LoadTextures();
}

void BrawlerBot::Grab(Vector2 targetPos)
{
  Bot::Grab(targetPos);
}

HeavyBot::HeavyBot(float spawnX, float spawnY) : Bot(spawnX, spawnY, BotType::HEAVY)
{
  speed = WALK_SPEED * 0.6f;
  width = frameWidth * 1.5f;
  height = frameHeight * 1.5f;
}

void HeavyBot::UpdateAI(Vector2 playerPos, float deltaTime)
{
  Bot::UpdateAI(playerPos, deltaTime);

  if (playerSpotted && GetDistanceToPlayer(playerPos) <= attackRange && CanAttack())
  {
    Punch(playerPos);
  }
}

void HeavyBot::LoadTextures()
{
  Bot::LoadTextures();
  // Load heavy bot textures
}

void HeavyBot::Punch(Vector2 targetPos)
{
  Bot::Punch(targetPos);
  attackCooldown = 2.0f; // Slower but more powerful
}

ThrowerBot::ThrowerBot(float spawnX, float spawnY) : Bot(spawnX, spawnY, BotType::THROWER)
{
  detectionRange = SHOOT_RANGE;
  isAggressive = false;
}

void ThrowerBot::UpdateAI(Vector2 playerPos, float deltaTime)
{
  Bot::UpdateAI(playerPos, deltaTime);

  float distance = GetDistanceToPlayer(playerPos);
  if (distance <= shootRange && distance > attackRange * 2 && CanAttack())
  {
    ThrowWeapon(playerPos);
  }
}

void ThrowerBot::LoadTextures()
{
  Bot::LoadTextures();
  // Load thrower textures
}

void ThrowerBot::ThrowWeapon(Vector2 targetPos)
{
  Bot::ThrowWeapon(targetPos);
  // Create projectile object
}

RusherBot::RusherBot(float spawnX, float spawnY) : Bot(spawnX, spawnY, BotType::RUSHER)
{
  // Rusher specific initialization
  speed = RUN_SPEED;
  isAggressive = true;
}

void RusherBot::UpdateAI(Vector2 playerPos, float deltaTime)
{
  Bot::UpdateAI(playerPos, deltaTime);

  // Rushers always run at full speed when they spot player
  if (playerSpotted)
  {
    speed = RUN_SPEED;
  }
}

void RusherBot::LoadTextures()
{
  Bot::LoadTextures();
  // Load rusher textures
}

void RusherBot::RunTowardPlayer(Vector2 playerPos)
{
  // Enhanced rushing with hit-and-run tactics
  Bot::RunTowardPlayer(playerPos);

  // Quick attack and retreat
  if (GetDistanceToPlayer(playerPos) <= attackRange && CanAttack())
  {
    Punch(playerPos);
    // Quick retreat after attack
    Vector2 retreatDir = Vector2Subtract({x, y}, playerPos);
    retreatDir = Vector2Normalize(retreatDir);
    x += retreatDir.x * speed * 0.5f * GetFrameTime();
  }
}

// Bot Spawner Implementation
BotSpawner::BotSpawner()
{
  encounterTimer = 0.0f;
  encounterRate = 5.0f; // Spawn every 5 seconds
  maxActiveBots = 6;
  aggressionLevel = 1.0f;
  randomEncounters = true;
}

BotSpawner::~BotSpawner()
{
  ClearAllBots();
}

void BotSpawner::Update(float deltaTime, Vector2 playerPos)
{
  encounterTimer += deltaTime;

  // Clean up defeated bots
  CleanupDefeatedBots();

  // Spawn new encounters
  if (randomEncounters && encounterTimer >= encounterRate)
  {
    // Reset encounter timer
    encounterTimer = 0.0f;

    // Only spawn if we haven't reached max bot limit
    if (activeBots.size() < maxActiveBots)
    {
      // Determine encounter type based on aggression level
      float encounterRoll = GetRandomValue(0, 100) / 100.0f;

      if (encounterRoll < 0.1f)
      {
        SpawnMixedEncounter();
      }
      else if (encounterRoll < 0.3f) // 20% chance for squad encounter
      {
        int squadSize = GetRandomValue(2, 4);
        BotType squadType = GetRandomBotType();
        SpawnSquad(squadSize, squadType);
      }
      else
      {
        BotType singleBotType = GetRandomBotType();
        SpawnBot(singleBotType);
      }
    }
  }

  for (Bot *bot : activeBots)
  {
    if (bot != nullptr)
    {
      bot->Update(deltaTime);
      bot->UpdateAI(playerPos, deltaTime);
    }
  }

  float baseRate = 5.0f;
  encounterRate = baseRate / (1.0f + aggressionLevel);

  if (encounterRate < 1.0f)
  {
    encounterRate = 1.0f;
  }
}

void BotSpawner::SpawnBot(BotType type)
{
  Vector2 spawnPos = GetSpawnPosition();
  Bot *newBot = nullptr;

  switch (type)
  {
  case BotType::STREET_THUG:
    newBot = new StreetThugBot(spawnPos.x, spawnPos.y);
    break;
  case BotType::SHOOTER:
    newBot = new ShooterBot(spawnPos.x, spawnPos.y);
    break;
  case BotType::BRAWLER:
    newBot = new BrawlerBot(spawnPos.x, spawnPos.y);
    break;
  case BotType::HEAVY:
    newBot = new HeavyBot(spawnPos.x, spawnPos.y);
    break;
  case BotType::THROWER:
    newBot = new ThrowerBot(spawnPos.x, spawnPos.y);
    break;
  case BotType::RUSHER:
    newBot = new RusherBot(spawnPos.x, spawnPos.y);
    break;
  default:
    newBot = new StreetThugBot(spawnPos.x, spawnPos.y);
    break;
  }

  if (newBot != nullptr)
  {
    newBot->LoadTextures();
    activeBots.push_back(newBot);
  }
}

void BotSpawner::SpawnSquad(int count, BotType primaryType)
{
  for (int i = 0; i < count; i++)
  {
    BotType spawnType = primaryType;

    if (GetRandomValue(0, 100) < 30)
    {
      spawnType = GetRandomBotType();
    }

    SpawnBot(spawnType);
  }
}

void BotSpawner::SpawnMixedEncounter()
{
  int encounterSize = GetRandomValue(3, 6);

  // Mixed encounters have variety
  for (int i = 0; i < encounterSize; i++)
  {
    BotType type = GetRandomBotType();
    SpawnBot(type);
  }
}

void BotSpawner::ClearAllBots()
{
  for (Bot *bot : activeBots)
  {
    if (bot != nullptr)
    {
      delete bot;
    }
  }
  activeBots.clear();
}

Vector2 BotSpawner::GetSpawnPosition()
{
  Vector2 spawnPos;

  // Spawn bots just off-screen
  int side = GetRandomValue(0, 3); // 0=left, 1=right, 2=top, 3=bottom

  switch (side)
  {
  case 0: // Left side
    spawnPos.x = -Bot::SPAWN_MARGIN;
    spawnPos.y = GetRandomValue(0, GetScreenHeight());
    break;
  case 1: // Right side
    spawnPos.x = GetScreenWidth() + Bot::SPAWN_MARGIN;
    spawnPos.y = GetRandomValue(0, GetScreenHeight());
    break;
  case 2: // Top
    spawnPos.x = GetRandomValue(0, GetScreenWidth());
    spawnPos.y = -Bot::SPAWN_MARGIN;
    break;
  case 3: // Bottom
    spawnPos.x = GetRandomValue(0, GetScreenWidth());
    spawnPos.y = GetScreenHeight() + Bot::SPAWN_MARGIN;
    break;
  default:
    spawnPos.x = GetScreenWidth() + Bot::SPAWN_MARGIN;
    spawnPos.y = GetScreenHeight() / 2;
    break;
  }

  return spawnPos;
}

void BotSpawner::CleanupDefeatedBots()
{
  for (auto it = activeBots.begin(); it != activeBots.end();)
  {
    Bot *bot = *it;
    if (bot != nullptr && !bot->IsAlive())
    {
      delete bot;
      it = activeBots.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

BotType BotSpawner::GetRandomBotType()
{
  // Weight the bot types based on aggression level
  int roll = GetRandomValue(0, 100);

  if (aggressionLevel < 1.5f) // Early game
  {
    if (roll < 50)
      return BotType::STREET_THUG;
    else if (roll < 70)
      return BotType::RUSHER;
    else if (roll < 85)
      return BotType::THROWER;
    else if (roll < 95)
      return BotType::SHOOTER;
    else
      return BotType::BRAWLER;
  }
  else if (aggressionLevel < 3.0f) // Mid game
  {
    if (roll < 30)
      return BotType::STREET_THUG;
    else if (roll < 50)
      return BotType::BRAWLER;
    else if (roll < 65)
      return BotType::RUSHER;
    else if (roll < 80)
      return BotType::SHOOTER;
    else if (roll < 90)
      return BotType::THROWER;
    else
      return BotType::HEAVY;
  }
  else // Late game - more challenging enemies
  {
    if (roll < 20)
      return BotType::STREET_THUG;
    else if (roll < 35)
      return BotType::BRAWLER;
    else if (roll < 50)
      return BotType::HEAVY;
    else if (roll < 65)
      return BotType::SHOOTER;
    else if (roll < 80)
      return BotType::RUSHER;
    else
      return BotType::THROWER;
  }
}

// Additional Bot helper methods that were declared but not implemented

bool Bot::IsInAttackRange() const
{
  // This would need player position - placeholder implementation
  return false; // Should be implemented with actual player position check
}

bool Bot::IsInShootRange() const
{
  // This would need player position - placeholder implementation
  return false; // Should be implemented with actual player position check
}

Vector2 Bot::GetRandomSpawnPoint()
{
  Vector2 spawnPos;

  // Similar to BotSpawner::GetSpawnPosition but as instance method
  int side = GetRandomValue(0, 3);

  switch (side)
  {
  case 0: // Left
    spawnPos.x = -SPAWN_MARGIN;
    spawnPos.y = GetRandomValue(0, GetScreenHeight());
    break;
  case 1: // Right
    spawnPos.x = GetScreenWidth() + SPAWN_MARGIN;
    spawnPos.y = GetRandomValue(0, GetScreenHeight());
    break;
  case 2: // Top
    spawnPos.x = GetRandomValue(0, GetScreenWidth());
    spawnPos.y = -SPAWN_MARGIN;
    break;
  case 3: // Bottom
    spawnPos.x = GetRandomValue(0, GetScreenWidth());
    spawnPos.y = GetScreenHeight() + SPAWN_MARGIN;
    break;
  }

  return spawnPos;
}

// Private method implementations that were declared but missing

void Bot::ExecuteDirectAttack(Vector2 playerPos, float deltaTime)
{
  float distance = GetDistanceToPlayer(playerPos);

  if (distance <= attackRange && CanAttack())
  {
    ChooseDirectAttack(playerPos);
  }
  else if (distance > attackRange)
  {
    RunTowardPlayer(playerPos);
  }
}

void Bot::ExecuteRangedAttack(Vector2 playerPos, float deltaTime)
{
  float distance = GetDistanceToPlayer(playerPos);

  if (distance <= shootRange && distance > attackRange && CanAttack())
  {
    ChooseRangedAttack(playerPos);
  }
  else if (distance > shootRange)
  {
    RunTowardPlayer(playerPos);
  }
  else if (distance <= attackRange)
  {
    // Back away to optimal range
    Vector2 awayDir = Vector2Subtract({x, y}, playerPos);
    awayDir = Vector2Normalize(awayDir);
    x += awayDir.x * speed * deltaTime;
    facing = (awayDir.x > 0) ? Direction::RIGHT : Direction::LEFT;
  }
}

void Bot::ExecuteMovement(Vector2 playerPos, float deltaTime)
{
  if (playerSpotted)
  {
    RunTowardPlayer(playerPos);
  }
  else
  {
    PaceAround();
  }
}

void Bot::ExecutePacing(float deltaTime)
{
  PaceAround();
}

void Bot::ExecuteStunned(float deltaTime)
{
  // Bot is stunned, can't do anything
  if (stunTimer <= 0)
  {
    isStunned = false;
    SetStateWithTimer(BotState::IDLE);
  }
}

void Bot::ExecuteKnockdown(float deltaTime)
{
  // Bot is knocked down, getting back up
  if (knockdownTimer <= 0)
  {
    isKnockedOut = false;
    SetStateWithTimer(BotState::IDLE);
  }
}

void Bot::ChooseDirectAttack(Vector2 playerPos)
{
  switch (type)
  {
  case BotType::STREET_THUG:
  case BotType::RUSHER:
    if (GetRandomValue(0, 1))
      Punch(playerPos);
    else
      Kick(playerPos);
    break;

  case BotType::BRAWLER:
    if (GetRandomValue(0, 2) == 0)
      Grab(playerPos);
    else if (GetRandomValue(0, 1))
      Punch(playerPos);
    else
      Kick(playerPos);
    break;

  case BotType::HEAVY:
    Punch(playerPos); // Heavy devastating punches
    break;

  default:
    Punch(playerPos);
    break;
  }
}

void Bot::ChooseRangedAttack(Vector2 playerPos)
{
  switch (type)
  {
  case BotType::SHOOTER:
    Shoot(playerPos);
    break;

  case BotType::THROWER:
    ThrowWeapon(playerPos);
    break;

  default:

    ChooseDirectAttack(playerPos);
    break;
  }
}

void Bot::AttemptBlock()
{
  Block();
}

void Bot::AttemptCounterAttack(Vector2 playerPos)
{
  if (GetRandomValue(0, 100) < (int)(counterAttackChance * 100))
  {
    attackCooldown = 0.5f; // Quick counter
    ChooseDirectAttack(playerPos);
  }
}

void Bot::SetupPacingArea()
{
  paceStartPos = {x - paceDistance, y};
  paceEndPos = {x + paceDistance, y};
  pacingRight = true;
}