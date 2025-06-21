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

class GameController
{
public:
void Init(),
    void Update(),
    void Draw(),
    void Unload(),

    private : GameState currentState;
  // core
  Character player;
  std::vector<Bot> bots;
  void SpawnBots(int count);
  // UI
  Button startButton, exitButton, yesButton, noButton;
  Popup popup;

  Sound clickSound;
  Music BackgroundMusic;
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

  void UpdateMenu();
  void UpdatGame();
  void UpdatePlaying();
  void DrawMenu();
  void DrawGame();
  void DrawPlaying();
}