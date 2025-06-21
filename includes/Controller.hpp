#include <raylib.h>
#include "includes/Layer.hpp"
#include "includes/Button.hpp"
#include "includes/TextOutlined.hpp"
#include "includes/GameLayer.hpp"
#include "includes/GameType.hpp"
#include "includes/Character.hpp"
#include "includes/Bot.hpp"
#include "includes/Popup.hpp"
#include <vector>
#include <string>

class Controller
{
public:
  Controller();
  Controller(const Controller &) = delete;
  Controller &operator=(const Controller &) = delete;
  void Init(int screenW, int screenH, int originalW, int originalH);
  void Update();
  void Draw();
  void Unload();

private:
  Gamestate currentState;
  // core
  Character *player;
  std::vector<Bot> bots;
  void SpawnBots(int count);
  // UI
  Button *startButton, *exitButton, *yesButton, *noButton;
  Popup popup;

  Sound clickSound;
  Music backgroundMusic;
  Music playingMusic;
  // Title
  Texture2D titleTexture;
  Vector2 titlePosition;
  float titleScale;
  // Layers
  std::vector<Layer *> menuLayers;
  std::vector<Layer *> gameLayers;
  std::vector<Gamelayer *> mainlayers;

  std::string animatedText;
  int frameCounter, dotCount, gameTimer;
  bool fadeOutComplete, showExitPop, playingMusicStarted, running;
  int screenWidth, screenHeight;
  int originalWidth, originalHeight;
  float scaleX, scaleY;
  float scale;
  int maxDots;
  int fadeDuration;

  void UpdateMenu();
  void UpdateGame();
  void UpdatePlaying();
  void DrawMenu();
  void DrawGame();
  void DrawPlaying();
};
