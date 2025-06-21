#ifndef BOT_HPP
#define BOT_HPP

#include <raylib.h>
#include "GameType.hpp"
#include <vector>

class Bot
{
private:
  // Bot configuration
  BotType type;

  // Graphics resources
  Texture2D idleTexture;
  Texture2D idleLeftTexture;
  Texture2D walkTexture;
  Texture2D runTexture;
  Texture2D attackTexture;

  // Animation states
  Animation idleRightAnim;
  Animation idleLeftAnim;
  Animation walkAnim;
  Animation runAnim;
  Animation attackAnim;

  // Transform properties
  float x, y;
  float width, height;
  float speed;
  Direction direction;

  // AI state system
  BotState state;
  BotState previousState;
  float stateTimer;
  float idleTime = 2.0f;

  // AI behavior parameters
  float chaseRange;
  float attackRange;
  float fleeingRange;
  float wanderTime;

  // Wandering behavior
  Vector2 wanderTarget;
  float wanderTimer;

  // Patrol system
  std::vector<Vector2> patrolWaypoints;
  int currentWaypointIndex;
  float waypointReachDistance;

  // Combat system
  bool isAttacking;
  float attackTimer;
  float attackCooldown;
  int health;
  int maxHealth;

  // Initialization state
  bool isLoaded;

  // Private helper methods
  void SetBotProperties(BotType botType);
  void UpdateAnimations();
  void GetTextureAndAnimation(Texture2D &texture, Rectangle &source);

public:
  // Constructor & Destructor
  Bot(BotType botType, float startX, float startY);
  ~Bot();

  // Core update loop
  void Update();
  void UpdateAI(Vector2 playerPos, float deltaTime);
  void Draw();

  // State management
  void SetState(BotState newState);
  BotState GetState() const { return state; }

  // AI behaviors
  void chasePlayer(Vector2 playerPos);
  void wander(float deltaTime);
  void Patrol();

  // Movement system
  void MoveTowards(Vector2 target);
  void MoveAway(Vector2 threat);

  // Combat system
  void Attack();
  bool CanAttack() const;
  void TakeDamage(int damage);
  bool IsAlive() const { return health > 0; }

  // Utility functions
  float DistanceTo(Vector2 target) const;
  bool IsPlayerInRange(Vector2 playerPosition, float range) const;
  bool CheckCollisionWithPlayer(Vector2 playerPos, float playerWidth, float playerHeight);

  // Getters
  Vector2 GetPosition() const { return {x, y}; }
  Rectangle GetBounds() const { return {x, y, width, height}; }
  BotType GetType() const { return type; }
  int GetHealth() const { return health; }
  int GetMaxHealth() const { return maxHealth; }
};

#endif