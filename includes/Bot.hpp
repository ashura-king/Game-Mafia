#ifndef BOT_H
#define BOT_H

#include <raylib.h>
#include "GameType.hpp"
#include <vector>
#include <string>

class Bot
{
private:
  // Textures and animations
  Texture2D idleTexture;
  Texture2D idleLeftTexture;
  Texture2D walkTexture;
  Texture2D runTexture;
  Texture2D attackTexture;

  Animation idleRightAnim;
  Animation idleLeftAnim;
  Animation walkAnim;
  Animation runAnim;
  Animation attackAnim;

  // Position and movement
  float x, y;
  float width, height;
  float speed;
  Direction direction;

  // AI state management
  BotState state;
  BotState previousState;
  float stateTimer;

  // AI behavior parameters - using your values
  float chaseRange = 200.0f;
  float attackRange = 80.0f;
  float fleeingRange = 400.0f;
  float wanderTime = 3.0f;
  float moveTime = 3.0f;
  float patrolStartX = 100.0f;
  float PatrolStartY = 600.0f;

  Vector2 targetPosition;
  Vector2 wanderTarget;
  float wanderTimer;

  // Patrol system
  std::vector<Vector2> patrolWaypoints;
  int currentWaypointIndex;
  float waypointReachDistance;

  // Combat
  bool isAttacking;
  float attackTimer;
  float attackCooldown;
  int health;
  int maxHealth;

  // Utility
  bool isLoaded;

public:
  // Constructor
  Bot(const std::string &idleRightPath,
      const std::string &idleLeftPath,
      const std::string &walkPath,
      const std::string &runPath,
      const std::string &attackPath,
      float startX,
      float startY,
      float botSpeed = 2.0f);

  // Destructor
  ~Bot();

  // Main update functions
  void Update();
  void UpdateAI(Vector2 playerPos, float deltaTime);
  void Draw();

  // AI State methods
  void SetState(BotState newState);
  BotState GetState() const { return state; }

  // AI Behaviors - using your method names
  void UpdateIdle();
  void UpdateWandering();
  void chasePlayer(Vector2 playerPos);
  void wander(float deltaTime);
  void Patrol();
  void UpdateFleeing(Vector2 threatPosition);

  // Movement methods
  void MoveTowards(Vector2 target);
  void MoveAway(Vector2 threat);
  void StopMoving();

  // Patrol system
  void SetPatrolWaypoints(const std::vector<Vector2> &waypoints);
  void UpdatePatrol();

  // Combat
  void Attack();
  bool CanAttack() const;
  void TakeDamage(int damage);
  bool IsAlive() const { return health > 0; }

  // Utility functions
  float DistanceTo(Vector2 target) const;
  bool IsPlayerInRange(Vector2 playerPosition, float range) const;
  bool CheckCollisionWithPlayer(Vector2 playerPos, float playerWidth, float playerHeight);
  Vector2 GetPosition() const { return {x, y}; }
  Rectangle GetBounds() const { return {x, y, width, height}; }

  // Animation helpers
  void UpdateAnimations();
  CharacterState GetCurrentCharacterState() const;
  void GetTextureAndAnimation(Texture2D &texture, Rectangle &source);
};

#endif