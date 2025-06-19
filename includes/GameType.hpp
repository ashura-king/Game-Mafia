#ifndef GAME_TYPES_HPP
#define GAME_TYPES_HPP

#include <raylib.h>
#include <vector>

enum class Gamestate
{
  MENU,
  GAME,
  PLAYING
};

enum class AnimationType
{
  REPEATING,
  ONESHOT
};

enum class CharacterState
{
  ATTACKING,
  FIRING,
  JUMPING,
  RUNNING,
  WALKING,
  IDLE_RIGHT,
  IDLE_LEFT
};

enum class BotState
{
  IDLE,
  WANDERING,
  CHASING,
  FLEEING,
};

enum Direction
{
  LEFT = -1,
  RIGHT = 1
};

struct Animation
{
  int first;
  int last;
  int curr;
  float speed;
  float duration_left;
  int step;
  AnimationType type;
};

// Forward declarations
class Layer;
class Gamelayer;

// Function declarations
void Animation_Update(Animation *self);
Rectangle animation_frame(Animation *self, int frame_width, int frame_height);
void UpdateAndDrawLayers(const std::vector<Layer *> &layers);

#endif