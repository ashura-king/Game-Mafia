#include "Controller.hpp"
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

  screenWidth = screenW;
  screenHeight = screenH;
  originalWidth = originalW;
  originalHeight = originalH;

  scaleX = (float)screenWidth / (float)originalWidth;
  scaleY = (float)screenHeight / (float)originalHeight;
  scale = scaleX;

  currentState = Gamestate::MENU;

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

  float groundY = 600.0f;

  collisionManager = new CollisionManager(groundY);
  collisionManager->CreateTestLevel();

  float characterHeight = 128.0f * 2;
  float spawnY = groundY - 32.0f - characterHeight;

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
                         120.0f, spawnY, 2.0f);
  if (player)
  {
    player->SetJumpSpeed(15.0f);
    player->SetGravity(0.8f);
    // FIX: Set groundY to the actual ground level, not camera Y
    player->SetGroundY(groundY); // Use 600.0f instead of 270.0f
    player->SetFireCooldown(0.3f);
    player->SetGunshotVolume(0.7f);
  }

  SpriteLoader::LoadMenuLayer(menuLayers, scale);
  SpriteLoader::LoadIntroLayer(gameLayers, scale);
  SpriteLoader::LoadMainGameLayer(this);
  ButtonSprite::LoadSprites(startButton, exitButton, yesButton, noButton, resumeButton, backToMenuButton, settingIcon, scale);

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
  showSettingsPopup = false; // Make sure this is initialized
  spawner = new BotSpawner();
  popup = Popup();

  if (!IsMusicStreamPlaying(backgroundMusic))
  {
    PlayMusicStream(backgroundMusic);
  }

  camera.offset = {screenWidth / 2.0f, screenHeight / 2.0f};
  camera.target = {player->GetX() + player->GetWidth() / 2.0f, player->GetY() + player->GetHeight() / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  // Initialize levelWidth properly
  levelWidth = 0.0f;
  for (auto &layer : mainlayers)
  {
    if (layer)
    {
      float layerWidth = layer->GetTextureWidth() * scale;
      if (layerWidth > levelWidth)
      {
        levelWidth = layerWidth;
      }
    }
  }

  // Set minimum level width if no layers loaded
  if (levelWidth <= 0.0f)
  {
    levelWidth = screenWidth * 3.0f; // Default to 3 screen widths
  }
}

void Controller::AddGamelayer(const std::string &file)
{
  if (FileExists(file.c_str()))
  {
    Gamelayer *layer = new Gamelayer(file.c_str(), 0.0f, scale);
    mainlayers.push_back(layer);

    float width = layer->GetTextureWidth() * scale;
    if (width > levelWidth)
      levelWidth = width;
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
  UpdateMusicStream(playingMusic);

  // Handle settings icon first - always allow this
  if (settingIcon)
  {
    settingIcon->Update();
    if (settingIcon->WasClicked())
    {
      showSettingsPopup = true;
      if (IsSoundValid(clickSound))
        PlaySound(clickSound);
    }
  }

  // If settings popup is showing, don't update game logic
  if (showSettingsPopup)
  {
    return; // Exit early, only the popup will be drawn
  }

  // Normal game logic continues here...
  float previousX = player->GetX();

  // 🎮 Handle player input and update
  player->HandleInput();
  player->Update(camera);

  Rectangle playerBounds = player->GetBoundBox();

  // Horizontal collision
  Collision *horizontalCollision = collisionManager->CheckHorizontalCollision(playerBounds, previousX);
  if (horizontalCollision)
  {
    Rectangle objBounds = horizontalCollision->GetBoundBox();
    if (player->GetX() > previousX)
      player->SetPosition(objBounds.x - playerBounds.width, player->GetY());
    else
      player->SetPosition(objBounds.x + objBounds.width, player->GetY());
  }

  // Clamp player within level bounds
  float minX = 0.0f;
  float maxX = levelWidth - player->GetWidth();
  if (player->GetX() < minX)
    player->SetPosition(minX, player->GetY());
  else if (player->GetX() > maxX)
    player->SetPosition(maxX, player->GetY());

  // FIXED: Vertical collision and ground detection
  Collision *verticalCollision = collisionManager->CheckVerticalCollision(playerBounds);
  if (verticalCollision)
  {
    Rectangle objBounds = verticalCollision->GetBoundBox();

    // Check if player is landing on top of the platform
    float playerBottom = playerBounds.y + playerBounds.height;
    float objTop = objBounds.y;

    // If player is on top of the platform (with some tolerance)
    if (playerBottom >= objTop - 5.0f && playerBottom <= objTop + 10.0f)
    {
      player->SetPosition(player->GetX(), objBounds.y - playerBounds.height);
      player->SetYVelocity(0.0f);
      player->SetOnGround(true);
    }
    else
    {
      // Player is colliding but not on top (e.g., hitting from side)
      player->SetOnGround(false);
    }
  }
  else
  {
    // No vertical collision, player is in air
    player->SetOnGround(false);
  }

  // Update collision based on current camera position
  collisionManager->Update(camera.target.x);

  // ==== 📷 CAMERA: METAL SLUG STYLE FOLLOW ====
  float halfScreenWidth = screenWidth / 2.0f;
  Vector2 playerCenter = {
      player->GetX() + player->GetWidth() / 2.0f,
      player->GetY() + player->GetHeight() / 2.0f};

  // Metal Slug style: Camera follows player but with some lag
  float followSpeed = 0.05f; // Slower follow for Metal Slug feel
  float targetCameraX = playerCenter.x;

  // Create a "dead zone" in the center where camera doesn't move
  float deadZoneWidth = screenWidth * 0.3f; // 30% of screen width
  float cameraLeftBound = camera.target.x - deadZoneWidth / 2.0f;
  float cameraRightBound = camera.target.x + deadZoneWidth / 2.0f;

  // Only move camera if player is outside the dead zone
  if (playerCenter.x < cameraLeftBound)
  {
    targetCameraX = playerCenter.x + deadZoneWidth / 2.0f;
  }
  else if (playerCenter.x > cameraRightBound)
  {
    targetCameraX = playerCenter.x - deadZoneWidth / 2.0f;
  }
  else
  {
    targetCameraX = camera.target.x; // Stay in place
  }

  // Smooth camera movement
  camera.target.x += (targetCameraX - camera.target.x) * followSpeed;

  // Clamp camera to level bounds
  float cameraMinX = halfScreenWidth;
  float cameraMaxX = levelWidth - halfScreenWidth;

  if (camera.target.x < cameraMinX)
  {
    camera.target.x = cameraMinX;
  }
  if (camera.target.x > cameraMaxX)
  {
    camera.target.x = cameraMaxX;
  }

  // Lock Y-axis like Metal Slug (keep camera at fixed height)
  camera.target.y = 350.0f; // Adjust this value to show the right vertical area

  // ✅ Camera delta for parallax update
  static float lastCameraX = camera.target.x;
  float cameraDelta = camera.target.x - lastCameraX;

  // If camera is clamped, use player's movement for parallax
  if (fabs(cameraDelta) < 0.01f)
  {
    float playerDelta = player->GetX() - previousX;
    // Only use player delta if camera is at the boundaries
    if ((camera.target.x <= cameraMinX && playerDelta < 0) ||
        (camera.target.x >= cameraMaxX && playerDelta > 0))
    {
      cameraDelta = playerDelta;
    }
  }

  lastCameraX = camera.target.x;

  // Update layers with delta
  for (auto &layer : mainlayers)
  {
    if (layer)
    {
      layer->UpdateLayer(cameraDelta);
    }
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

  if (collisionManager)
  {
    collisionManager->Draw();
  }

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

#ifdef DEBUG
  DrawText(TextFormat("Player Y: %.2f", player->GetY()), 10, 10, 20, RED);
  DrawText(TextFormat("OnGround: %s", player->IsOnGround() ? "true" : "false"), 10, 30, 20, RED);
  DrawText(TextFormat("Y Velocity: %.2f", player->GetYVelocity()), 10, 50, 20, RED);
  DrawText(TextFormat("Settings: %s", showSettingsPopup ? "true" : "false"), 10, 70, 20, RED);
#endif
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

  if (collisionManager)
  {
    delete collisionManager;
    collisionManager = nullptr;
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