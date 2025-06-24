#include <raylib.h>
#pragma once
#include "includes/Character.hpp"
#include "includes/Layer.hpp"
#include "includes/GameLayer.hpp"
#include "includes/Button.hpp"
#include "includes/Popup.hpp"
#include "includes/GameType.hpp"
#include "includes/Bot.hpp"
#include "includes/TextOutlined.hpp"
#include "includes/Civillian.hpp"
#include "includes/ThugBot.hpp"
#include "includes/GangsterBot.hpp"
#include "includes/SettingMenu.hpp"
#include "includes/SettingPop.hpp"
#include "includes/SettingMenu.hpp"
#include <vector>
#include <string>
#include <algorithm>

class Controller
{
public:
  Controller();
  ~Controller() = default;

  void Init(int screenW, int screenH, int originalW, int originalH);
  void Update();
  void Draw();
  void Unload();
  void SpawnBots(int count);
  const char *GetBotStateText(BotState state);
  const float SPAWN_SAFE_DISTANCE = 200.0f;
  const int MAX_BOTS_PER_TYPE = 6;
  const float BOT_INTERACTION_RANGE = 100.0f;

private:
  // Screen properties
  int screenWidth, screenHeight;
  int originalWidth, originalHeight;
  float scaleX, scaleY, scale;

  // Game state
  Gamestate currentState;

  // Audio
  Sound clickSound;
  Music backgroundMusic;
  Music playingMusic;
  bool playingMusicStarted;
  bool menuMusicStarted;

  // Textures
  Texture2D titleTexture;
  float titleScale;
  Vector2 titlePosition;

  // Game objects
  Character *player;
  std::vector<Bot *> bots;

  // Layers
  std::vector<Layer *> menuLayers;
  std::vector<Layer *> gameLayers;
  std::vector<Gamelayer *> mainlayers;

  // UI Elements
  Button *startButton;
  Button *exitButton;
  Button *yesButton;
  Button *noButton;
  Popup popup;
  SettingPop settingpop;
  bool showSettingsPopup = false;
  Button *resumeButton;
  Button *backToMenuButton;

  // State management
  int frameCounter;
  int dotCount;
  int maxDots;
  std::string animatedText;
  int gameTimer;
  int fadeDuration;
  bool fadeOutComplete;
  bool running;
  bool showExitPop;
  SettingMenu *settingIcon = nullptr;

  Vector2 GetSafeSpawnPosition(Vector2 playerPos, float minDistance);
  bool IsPositionOccupied(Vector2 position);
  void UpdateBotAI(Vector2 playerPos, float deltaTime);
  void UpdateBotSpecificBehavior(Bot *bot, Vector2 playerPos,
                                 float deltaTime, const std::vector<Bot *> &allBots);
  void HandleBotPlayerInteractions(Vector2 playerPos);
  void HandlePlayerDamage(Bot *attackingBot);
  void HandleBotInteractions();
  void HandleBotPairInteraction(Bot *bot1, Bot *bot2, float distance);
  void CleanupAndRespawnBots();
  void DrawBotsWithTacticalInfo();
  void DrawBotTacticalInfo(Bot *bot);
  void DrawBotStatusHUD();

  // Utility methods
  const char *GetBotTypeName(BotType type);
  int CountBotsByType(BotType type) const;

  // Update methods
  void UpdateMenu();
  void UpdateGame();
  void UpdatePlaying();

  // Draw methods
  void DrawMenu();
  void DrawGame();
  void DrawPlaying();
};