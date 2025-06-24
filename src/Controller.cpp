#include "includes/Controller.hpp"

#include <raylib.h>

Controller::Controller()
{
  startButton = nullptr;
  exitButton = nullptr;
  yesButton = nullptr;
  noButton = nullptr;
}

void Controller::Init(int screenW, int screenH, int originalW, int originalH)
{
  InitAudioDevice();

  // Store dimensions
  screenWidth = screenW;
  screenHeight = screenH;
  originalWidth = originalW;
  originalHeight = originalH;

  // Calculate scaling factors
  scaleX = (float)screenWidth / (float)originalWidth;
  scaleY = (float)screenHeight / (float)originalHeight;
  scale = scaleX;

  currentState = Gamestate::MENU;

  // Load sounds and textures
  clickSound = LoadSound("Audio/start.mp3");
  backgroundMusic = LoadMusicStream("Audio/Intro1.mp3");
  playingMusic = LoadMusicStream("Audio/PlayingSound.mp3");
  titleTexture = LoadTexture("resource/texture/TitleGame.png");
  titleScale = scale * 3.0f;
  titlePosition = {(screenWidth - (titleTexture.width * titleScale)) / 2.0f, 20.0f * scale};

  // Initialize player
  player = new Character("resource/player/Idle.png",
                         "resource/player/Idle_2.png",
                         "resource/player/Walk.png",
                         "resource/player/Run.png",
                         "resource/player/Shot.png",
                         "resource/player/Jump.png",
                         "resource/player/Attack_1.png",
                         "Audio/Gun.mp3",
                         "Audio/Attack.mp3",
                         "resource/player/bullet.png",
                         120.0f, 270.0f, 2.0f);
  player->SetJumpSpeed(15.0f);
  player->SetGravity(0.8f);
  player->SetGroundY(270.0f);
  player->SetFireCooldown(0.3f);
  player->SetGunshotVolume(0.7f);

  // Menu Layers
  menuLayers = {
      new Layer("resource/texture/Sky_pale.png", 0.1f, 0, scale),
      new Layer("resource/texture/back.png", 0.5f, 0, scale),
      new Layer("resource/texture/Houses3_pale.png", 1.0f, 70, scale),
      new Layer("resource/texture/houses1.png", 1.0f, 70, scale),
      new Layer("resource/texture/minishop&callbox.png", 1.0f, 80, scale),
      new Layer("resource/texture/road&lamps.png", 1.0f, 75, scale)};

  // Game Layers
  gameLayers = {
      new Layer("resource/texture/sky.png", 0.1f, 0, scale),
      new Layer("resource/texture/houses3.png", 0.5f, 0, scale),
      new Layer("resource/texture/night2.png", 1.0f, 70, scale),
      new Layer("resource/texture/night.png", 1.0f, 75, scale),
      new Layer("resource/texture/road.png", 1.0f, 75, scale),
      new Layer("resource/texture/crosswalk.png", 1.0f, 70, scale)};

  // Main Game Layers
  mainlayers = {
      new Gamelayer("resource/texture/mainsky.png", 0.0f, scale),
      new Gamelayer("resource/texture/housemain2.png", 0.0f, scale),
      new Gamelayer("resource/texture/housemain.png", 0.0f, scale),
      new Gamelayer("resource/texture/housemain1.png", 0.0f, scale),
      new Gamelayer("resource/texture/fountain&bush.png", 0.0f, scale),
      new Gamelayer("resource/texture/policebox.png", 0.0f, scale),
      new Gamelayer("resource/texture/mainroad.png", 0.0f, scale)};

  // Buttons
  startButton = new Button("resource/texture/button1.png", "resource/texture/button2.png", "resource/texture/button3.png", scale * 5.0f, true, 70.0f);
  exitButton = new Button("resource/texture/exit1.png", "resource/texture/exit2.png", "resource/texture/exit3.png", scale * 5.0f, true, 160.0f);
  yesButton = new Button("resource/texture/yes.png", "resource/texture/yes2.png", "resource/texture/yes3.png", 2.5f);
  noButton = new Button("resource/texture/no.png", "resource/texture/no2.png", "resource/texture/no3.png", 2.5f);

  settingIcon = new SettingMenu("resource/texture/gear.png", "resource/texture/gearHover.png", "resource/texture/gearClick.png", 1.0f, true, true);
  resumeButton = new Button("resource/texture/resume1.png",
                            "resource/texture/resume2.png",
                            "resource/texture/resume3.png",
                            2.5f, true, 0.0f);
  backToMenuButton = new Button("resource/texture/menu1.png", "resource/texture/menu2.png", "resource/texture/menu3.png", 2.5f, true, 0.0f);

  // Init state helpers
  frameCounter = 0;
  dotCount = 0;
  maxDots = 3;
  animatedText = " ";
  gameTimer = 0;
  fadeDuration = 300;
  fadeOutComplete = false;
  playingMusicStarted = false;
  running = true;
  showExitPop = false;

  popup = Popup();

  PlayMusicStream(backgroundMusic);

  SpawnBots(4);
}

// Add the missing CountBotsByType function
int Controller::CountBotsByType(BotType type) const
{
  int count = 0;
  for (const Bot *bot : bots)
  {
    if (bot && bot->GetBotType() == type)
    {
      count++;
    }
  }
  return count;
}

void Controller::SpawnBots(int count)
{
  // Clear existing bots
  for (Bot *bot : bots)
  {
    delete bot;
  }
  bots.clear();

  Vector2 playerPos = {player->GetX(), player->GetY()};

  // Spawn different types of bots with balanced distribution
  for (int i = 0; i < count; i++)
  {
    Vector2 spawnPos = GetSafeSpawnPosition(playerPos, SPAWN_SAFE_DISTANCE);
    Bot *newBot = nullptr;

    // Distribute bot types evenly (now: CIVILIAN, THUG, GANGSTER)
    int botTypeIndex = i % 3;

    switch (botTypeIndex)
    {
    case 0:
      if (CountBotsByType(BotType::CIVILIAN) < MAX_BOTS_PER_TYPE)
      {
        newBot = new CivilianBot(spawnPos.x, spawnPos.y);
      }
      break;
    case 1:
      if (CountBotsByType(BotType::THUG) < MAX_BOTS_PER_TYPE)
      {
        newBot = new ThugBot(spawnPos.x, spawnPos.y);
      }
      break;
    case 2:
      if (CountBotsByType(BotType::GANGSTER) < MAX_BOTS_PER_TYPE)
      {
        newBot = new GangsterBot(spawnPos.x, spawnPos.y);
      }
      break;
    }

    if (newBot)
    {
      newBot->LoadTextures();
      newBot->SetProperties();
      bots.push_back(newBot);
    }
  }
}

Vector2 Controller::GetSafeSpawnPosition(Vector2 playerPos, float minDistance)
{
  Vector2 spawnPos;
  int attempts = 0;
  const int maxAttempts = 50;
  const float GROUND_LEVEL = 470.0f; // Match your player's ground level

  do
  {
    // Only randomize X position, keep Y at ground level
    spawnPos.x = GetRandomValue(100, GetScreenWidth() - 100);
    spawnPos.y = GROUND_LEVEL; // Fixed Y position
    attempts++;

    if (attempts >= maxAttempts)
    {
      // Fallback: spawn at screen edges horizontally
      spawnPos.x = (GetRandomValue(0, 1) == 0) ? 50 : GetScreenWidth() - 50;
      spawnPos.y = GROUND_LEVEL;
      break;
    }
  } while (Vector2Distance(spawnPos, playerPos) < minDistance ||
           IsPositionOccupied(spawnPos));

  return spawnPos;
}

bool Controller::IsPositionOccupied(Vector2 position)
{
  const float minBotDistance = 80.0f;

  for (const Bot *bot : bots)
  {
    if (bot)
    {
      // Only check horizontal distance
      float horizontalDistance = fabs(position.x - bot->x);
      if (horizontalDistance < minBotDistance)
      {
        return true;
      }
    }
  }
  return false;
}

void Controller::Update()
{
  switch (currentState)
  {
  case Gamestate::MENU:
    UpdateMenu();
    break;
  case Gamestate::GAME:
    UpdateGame();
    break;
  case Gamestate::PLAYING:
    UpdatePlaying();
    break;
  default:
    break;
  }
}

void Controller::Draw()
{
  BeginDrawing();
  ClearBackground(RAYWHITE);

  switch (currentState)
  {
  case Gamestate::MENU:
    DrawMenu();
    break;
  case Gamestate::GAME:
    DrawGame();
    break;
  case Gamestate::PLAYING:
    DrawPlaying();
    break;
  }

  EndDrawing();
}

void Controller::UpdateMenu()
{
  UpdateMusicStream(backgroundMusic);

  if (!menuMusicStarted)
  {
    PlayMusicStream(backgroundMusic);
    menuMusicStarted = true;
    playingMusicStarted = false;
  }
  for (Layer *layer : menuLayers)
    layer->Update();

  if (!showExitPop)
  {
    startButton->Update();
    exitButton->Update();

    if (startButton->IsClicked())
    {
      PlaySound(clickSound);
      currentState = Gamestate::GAME;
      gameTimer = 0;
      fadeOutComplete = false;
    }

    if (exitButton->IsClicked())
    {
      PlaySound(clickSound);
      showExitPop = true;
    }
  }
}

void Controller::UpdateGame()
{
  for (Layer *layer : gameLayers)
    layer->Update();

  frameCounter++;
  if (frameCounter >= 30)
  {
    frameCounter = 0;
    dotCount = (dotCount + 1) % (maxDots + 1);
    animatedText = "Please wait" + std::string(dotCount, '.');
  }

  if (!fadeOutComplete)
  {
    gameTimer++;
    float volume = 1.0f - (float)gameTimer / fadeDuration;
    SetMusicVolume(backgroundMusic, volume < 0 ? 0 : volume);

    if (gameTimer >= fadeDuration)
    {
      fadeOutComplete = true;
      SpawnBots(4); // Spawn bots when transitioning to playing state
      currentState = Gamestate::PLAYING;
    }
  }
}

void Controller::UpdatePlaying()
{
  // Handle settings popup first
  settingIcon->Update();
  if (settingIcon->WasClicked())
  {
    PlaySound(clickSound);
    showSettingsPopup = true;
    return;
  }

  if (showSettingsPopup)
  {
    return;
  }

  // Update player
  player->HandleInput();
  player->Update();

  Vector2 playerPos = {player->GetX(), player->GetY()};
  float deltaTime = GetFrameTime();
  float backgroundSpeed = player->GetCurrentMovementSpeed();

  // Update background layers
  for (Gamelayer *main : mainlayers)
    main->UpdateLayer(backgroundSpeed);

  // Update bot AI and interactions
  UpdateBotAI(playerPos, deltaTime);

  HandleBotPlayerInteractions(playerPos);

  HandleBotInteractions();

  CleanupAndRespawnBots();

  // Update music
  if (!playingMusicStarted)
  {
    StopMusicStream(backgroundMusic);
    PlayMusicStream(playingMusic);
    SetMusicVolume(playingMusic, 0.5f);
    playingMusicStarted = true;
  }
  UpdateMusicStream(playingMusic);
}

void Controller::UpdateBotAI(Vector2 playerPos, float deltaTime)
{
  // Create a vector of bot pointers for AI updates
  std::vector<Bot *> aliveBots;
  for (Bot *bot : bots)
  {
    if (bot && bot->IsAlive() && bot->IsSpawned())
    {
      aliveBots.push_back(bot);
    }
  }

  // Update each bot's AI with knowledge of all other bots
  for (Bot *bot : aliveBots)
  {
    bot->Update();

    // The key E-SWAT integration happens here
    bot->UpdateAI(playerPos, deltaTime, aliveBots);

    // Update bot-specific behaviors
    UpdateBotSpecificBehavior(bot, playerPos, deltaTime, aliveBots);
  }
}

void Controller::DrawMenu()
{
  for (Layer *layer : menuLayers)
    layer->Draw();

  startButton->Draw();
  exitButton->Draw();

  DrawTextureEx(titleTexture, titlePosition, 0.0f, titleScale, WHITE);

  if (showExitPop)
    popup.DrawExitPopup(running, showExitPop, clickSound, *yesButton, *noButton);
}

void Controller::DrawGame()
{
  for (Layer *layer : gameLayers)
    layer->Draw();

  DrawTextOutlined(animatedText.c_str(), 350, 270, 40, WHITE, BLACK);

  if (!fadeOutComplete)
  {
    float alpha = 1.0f - (float)gameTimer / fadeDuration;
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, alpha));
  }
}

void Controller::UpdateBotSpecificBehavior(Bot *bot, Vector2 playerPos,
                                           float deltaTime, const std::vector<Bot *> &allBots)
{
  switch (bot->GetBotType())
  {
  case BotType::CIVILIAN:
  {
    CivilianBot *civilian = static_cast<CivilianBot *>(bot);
    if (civilian->IsNearDanger(player->GetPosition(), allBots))
    {
      civilian->ExecutePanicBehavior(player->GetPosition(), GetFrameTime(), bots);
    }
    else
    {
      civilian->SetState(BotState::IDLE);
    }
    break;
  }

  case BotType::GANGSTER:
  {
    GangsterBot *gang = static_cast<GangsterBot *>(bot);

    gang->ExecuteESWATTactics(playerPos, deltaTime, allBots);
    break;
  }

  case BotType::THUG:
  {
    ThugBot *thug = static_cast<ThugBot *>(bot);
    thug->UpdateAI(player->GetPosition(), GetFrameTime(), bots);
    break;
  }
  }
}

void Controller::HandleBotPlayerInteractions(Vector2 playerPos)
{
  const float INTERACTION_RANGE = 50.0f;

  for (Bot *bot : bots)
  {
    if (!bot || !bot->IsAlive())
      continue;

    float distance = Vector2Distance({bot->x, bot->y}, playerPos);

    if (distance <= INTERACTION_RANGE)
    {
      switch (bot->GetBotType())
      {
      case BotType::THUG:
      case BotType::GANGSTER:
        // Handle combat interaction
        if (bot->CanAttack())
        {
          bot->Attack();
          HandlePlayerDamage(bot);
        }
        break;

      case BotType::CIVILIAN:
        // Civilians just panic and flee - no damage
        break;
      }
    }
  }
}

void Controller::HandlePlayerDamage(Bot *attackingBot)
{
  // Implement player damage system here
  float damage = 0.0f;

  switch (attackingBot->GetBotType())
  {
  case BotType::THUG:
    damage = 15.0f;
    break;
  case BotType::GANGSTER:
    damage = 20.0f; // SWAT is more precise
    break;
  default:
    damage = 0.0f;
    break;
  }

  if (damage > 0.0f)
  {
    // Apply damage to player
    // player->TakeDamage(damage);

    // Play damage sound effect
    // PlaySound(playerHitSound);

    TraceLog(LOG_INFO, "Player took %.1f damage from %s",
             damage, GetBotTypeName(attackingBot->GetBotType()));
  }
}

void Controller::HandleBotInteractions()
{
  // Handle bot-to-bot interactions (friendly fire, coordination, etc.)
  for (size_t i = 0; i < bots.size(); ++i)
  {
    for (size_t j = i + 1; j < bots.size(); ++j)
    {
      Bot *bot1 = bots[i];
      Bot *bot2 = bots[j];

      if (!bot1 || !bot2 || !bot1->IsAlive() || !bot2->IsAlive())
        continue;

      float distance = Vector2Distance({bot1->x, bot1->y}, {bot2->x, bot2->y});

      if (distance <= BOT_INTERACTION_RANGE)
      {
        HandleBotPairInteraction(bot1, bot2, distance);
      }
    }
  }
}

void Controller::HandleBotPairInteraction(Bot *bot1, Bot *bot2, float distance)
{
  BotType type1 = bot1->GetBotType();
  BotType type2 = bot2->GetBotType();

  // Civilians panic when near hostile bots
  if (type1 == BotType::CIVILIAN && (type2 == BotType::THUG || type2 == BotType::GANGSTER))
  {
    CivilianBot *civilian = static_cast<CivilianBot *>(bot1);
    civilian->SetState(BotState::FLEEING);
  }
  else if (type2 == BotType::CIVILIAN && (type1 == BotType::THUG || type1 == BotType::GANGSTER))
  {
    CivilianBot *civilian = static_cast<CivilianBot *>(bot2);
    civilian->SetState(BotState::FLEEING);
  }
}

void Controller::CleanupAndRespawnBots()
{
  // Remove dead bots
  auto it = std::remove_if(bots.begin(), bots.end(),
                           [](Bot *bot)
                           {
                             if (!bot->IsAlive())
                             {
                               delete bot;
                               return true;
                             }
                             return false;
                           });
  bots.erase(it, bots.end());

  // Respawn bots if too few remain
  const int MIN_BOTS = 3;
  if (bots.size() < MIN_BOTS)
  {
    int botsToSpawn = MIN_BOTS - bots.size();
    SpawnBots(botsToSpawn);
  }
}

// Fixed GetBotTypeName function - removed ESWAT case
const char *Controller::GetBotTypeName(BotType type)
{
  switch (type)
  {
  case BotType::CIVILIAN:
    return "Civilian";
  case BotType::GANGSTER:
    return "SWAT";
  case BotType::THUG:
    return "Thug";
  default:
    return "Unknown";
  }
}

// Add the missing GetBotStateText function
const char *Controller::GetBotStateText(BotState state)
{
  switch (state)
  {
  case BotState::IDLE:
    return "IDLE";
  case BotState::WANDERING:
    return "WANDERING";
  case BotState::CHASING:
    return "CHASING";
  case BotState::ATTACK:
    return "ATTACKING";
  case BotState::FLEEING:
    return "FLEEING";
  case BotState::DEAD:
    return "DEAD";
  case BotState::SPAWNING:
    return "SPAWNING";
  case BotState::PATROLLING:
    return "PATROLLING";
  case BotState::TACTICAL_POSITIONING:
    return "POSITIONING";
  case BotState::COORDINATED_ATTACK:
    return "COORD_ATTACK";
  case BotState::RETREATING:
    return "RETREATING";
  default:
    return "UNKNOWN";
  }
}

void Controller::DrawPlaying()
{
  // Draw background layers
  for (Gamelayer *main : mainlayers)
    main->Drawlayer();

  // Draw bots with tactical indicators
  DrawBotsWithTacticalInfo();

  // Draw player
  player->Draw();

  // Draw UI elements
  settingIcon->Draw();
  DrawBotStatusHUD();

  if (showSettingsPopup)
  {
    settingpop.DrawSettingPopup(showSettingsPopup, clickSound,
                                *resumeButton, *backToMenuButton, currentState);
  }
}

void Controller::DrawBotsWithTacticalInfo()
{
  for (Bot *bot : bots)
  {
    if (!bot || !bot->IsAlive())
      continue;

    bot->Draw();

    // Draw tactical information for debugging/visual feedback
    if (bot->GetBotType() != BotType::CIVILIAN)
    {
      DrawBotTacticalInfo(bot);
    }
  }
}

void Controller::DrawBotTacticalInfo(Bot *bot)
{
  Vector2 botPos = {bot->x, bot->y};

  // Draw detection range (semi-transparent circle)
  if (bot->GetBotType() == BotType::GANGSTER)
  {
    DrawCircleLines(botPos.x, botPos.y, 250.0f, Fade(BLUE, 0.3f));
  }
  else if (bot->GetBotType() == BotType::THUG)
  {
    DrawCircleLines(botPos.x, botPos.y, 180.0f, Fade(RED, 0.3f));
  }

  // Draw state information
  const char *stateText = GetBotStateText(bot->GetState());
  DrawText(stateText, botPos.x - 20, botPos.y - 30, 10, WHITE);
}

void Controller::DrawBotStatusHUD()
{
  // Draw bot count and status in corner
  int civilians = 0, swat = 0, thugs = 0;

  for (Bot *bot : bots)
  {
    if (!bot || !bot->IsAlive())
      continue;

    switch (bot->GetBotType())
    {
    case BotType::CIVILIAN:
      civilians++;
      break;
    case BotType::GANGSTER:
      swat++;
      break;
    case BotType::THUG:
      thugs++;
      break;
    }
  }

  DrawText(TextFormat("Civilians: %d", civilians), 10, 10, 20, GREEN);
  DrawText(TextFormat("SWAT: %d", swat), 10, 35, 20, BLUE);
  DrawText(TextFormat("Thugs: %d", thugs), 10, 60, 20, RED);
}

void Controller::Unload()
{
  for (Layer *layer : menuLayers)
    delete layer;
  menuLayers.clear();

  for (Layer *layer : gameLayers)
    delete layer;
  gameLayers.clear();

  for (Gamelayer *main : mainlayers)
    delete main;
  mainlayers.clear();

  // Clean up bots
  for (Bot *bot : bots)
  {
    delete bot;
  }
  bots.clear();

  delete player;
  player = nullptr;

  delete startButton;
  delete exitButton;
  delete yesButton;
  delete noButton;

  startButton = nullptr;
  exitButton = nullptr;
  yesButton = nullptr;
  noButton = nullptr;

  UnloadTexture(titleTexture);
  UnloadSound(clickSound);
  UnloadMusicStream(backgroundMusic);
  UnloadMusicStream(playingMusic);
}