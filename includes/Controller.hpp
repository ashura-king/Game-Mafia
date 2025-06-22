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

  // Textures
  Texture2D titleTexture;
  float titleScale;
  Vector2 titlePosition;

  // Game objects
  Character *player;
  std::vector<Bot *> bots; // Changed from ThugBot to Bot* for polymorphism

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

  // Update methods
  void UpdateMenu();
  void UpdateGame();
  void UpdatePlaying();

  // Draw methods
  void DrawMenu();
  void DrawGame();
  void DrawPlaying();
};