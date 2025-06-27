#include "includes/ThugBot.hpp"

// CONSTRUCTOR
StreetThugBot::StreetThugBot(float spawnX, float spawnY)
    : Bot(spawnX, spawnY, BotType::STREET_THUG) // Call base constructor
{
  // StreetThug-specific initialization
  isAggressive = true;
  speed = WALK_SPEED * 1.1f; // 10% faster than base

  // Custom properties
  isBerserk = false;
  berserkTimer = 0.0f;
  backupCallCount = 0;
  lastTauntTime = 0.0f;
  comboStreak = 0;
  aggressionBoost = 1.0f;

  // Override base stats
  health = maxHealth = 80;
  punchDamage = 18; // Higher than base
  kickDamage = 22;
  grabDamage = 15;

  // Behavior settings
  detectionRange = DETECTION_RANGE * 1.2f; // Better detection
  attackRange = ATTACK_RANGE * 0.9f;       // Prefers close combat
  blockChance = 0.15f;                     // Lower block chance (more aggressive)
  counterAttackChance = 0.25f;             // Higher counter chance

  // Thug-specific behavior timers
  rushTimer = 0.0f;
  circleTimer = 0.0f;
  feintTimer = 0.0f;

  // Combat preferences
  preferredAttack = AttackType::PUNCH;
  rushMode = false;
  isCircling = false;
  lastAttackSuccessful = false;
}

// DESTRUCTOR
StreetThugBot::~StreetThugBot()
{
  // Base class destructor called automatically
  // Add any StreetThug-specific cleanup here
}

// OVERRIDE base UpdateAI method
void StreetThugBot::UpdateAI(Vector2 playerPos, float deltaTime)
{
  // Call base AI first
  Bot::UpdateAI(playerPos, deltaTime);

  // Update thug-specific timers
  UpdateThumgTimers(deltaTime);

  // StreetThug-specific AI behavior
  float distanceToPlayer = GetDistanceToPlayer(playerPos);

  // Berserk mode when health is low
  if (health < maxHealth * 0.3f && !isBerserk)
  {
    BerserkMode();
  }

  // Update berserk state
  if (isBerserk)
  {
    UpdateBerserkMode(deltaTime);
  }

  // Enhanced combat behavior
  if (playerSpotted)
  {
    UpdateCombatBehavior(playerPos, distanceToPlayer, deltaTime);
  }
  else
  {
    UpdateIdleBehavior(deltaTime);
  }

  // Occasionally taunt or call for backup
  HandleSocialBehavior(deltaTime);
}

void StreetThugBot::UpdateThumgTimers(float deltaTime)
{
  if (rushTimer > 0)
    rushTimer -= deltaTime;
  if (circleTimer > 0)
    circleTimer -= deltaTime;
  if (feintTimer > 0)
    feintTimer -= deltaTime;
  if (lastTauntTime > 0)
    lastTauntTime -= deltaTime;
}

void StreetThugBot::UpdateBerserkMode(float deltaTime)
{
  berserkTimer -= deltaTime;
  if (berserkTimer <= 0)
  {
    // Exit berserk mode
    isBerserk = false;
    speed = WALK_SPEED * 1.1f; // Back to normal speed
    punchDamage = 18;          // Reset damage
    kickDamage = 22;
    attackCooldown = 1.0f; // Reset attack speed
    aggressionBoost = 1.0f;
  }
  else
  {
    // Maintain berserk effects
    aggressionBoost = 1.5f;
    // Pulsing speed during berserk
    float pulseSpeed = 1.0f + 0.3f * sin(berserkTimer * 4.0f);
    speed = RUN_SPEED * 1.3f * pulseSpeed;
  }
}

void StreetThugBot::UpdateCombatBehavior(Vector2 playerPos, float distance, float deltaTime)
{
  // Different behavior based on distance
  if (distance > attackRange * 2.0f)
  {
    // Far from player - rush or circle
    if (rushTimer <= 0 && GetRandomValue(0, 100) < 30)
    {
      StartRushAttack();
    }
    else
    {
      RunTowardPlayer(playerPos);
    }
  }
  else if (distance > attackRange)
  {
    // Medium range - circle or feint
    if (circleTimer <= 0 && GetRandomValue(0, 100) < 25)
    {
      StartCircling();
    }
    else if (feintTimer <= 0 && GetRandomValue(0, 100) < 20)
    {
      PerformFeint(playerPos);
    }
    else
    {
      RunTowardPlayer(playerPos);
    }
  }
  else
  {
    // Close range - attack or special moves
    ExecuteCloseRangeCombo(playerPos);
  }

  // Update special movement states
  if (rushMode)
  {
    ExecuteRush(playerPos, deltaTime);
  }

  if (isCircling)
  {
    ExecuteCircling(playerPos, deltaTime);
  }
}

void StreetThugBot::UpdateIdleBehavior(float deltaTime)
{
  // More active idle behavior for thugs
  idleTimer += deltaTime;

  if (idleTimer >= 1.5f) // More frequent movement
  {
    // Chance for different idle behaviors
    int behavior = GetRandomValue(0, 100);

    if (behavior < 60)
    {
      PaceAround(); // Standard pacing
    }
    else if (behavior < 80)
    {
      PerformTaunt(); // Taunt animation
    }
    else
    {
      LookAround(); // Look for threats
    }

    idleTimer = 0.0f;
  }
}

void StreetThugBot::HandleSocialBehavior(float deltaTime)
{
  // Call for backup occasionally when in combat
  if (playerSpotted && GetRandomValue(0, 1000) < 3) // 0.3% chance per frame
  {
    CallForBackup();
  }

  // Taunt when successful
  if (lastAttackSuccessful && lastTauntTime <= 0)
  {
    PerformTaunt();
    lastTauntTime = 3.0f; // Cooldown
    lastAttackSuccessful = false;
  }
}

// OVERRIDE base LoadTextures method
void StreetThugBot::LoadTextures()
{
  // Load StreetThug-specific textures
  idleTexture = LoadTexture("assets/bots/streetthug/idle.png");
  walkTexture = LoadTexture("assets/bots/streetthug/walk.png");
  runTexture = LoadTexture("assets/bots/streetthug/run.png");
  punchTexture = LoadTexture("assets/bots/streetthug/punch.png");
  kickTexture = LoadTexture("assets/bots/streetthug/kick.png");
  grabTexture = LoadTexture("assets/bots/streetthug/grab.png");
  hurtTexture = LoadTexture("assets/bots/streetthug/hurt.png");
  knockdownTexture = LoadTexture("assets/bots/streetthug/knockdown.png");
  deathTexture = LoadTexture("assets/bots/streetthug/death.png");

  // Thug-specific textures
  berserkTexture = LoadTexture("assets/bots/streetthug/berserk.png");
  tauntTexture = LoadTexture("assets/bots/streetthug/taunt.png");
  feintTexture = LoadTexture("assets/bots/streetthug/feint.png");

  // Set up StreetThug-specific animations
  idleAnim = {0, 4, 0, 0.25f, 0.0f, 0, AnimationType::REPEATING};
  walkAnim = {0, 6, 0, 0.12f, 0.0f, 0, AnimationType::REPEATING};
  runAnim = {0, 8, 0, 0.08f, 0.0f, 0, AnimationType::REPEATING};
  punchAnim = {0, 5, 0, 0.08f, 0.0f, 0, AnimationType::ONESHOT};
  kickAnim = {0, 4, 0, 0.1f, 0.0f, 0, AnimationType::ONESHOT};
  grabAnim = {0, 6, 0, 0.07f, 0.0f, 0, AnimationType::ONESHOT};
  berserkAnim = {0, 6, 0, 0.15f, 0.0f, 0, AnimationType::REPEATING};
  tauntAnim = {0, 8, 0, 0.2f, 0.0f, 0, AnimationType::ONESHOT};
  feintAnim = {0, 3, 0, 0.1f, 0.0f, 0, AnimationType::ONESHOT};
}

// COMBAT METHODS - Enhanced versions

void StreetThugBot::Punch(Vector2 targetPos)
{
  if (!CanAttack())
    return;

  // Enhanced punch with combo potential
  float baseCooldown = isBerserk ? 0.6f : 1.0f;
  attackCooldown = baseCooldown * aggressionBoost;

  // Damage calculation with combo bonus
  int damage = punchDamage + (comboStreak * 2);
  if (isBerserk)
    damage = (int)(damage * 1.5f);

  PlayPunchSound();
  CreateHitEffect();

  // Check for successful hit
  if (GetDistanceToPlayer(targetPos) <= attackRange)
  {
    // Deal damage logic would go here
    comboStreak++;
    lastAttackSuccessful = true;

    // Chain attack chance
    if (isBerserk && GetRandomValue(0, 100) < 40)
    {
      attackCooldown *= 0.5f; // Quick follow-up
    }
  }
  else
  {
    comboStreak = 0; // Reset combo on miss
  }

  // Set preferred attack for next time
  preferredAttack = (GetRandomValue(0, 1)) ? AttackType::KICK : AttackType::GRAB;
}

void StreetThugBot::Kick(Vector2 targetPos)
{
  if (!CanAttack())
    return;

  float baseCooldown = isBerserk ? 0.8f : 1.2f;
  attackCooldown = baseCooldown * aggressionBoost;

  int damage = kickDamage + (comboStreak * 3);
  if (isBerserk)
    damage = (int)(damage * 1.5f);

  PlayKickSound();
  CreateHitEffect();

  if (GetDistanceToPlayer(targetPos) <= attackRange * 1.1f) // Kicks have slightly longer range
  {
    // Deal kick damage + potential knockdown
    comboStreak++;
    lastAttackSuccessful = true;

    // Kick has chance to knock down
    if (GetRandomValue(0, 100) < 25)
    {
      // Trigger knockdown effect on player
    }
  }
  else
  {
    comboStreak = 0;
  }

  preferredAttack = (GetRandomValue(0, 1)) ? AttackType::PUNCH : AttackType::GRAB;
}

void StreetThugBot::Grab(Vector2 targetPos)
{
  if (!CanAttack())
    return;

  float baseCooldown = isBerserk ? 1.0f : 1.5f;
  attackCooldown = baseCooldown * aggressionBoost;

  int damage = grabDamage + (comboStreak * 4);
  if (isBerserk)
    damage = (int)(damage * 1.5f);

  CreateHitEffect();

  if (GetDistanceToPlayer(targetPos) <= attackRange * 0.8f) // Grabs need closer range
  {
    // Deal grab damage + stun
    comboStreak++;
    lastAttackSuccessful = true;

    // Grab leads to follow-up attack
    if (GetRandomValue(0, 100) < 60)
    {
      // Queue a quick punch after grab
      attackCooldown = 0.3f;
      preferredAttack = AttackType::PUNCH;
    }
  }
  else
  {
    comboStreak = 0;
  }
}

// SPECIAL THUG ABILITIES

void StreetThugBot::BerserkMode()
{
  if (isBerserk)
    return; // Already in berserk

  isBerserk = true;
  berserkTimer = 8.0f;                     // Berserk for 8 seconds
  speed = RUN_SPEED * 1.5f;                // Much faster
  punchDamage = (int)(punchDamage * 1.5f); // More damage
  kickDamage = (int)(kickDamage * 1.5f);
  grabDamage = (int)(grabDamage * 1.5f);
  attackCooldown *= 0.7f; // Faster attacks
  aggressionBoost = 1.5f;

  // Reset combo for berserk bonus
  comboStreak = 0;

  // Visual/audio effect
  PlayBerserkSound();
  CreateBerserkEffect();
}

void StreetThugBot::CallForBackup()
{
  if (backupCallCount < 2) // Max 2 backup calls
  {
    backupCallCount++;
    aggroLevel += 1.0f; // Increase global aggression

    // Could trigger BotSpawner to spawn more thugs nearby
    PlayCallSound();
    CreateCallEffect();

    // Boost own confidence
    aggressionBoost += 0.2f;
  }
}

void StreetThugBot::StartRushAttack()
{
  rushMode = true;
  rushTimer = 2.0f; // Rush for 2 seconds
  speed = RUN_SPEED * 1.2f;

  PlayRushSound();
}

void StreetThugBot::ExecuteRush(Vector2 playerPos, float deltaTime)
{
  // Rush straight at player with increased speed
  Vector2 direction = Vector2Subtract(playerPos, {x, y});
  direction = Vector2Normalize(direction);

  x += direction.x * speed * deltaTime;
  y += direction.y * speed * deltaTime * 0.5f; // Less Y movement

  facing = (direction.x > 0) ? Direction::RIGHT : Direction::LEFT;

  // End rush
  if (rushTimer <= 0)
  {
    rushMode = false;
    speed = WALK_SPEED * 1.1f; // Back to normal

    // If close enough, immediately attack
    if (GetDistanceToPlayer(playerPos) <= attackRange)
    {
      attackCooldown = 0.2f; // Quick attack after rush
    }
  }
}

void StreetThugBot::StartCircling()
{
  isCircling = true;
  circleTimer = 3.0f; // Circle for 3 seconds
  circleDirection = (GetRandomValue(0, 1)) ? 1.0f : -1.0f;
}

void StreetThugBot::ExecuteCircling(Vector2 playerPos, float deltaTime)
{
  // Circle around player while maintaining distance
  Vector2 toPlayer = Vector2Subtract(playerPos, {x, y});
  float distance = Vector2Length(toPlayer);

  // Maintain optimal distance
  float targetDistance = attackRange * 1.5f;

  if (distance > targetDistance)
  {
    // Move closer
    Vector2 direction = Vector2Normalize(toPlayer);
    x += direction.x * speed * deltaTime * 0.7f;
    y += direction.y * speed * deltaTime * 0.7f;
  }
  else if (distance < targetDistance * 0.8f)
  {
    // Move away
    Vector2 direction = Vector2Normalize(toPlayer);
    x -= direction.x * speed * deltaTime * 0.5f;
    y -= direction.y * speed * deltaTime * 0.5f;
  }

  // Circle movement
  Vector2 perpendicular = {-toPlayer.y * circleDirection, toPlayer.x * circleDirection};
  perpendicular = Vector2Normalize(perpendicular);

  x += perpendicular.x * speed * deltaTime * 0.8f;
  y += perpendicular.y * speed * deltaTime * 0.8f;

  // Face player while circling
  facing = (toPlayer.x > 0) ? Direction::RIGHT : Direction::LEFT;

  // End circling
  if (circleTimer <= 0)
  {
    isCircling = false;

    // Chance for surprise attack after circling
    if (GetRandomValue(0, 100) < 70)
    {
      attackCooldown = 0.1f; // Quick surprise attack
    }
  }
}

void StreetThugBot::PerformFeint(Vector2 playerPos)
{
  feintTimer = 1.0f;

  // Quick movement toward player then back
  Vector2 direction = Vector2Subtract(playerPos, {x, y});
  direction = Vector2Normalize(direction);

  // Quick step forward
  x += direction.x * 30.0f;
  y += direction.y * 15.0f;

  // Set up for real attack
  attackCooldown = 0.8f;

  PlayFeintSound();
}

void StreetThugBot::ExecuteCloseRangeCombo(Vector2 playerPos)
{
  if (!CanAttack())
    return;

  // Choose attack based on preference and situation
  float distance = GetDistanceToPlayer(playerPos);

  if (preferredAttack == AttackType::PUNCH || distance > attackRange * 0.9f)
  {
    Punch(playerPos);
  }
  else if (preferredAttack == AttackType::KICK)
  {
    Kick(playerPos);
  }
  else if (preferredAttack == AttackType::GRAB && distance <= attackRange * 0.7f)
  {
    Grab(playerPos);
  }
  else
  {
    // Default to punch
    Punch(playerPos);
  }
}

void StreetThugBot::PerformTaunt()
{
  // Play taunt animation and sound
  PlayTauntSound();

  // Taunt might intimidate or enrage other bots
  aggroLevel += 0.3f;
}

void StreetThugBot::LookAround()
{
  // Change facing direction occasionally while idle
  if (GetRandomValue(0, 100) < 30)
  {
    facing = (facing == Direction::RIGHT) ? Direction::LEFT : Direction::RIGHT;
  }
}

// ENHANCED DRAWING
void StreetThugBot::Draw()
{
  if (!isOnScreen || !IsAlive())
    return;

  Texture2D currentTexture = idleTexture;
  Rectangle sourceRect = {0, 0, frameWidth, frameHeight};

  // Choose texture based on state and special conditions
  if (isBerserk)
  {
    currentTexture = berserkTexture;
  }
  else
  {
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
  }

  // Handle animation frame
  sourceRect.x = currentFrame * frameWidth;

  // Flip texture if facing left
  if (facing == Direction::LEFT)
  {
    sourceRect.width = -frameWidth;
  }
  else
  {
    sourceRect.width = frameWidth;
  }

  Rectangle destRect = {x - width / 2, y - height / 2, width, height};

  // Berserk visual effect
  Color tint = WHITE;
  if (isBerserk)
  {
    // Red tint that pulses
    float pulse = 0.7f + 0.3f * sin(berserkTimer * 8.0f);
    tint = ColorAlpha(RED, pulse);
  }

  DrawTexturePro(currentTexture, sourceRect, destRect, {0, 0}, 0.0f, tint);

  // Draw health bar
  if (health < maxHealth)
  {
    DrawHealthBar();
  }

  // Draw special effect indicators
  DrawSpecialEffects();
}

void StreetThugBot::DrawHealthBar()
{
  float barWidth = width;
  float barHeight = 4.0f;
  float healthPercent = (float)health / (float)maxHealth;

  Rectangle bgRect = {x - barWidth / 2, y - height / 2 - 10, barWidth, barHeight};
  Rectangle healthRect = {x - barWidth / 2, y - height / 2 - 10, barWidth * healthPercent, barHeight};

  DrawRectangleRec(bgRect, DARKRED);
  DrawRectangleRec(healthRect, isBerserk ? ORANGE : GREEN);

  // Combo indicator
  if (comboStreak > 0)
  {
    DrawText(TextFormat("x%d", comboStreak), (int)(x - 10), (int)(y - height / 2 - 25), 12, YELLOW);
  }
}

void StreetThugBot::DrawSpecialEffects()
{
  // Rush effect
  if (rushMode)
  {
    DrawCircleLines((int)x, (int)y, width * 0.7f, ORANGE);
  }

  // Circling indicator
  if (isCircling)
  {
    DrawCircleLines((int)x, (int)y, width * 0.5f, BLUE);
  }

  // Berserk aura
  if (isBerserk)
  {
    float auraSize = width + 10.0f * sin(berserkTimer * 6.0f);
    DrawCircleLines((int)x, (int)y, auraSize, RED);
  }
}

// AUDIO PLACEHOLDER IMPLEMENTATIONS
void StreetThugBot::PlayBerserkSound() { /* PlaySound(berserkSound); */ }
void StreetThugBot::PlayCallSound() { /* PlaySound(callForBackupSound); */ }
void StreetThugBot::PlayRushSound() { /* PlaySound(rushSound); */ }
void StreetThugBot::PlayTauntSound() { /* PlaySound(tauntSound); */ }
void StreetThugBot::PlayFeintSound() { /* PlaySound(feintSound); */ }

// EFFECT PLACEHOLDER IMPLEMENTATIONS
void StreetThugBot::CreateBerserkEffect() { /* Create berserk particle effect */ }
void StreetThugBot::CreateCallEffect() { /* Create backup call effect */ }