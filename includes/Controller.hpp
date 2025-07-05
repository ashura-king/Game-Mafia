#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include <algorithm>

#include "Bots/Character.hpp"
#include "Layer/GameLayer.hpp"
#include "Buttons/Button.hpp"
#include "Buttons/Popup.hpp"
#include "GameType.hpp"
#include "Bots/Bot.hpp"
#include "Layer/TextOutlined.hpp"
#include "Buttons/SettingMenu.hpp"
#include "Buttons/SettingPop.hpp"
#include "Bots/BotSpawner.hpp"
#include "Layer/SpriteLoader.hpp"
#include "Layer/ButtonSprite.hpp"
#include "Platform/GameObject.hpp"
#include "Platform/ObjectSpawner.hpp"

class Controller
{
public:
  Controller();
  ~Controller() = default;

  void Init(int screenW, int screenH, int originalW, int originalH);
  void Update();
  void Draw();
  void Unload();

  Camera2D camera;
  float levelWidth = 0.0f;
  void AddGamelayer(const std::string &file);
  bool IsRunning() const { return running; }

private:
  ObjectSpawner *objectSpawner;
  GameObject *gameObjectManager;
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

  // Background Layers
  std::vector<Layer *> menuLayers;
  std::vector<Layer *> gameLayers;
  std::vector<Gamelayer *> mainlayers;

  // UI Elements
  Button *startButton;
  Button *exitButton;
  Button *yesButton;
  Button *noButton;
  Button *resumeButton;
  Button *backToMenuButton;
  Popup popup;
  SettingPop settingpop;
  SettingMenu *settingIcon = nullptr;

  // UI State
  bool showSettingsPopup = false;
  bool showExitPop;
  bool running;

  // State helpers
  int frameCounter;
  int dotCount;
  int maxDots;
  std::string animatedText;
  int gameTimer;
  int fadeDuration;
  bool fadeOutComplete;

  // Update methods
  void UpdateMenu();
  void UpdateGame();
  void UpdatePlaying();

  // Draw methods
  void DrawMenu();
  void DrawGame();
  void DrawPlaying();

  // Bot utility
  BotSpawner *spawner;
};
