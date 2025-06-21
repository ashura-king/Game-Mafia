#include "includes/Controller.hpp"

void Controller::Init()
{

  currentState = GameState::MENU;
  // Initialize menu
  clickSound = LoadSound("AUdio/start.mp3");
  BackgroudMusic = LoadMusicStream("Audio/Intro1.mp3");
  playingMusic = LoadMusicStream("Audio/PlayingSound.mp3");
  titleTexture = LoadTexture("resource/TitleGame.png");
  titleScale = 3.0f;
  titlePosition = {(screenWidth - (titleTexture.width * titleScale)) / 2.0f, 20.0f * scale};

  Character player("resource/player/Idle.png",
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
  player.SetJumpSpeed(15.0f);
  player.SetGravity(0.8f);
  player.SetGroundY(270.0f);
  player.SetFireCooldown(0.3f);
  player.SetGunshotVolume(0.7f);

  // Initialize the layers
  menuLayers = {
      new Layer("resource/Sky_pale.png", 0.1f, 0, scale),
      new Layer("resource/back.png", 0.5f, 0, scale),
      new Layer("resource/Houses3_pale.png", 1.0f, 70, scale),
      new Layer("resource/houses1.png", 1.0f, 70, scale),
      new Layer("resource/minishop&callbox.png", 1.0f, 80, scale),
      new Layer("resource/road&lamps.png", 1.0f, 75, scale)};

  gameLayers = {
      new Layer("resource/sky.png", 0.1f, 0, scale),
      new Layer("resource/houses3.png", 0.5f, 0, scale),
      new Layer("resource/night2.png", 1.0f, 70, scale),
      new Layer("resource/night.png", 1.0f, 75, scale),
      new Layer("resource/road.png", 1.0f, 75, scale),
      new Layer("resource/crosswalk.png", 1.0f, 70, scale)};

  mainlayers = {
      new Gamelayer("resource/mainsky.png", 0.0f, scale),
      new Gamelayer("resource/housemain2.png", 0.0f, scale),
      new Gamelayer("resource/housemain.png", 0.0f, scale),
      new Gamelayer("resource/housemain1.png", 0.0f, scale),
      new Gamelayer("resource/fountain&bush.png", 0.0f, scale),
      new Gamelayer("resource/policebox.png", 0.0f, scale),
      new Gamelayer("resource/mainroad.png", 0.0f, scale)};
  // Initialize button
  startButton = Button("resource/button1.png", "resource/button2.png", "resource/button3.png", scale * 5.0f, true, 70.0f);
  exitButton = Button("resource/exit1.png", "resource/exit2.png", "resource/exit3.png", scale * 5.0f, true, 160.0f);
  yesButton = Button("resource/yes.png", "resource/yes2.png", "resource/yes3.png", 2.5f);
  noButton = Button("resource/no.png", "resource/no2.png", "resource/no3.png", 2.5f);

  // Init State Helper
  int frameCounter = 0;
  int dotCount = 0;
  const int maxDots = 3;
  std::string animatedText = " ";
  int gameTimer = 0;
  const int fadeDuration = 300;
  bool fadeOutComplete = false;
  bool playingMusicStarted = false;
  bool running = true;
  bool showExitPop = false;
  Popup popup;
  playMusicStream(BackgroundMusic);

  SpawnBots(10);
}

void Controller::SpawnBots(int count)
{
  bots.clear();
  for (int i = 0; i < count; ++i)
  {
    float x = GetRandomValue(100, GetScreenWidth() - 300);
    float y = GetRandomValue(100, GetScreenHeight() - 300);
    BotType type = static_cast<BotType>(GetRandomValue(0, 3));
    bots.emplace_back(type, x, y);
  }
}
void Controller::Update()
{
  switch (currentState)
  {
  case GameState::MENU:
    UpdateMenu();
    break;
  case Gamestate::GAME:
    UpdateGame();
    break;
  case GameState::PLAYING:
    UpdatePlaying();
    break;
  default:
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
  case GameState::PLAYING:
    DrawPlaying();
    break;
  }
  EndDrawing();
}

void Controller::UpdateMenu()
{
  UpdateMusicStream(backgroundMusic);
  for (Layer *layer : menuLayers)
  {
    layer->Update();
  }
  if (!showExitPop)
  {
    startButton.Update();
    exitButton.Update();

    if (startButton.IsClicked())
    {
      PlaySound(clickSound);
      currentState = Gamestate::GAME;
      gameTimer = 0;
      fadeOutComplete = false;
    }

    if (exitButton.IsClicked())
    {
      Playsound(clickSound);
      showExitPop = true;
    }
  }
}
void Controller::UpdateGame()
{
  for (Layer *layer : gameLayers)
  {
    layer->Update();
  }
  frameCount++;
  if (frameCount >= 30)
  {
    frameCounter = 0;
    dotCount = (dotCount + 1) % (maxDots + 1);
    animatedText = "Please wait" + std::string(dotCount, '.');
  }
  if (!fadeOutComplete)
  {
    gameTimer++;
    float volume = 1.0f - (float)gameTimer / fadeDuration;
    SteMusicVolume(backgroundMusic, volume < 0 ? 0 : volume);

    if (gameTimer >= fadeDuration)
    {
      fadeOutComplete = true;
      currentState = GameState::PLAYING:
    }
  }
}
void Controller::UpdatePlaying()
{
}