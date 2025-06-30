#include "includes/Controller.hpp"
#include <algorithm>
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

  // Load sounds and textures with validation
  if (FileExists("Audio/start.mp3"))
  {
    clickSound = LoadSound("Audio/start.mp3");
  }
  if (FileExists("Audio/Intro1.mp3"))
  {
    backgroundMusic = LoadMusicStream("Audio/Intro1.mp3");
  }
  if (FileExists("Audio/PlayingSound.mp3"))
  {
    playingMusic = LoadMusicStream("Audio/PlayingSound.mp3");
  }

  if (FileExists("resource/texture/TitleGame.png"))
  {
    titleTexture = LoadTexture("resource/texture/TitleGame.png");
    titleScale = scale * 3.0f;
    titlePosition = {(screenWidth - (titleTexture.width * titleScale)) / 2.0f, 20.0f * scale};
  }

  // Initialize player with validation
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
  if (player)
  {
    player->SetJumpSpeed(15.0f);
    player->SetGravity(0.8f);
    player->SetGroundY(270.0f);
    player->SetFireCooldown(0.3f);
    player->SetGunshotVolume(0.7f);
  }

  // Menu Layers with validation
  if (FileExists("resource/texture/Sky_pale.png"))
    menuLayers.push_back(new Layer("resource/texture/Sky_pale.png", 0.1f, 0, scale));
  if (FileExists("resource/texture/back.png"))
    menuLayers.push_back(new Layer("resource/texture/back.png", 0.5f, 0, scale));
  if (FileExists("resource/texture/Houses3_pale.png"))
    menuLayers.push_back(new Layer("resource/texture/Houses3_pale.png", 1.0f, 70, scale));
  if (FileExists("resource/texture/houses1.png"))
    menuLayers.push_back(new Layer("resource/texture/houses1.png", 1.0f, 70, scale));
  if (FileExists("resource/texture/minishop&callbox.png"))
    menuLayers.push_back(new Layer("resource/texture/minishop&callbox.png", 1.0f, 80, scale));
  if (FileExists("resource/texture/road&lamps.png"))
    menuLayers.push_back(new Layer("resource/texture/road&lamps.png", 1.0f, 75, scale));

  // Game Layers with validation
  if (FileExists("resource/texture/sky.png"))
    gameLayers.push_back(new Layer("resource/texture/sky.png", 0.1f, 0, scale));
  if (FileExists("resource/texture/houses3.png"))
    gameLayers.push_back(new Layer("resource/texture/houses3.png", 0.5f, 0, scale));
  if (FileExists("resource/texture/night2.png"))
    gameLayers.push_back(new Layer("resource/texture/night2.png", 1.0f, 70, scale));
  if (FileExists("resource/texture/night.png"))
    gameLayers.push_back(new Layer("resource/texture/night.png", 1.0f, 75, scale));
  if (FileExists("resource/texture/road.png"))
    gameLayers.push_back(new Layer("resource/texture/road.png", 1.0f, 75, scale));
  if (FileExists("resource/texture/crosswalk.png"))
    gameLayers.push_back(new Layer("resource/texture/crosswalk.png", 1.0f, 70, scale));

  // Main Game Layers with validation
  AddGamelayer("resource/texture/1.png");
  AddGamelayer("resource/texture/6.png");
  AddGamelayer("resource/texture/5.png");
  AddGamelayer("resource/texture/4.png");
  AddGamelayer("resource/texture/3.png");
  AddGamelayer("resource/texture/2.png");
  AddGamelayer("resource/texture/7.png");

  // Buttons with validation
  if (FileExists("resource/texture/button1.png"))
  {
    startButton = new Button("resource/texture/button1.png", "resource/texture/button2.png", "resource/texture/button3.png", scale * 5.0f, true, 70.0f);
  }
  if (FileExists("resource/texture/exit1.png"))
  {
    exitButton = new Button("resource/texture/exit1.png", "resource/texture/exit2.png", "resource/texture/exit3.png", scale * 5.0f, true, 160.0f);
  }
  if (FileExists("resource/texture/yes.png"))
  {
    yesButton = new Button("resource/texture/yes.png", "resource/texture/yes2.png", "resource/texture/yes3.png", 2.5f);
  }
  if (FileExists("resource/texture/no.png"))
  {
    noButton = new Button("resource/texture/no.png", "resource/texture/no2.png", "resource/texture/no3.png", 2.5f);
  }

  if (FileExists("resource/texture/gear.png"))
  {
    settingIcon = new SettingMenu("resource/texture/gear.png", "resource/texture/gearHover.png", "resource/texture/gearClick.png", 1.0f, true, true);
  }
  if (FileExists("resource/texture/resume1.png"))
  {
    resumeButton = new Button("resource/texture/resume1.png",
                              "resource/texture/resume2.png",
                              "resource/texture/resume3.png",
                              2.5f, true, 0.0f);
  }
  if (FileExists("resource/texture/menu1.png"))
  {
    backToMenuButton = new Button("resource/texture/menu1.png", "resource/texture/menu2.png", "resource/texture/menu3.png", 2.5f, true, 0.0f);
  }

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
  spawner = new BotSpawner();
  popup = Popup();
  // spawner = new BotSpawner();

  if (!IsMusicStreamPlaying(backgroundMusic))
  {
    PlayMusicStream(backgroundMusic);
  }

  // Camera
  camera.offset = {screenWidth / 2.0f, screenHeight / 2.0f};
  camera.target = {player->GetX(), player->GetY()};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
}
void Controller::AddGamelayer(const std::string &file)
{
  if (FileExists(file.c_str()))
  {
    Gamelayer *layer = new Gamelayer(file.c_str(), 0.0f, scale);
    mainlayers.push_back(layer);

    float width = layer->GetTextureWidth() * scale;
    if (width > levelWidth)
      levelWidth = width; // Keep the widest as level width
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
  if (IsMusicStreamPlaying(backgroundMusic))
  {
    UpdateMusicStream(backgroundMusic);

    if (!menuMusicStarted)
    {
      PlayMusicStream(backgroundMusic);
      menuMusicStarted = true;
      playingMusicStarted = false;
    }
  }

  for (Layer *layer : menuLayers)
    if (layer)
      layer->Update();

  if (!showExitPop)
  {
    if (startButton)
    {
      startButton->Update();
      if (startButton->IsClicked())
      {
        if (IsSoundValid(clickSound))
          PlaySound(clickSound);
        currentState = Gamestate::GAME;
        gameTimer = 0;
        fadeOutComplete = false;
      }
    }

    if (exitButton)
    {
      exitButton->Update();
      if (exitButton->IsClicked())
      {
        if (IsSoundValid(clickSound))
          PlaySound(clickSound);
        showExitPop = true;
      }
    }
  }
}

void Controller::UpdateGame()
{
  for (Layer *layer : gameLayers)
    if (layer)
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
    if (IsMusicStreamPlaying(backgroundMusic))
    {
      float volume = 1.0f - (float)gameTimer / fadeDuration;
      SetMusicVolume(backgroundMusic, volume < 0 ? 0 : volume);
    }

    if (gameTimer >= fadeDuration)
    {
      fadeOutComplete = true;

      currentState = Gamestate::PLAYING;
    }
  }
}

void Controller::UpdatePlaying()
{

  if (settingIcon)
  {
    settingIcon->Update();
    if (settingIcon->WasClicked())
    {
      if (IsSoundValid(clickSound))
        PlaySound(clickSound);
      showSettingsPopup = true;
      return;
    }
  }

  if (showSettingsPopup)
  {
    return;
  }

  if (player)
  {
    player->HandleInput();
    player->Update(camera);
  }
  float px = player->GetX();
  float clampedX = Clamp(px, 0.0f, levelWidth - player->GetWidth());
  if (px != clampedX)
    player->SetPosition(clampedX, player->GetY());

  float minX = screenWidth / 2.0f;
  float maxX = levelWidth - screenWidth / 2.0f;
  float camX = Clamp(player->GetX(), minX, maxX);

  float camY = screenHeight / 2.0f;

  camera.target = {camX, camY};

  Vector2 playerPos = player ? Vector2{player->GetX(), player->GetY()} : Vector2{0, 0};
  float deltaTime = GetFrameTime();

  // if (spawner)
  {
    spawner->Update(deltaTime, playerPos);
  }

  float newCamX = Clamp(player->GetX(), minX, maxX);
  float scrollDelta = newCamX - camera.target.x;
  camera.target.x = newCamX;
  // Update background layers
  for (Gamelayer *main : mainlayers)
    if (main)
      main->UpdateLayer(scrollDelta);

  // Update music
  if (!playingMusicStarted && !IsMusicStreamPlaying(playingMusic))
  {
    if (IsMusicStreamPlaying(backgroundMusic))
      StopMusicStream(backgroundMusic);
    PlayMusicStream(playingMusic);
    SetMusicVolume(playingMusic, 0.5f);
    playingMusicStarted = true;
  }
  if (IsMusicStreamPlaying(playingMusic))
  {
    UpdateMusicStream(playingMusic);
  }
}

void Controller::DrawMenu()
{
  for (Layer *layer : menuLayers)
    if (layer)
      layer->Draw();

  if (startButton)
    startButton->Draw();
  if (exitButton)
    exitButton->Draw();

  if (titleTexture.id > 0)
  {
    DrawTextureEx(titleTexture, titlePosition, 0.0f, titleScale, WHITE);
  }

  if (showExitPop && yesButton && noButton)
    popup.DrawExitPopup(running, showExitPop, clickSound, *yesButton, *noButton);
}

void Controller::DrawGame()
{
  for (Layer *layer : gameLayers)
    if (layer)
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

  BeginMode2D(camera);
  for (Gamelayer *main : mainlayers)
    if (main)
      main->Drawlayer();

  if (player)
    player->Draw();
  EndMode2D();

  if (settingIcon)
    settingIcon->Draw();

  if (showSettingsPopup && resumeButton && backToMenuButton)
  {
    settingpop.DrawSettingPopup(showSettingsPopup, clickSound,
                                *resumeButton, *backToMenuButton, currentState);
  }
}

void Controller::Unload()
{
  for (Layer *layer : menuLayers)
    delete layer;
  menuLayers.clear();

  for (Layer *layer : gameLayers)
    delete layer;
  gameLayers.clear();

  if (spawner)
  {
    delete spawner;
    spawner = nullptr;
  }

  for (Gamelayer *main : mainlayers)
    delete main;
  mainlayers.clear();

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

  if (titleTexture.id > 0)
    UnloadTexture(titleTexture);
  if (IsSoundValid(clickSound))
    UnloadSound(clickSound);
  if (IsMusicStreamPlaying(backgroundMusic))
    UnloadMusicStream(backgroundMusic);
  if (IsMusicStreamPlaying(playingMusic))
    UnloadMusicStream(playingMusic);
}