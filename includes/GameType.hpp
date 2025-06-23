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

enum class BotType
{
  CIVILIAN,
  THUG,
  SWAT,

};
enum class BotState
{
  IDLE,
  WANDERING,
  CHASING,
  ATTACK,
  FLEEING,
  DEAD,
  SPAWNING,
  PATROLLING,
  TACTICAL_POSITIONING,
  COORDINATED_ATTACK,
  RETREATING
};
enum class TacticalRole
{
  DIRECT_ASSAULT, // Attacks head-on like E-SWAT front units
  LEFT_FLANKER,   // Circles to attack from left
  RIGHT_FLANKER,  // Circles to attack from right
  REAR_AMBUSH,    // Moves behind for surprise attack
  SUPPORT_FIRE    // Provides covering fire from distance
};

enum class TacticalPhase
{
  POSITIONING,        // Moving to tactical position
  COORDINATED_ATTACK, // Synchronized attack phase
  RETREAT_REGROUP
};
enum Direction
{
  LEFT = -1,
  RIGHT = 1,
  UP,
  DOWN
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