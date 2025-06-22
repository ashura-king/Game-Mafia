#include "includes/Controller.hpp"
#include "includes/Civillian.hpp"
#include "includes/Swat.hpp"
#include "includes/ThugBot.hpp"
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
  titleTexture = LoadTexture("resource/TitleGame.png");
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
      new Layer("resource/Sky_pale.png", 0.1f, 0, scale),
      new Layer("resource/back.png", 0.5f, 0, scale),
      new Layer("resource/Houses3_pale.png", 1.0f, 70, scale),
      new Layer("resource/houses1.png", 1.0f, 70, scale),
      new Layer("resource/minishop&callbox.png", 1.0f, 80, scale),
      new Layer("resource/road&lamps.png", 1.0f, 75, scale)};

  // Game Layers
  gameLayers = {
      new Layer("resource/sky.png", 0.1f, 0, scale),
      new Layer("resource/houses3.png", 0.5f, 0, scale),
      new Layer("resource/night2.png", 1.0f, 70, scale),
      new Layer("resource/night.png", 1.0f, 75, scale),
      new Layer("resource/road.png", 1.0f, 75, scale),
      new Layer("resource/crosswalk.png", 1.0f, 70, scale)};

  // Main Game Layers
  mainlayers = {
      new Gamelayer("resource/mainsky.png", 0.0f, scale),
      new Gamelayer("resource/housemain2.png", 0.0f, scale),
      new Gamelayer("resource/housemain.png", 0.0f, scale),
      new Gamelayer("resource/housemain1.png", 0.0f, scale),
      new Gamelayer("resource/fountain&bush.png", 0.0f, scale),
      new Gamelayer("resource/policebox.png", 0.0f, scale),
      new Gamelayer("resource/mainroad.png", 0.0f, scale)};

  // Buttons
  startButton = new Button("resource/button1.png", "resource/button2.png", "resource/button3.png", scale * 5.0f, true, 70.0f);
  exitButton = new Button("resource/exit1.png", "resource/exit2.png", "resource/exit3.png", scale * 5.0f, true, 160.0f);
  yesButton = new Button("resource/yes.png", "resource/yes2.png", "resource/yes3.png", 2.5f);
  noButton = new Button("resource/no.png", "resource/no2.png", "resource/no3.png", 2.5f);

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

  // Initialize bots when entering playing state
  // SpawnBots(4); // Comment this out for now, spawn when game starts
}

void Controller::SpawnBots(int count)
{
  // Clear existing bots
  for (Bot *bot : bots)
  {
    delete bot;
  }
  bots.clear();

  for (int i = 0; i < count; ++i)
  {
    float x = GetRandomValue(100, GetScreenWidth() - 300);
    float y = GetRandomValue(100, GetScreenHeight() - 300);

    // Random bot type selection
    BotType type = static_cast<BotType>(GetRandomValue(0, 2)); // 0=CIVILIAN, 1=SWAT, 2=THUG

    Bot *newBot = nullptr;

    switch (type)
    {
    case BotType::CIVILIAN:
      newBot = new CivilianBot(x, y);
      break;
    case BotType::SWAT:
      newBot = new SwatBot(x, y);
      break;
    case BotType::THUG:
      newBot = new ThugBot(x, y);
      break;
    default:
      newBot = new ThugBot(x, y); // Default fallback
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
  player->HandleInput();
  player->Update();

  Vector2 playerPos = {player->GetX(), player->GetY()};
  float deltaTime = GetFrameTime();
  float backgroundSpeed = player->GetCurrentMovementSpeed();

  for (Gamelayer *main : mainlayers)
    main->UpdateLayer(backgroundSpeed);

  // Update all bots with proper AI
  for (Bot *bot : bots)
  {
    if (bot && bot->IsAlive())
    {
      bot->Update();
      bot->UpdateAI(playerPos, deltaTime, bots);

      // Check collision with player for damage/interaction
      if (bot->CheckCollisionWithPlayer(playerPos, player->GetWidth(), player->GetHeight()))
      {
        // Handle collision based on bot type
        if (bot->GetBotType() == BotType::THUG || bot->GetBotType() == BotType::SWAT)
        {
          if (bot->CanAttack())
          {
            bot->Attack();
            // Apply damage to player here if needed
            // player->TakeDamage(bot->GetAttackDamage());
          }
        }
      }
    }
  }

  // Remove dead bots
  bots.erase(std::remove_if(bots.begin(), bots.end(),
                            [](Bot *bot)
                            {
                              if (!bot->IsAlive())
                              {
                                delete bot;
                                return true;
                              }
                              return false;
                            }),
             bots.end());

  if (!playingMusicStarted)
  {
    StopMusicStream(backgroundMusic);
    PlayMusicStream(playingMusic);
    SetMusicVolume(playingMusic, 0.5f);
    playingMusicStarted = true;
  }

  UpdateMusicStream(playingMusic);
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

void Controller::DrawPlaying()
{
  for (Gamelayer *main : mainlayers)
    main->Drawlayer();

  // Draw all bots
  for (Bot *bot : bots)
  {
    if (bot && bot->IsAlive())
    {
      bot->Draw();
    }
  }

  player->Draw();
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