#pragma once

#include "raylib.h"
#include "raymath.h"
#include "includes/GameType.hpp"
#include <vector>

// Base Bot class that all specific bot types inherit from
class Bot
{
public:
  // Constructor & virtual destructor
  Bot(float startX, float startY);
  virtual ~Bot();

  // Virtual methods for customization in derived classes
  virtual void LoadTextures();
  virtual void SetProperties();
  virtual BotType GetBotType() const;

  virtual void Update();
  virtual void UpdateAI(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &otherBots);
  virtual void Draw();

  // Virtual movement/behavior (can be overridden)
  virtual void ChasePlayer(Vector2 playerPos, const std::vector<Bot *> &otherBots);
  virtual void Wander(float deltaTime, const std::vector<Bot *> &otherBots);
  virtual void MoveTowards(Vector2 target);
  virtual void MoveAway(Vector2 threat);
  virtual void Patrol();

  // Virtual combat (e.g., override for civilians or advanced bots)
  virtual void Attack();
  virtual void TakeDamage(int damage);

  // State control
  void SetState(BotState newState);
  BotState GetState() const { return state; }

  // Combat conditions
  bool CanAttack() const;

  // Utility functions
  float DistanceTo(Vector2 target) const;
  bool IsPlayerInRange(Vector2 playerPosition, float range) const;
  bool CheckCollisionWithPlayer(Vector2 playerPos, float playerWidth, float playerHeight);
  bool IsAlive() const { return health > 0; }
  bool IsSpawned() const { return isSpawned; }

  // Collision with other bots
  bool WouldCollideWithBots(Vector2 position, const std::vector<Bot *> &otherBots) const;
  Vector2 GetAvoidanceDirection(Vector2 blockedPosition, const std::vector<Bot *> &otherBots) const;

  // Animation and visual helpers
  void UpdateAnimations();
  void GetTextureAndAnimation(Texture2D &texture, Rectangle &source);
  void UpdateDirection(Vector2 movementVector);

  // E-SWAT Tactical System Methods
  virtual void ExecuteESWATTactics(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots);
  void AssignESWATRole(const std::vector<Bot *> &allBots, Vector2 playerPos);
  Vector2 GetESWATPosition(Vector2 playerPos, const std::vector<Bot *> &allBots);
  bool CheckESWATCoordination(const std::vector<Bot *> &allBots);
  void ExecutePositioning(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots);
  void ExecuteCoordinatedAttack(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots);
  void ExecuteRetreatRegroup(Vector2 playerPos, float deltaTime, const std::vector<Bot *> &allBots);
  void CircleStrikePlayer(Vector2 playerPos, float deltaTime);

  // Public properties (you may later wrap these in getters/setters)
  float x, y;
  float width, height;
  float speed;
  Direction direction;
  int health, maxHealth;
  Vector2 GetPosition() const { return {x, y}; };
  bool IsInPosition() const { return isInPosition; };
  bool isInPosition;

protected:
  // Texture resources (to be loaded in LoadTextures)
  Texture2D idleTexture;
  Texture2D idleLeftTexture;
  Texture2D walkTexture;
  Texture2D runTexture;
  Texture2D attackTexture;

  // Animation structures
  Animation idleRightAnim;
  Animation idleLeftAnim;
  Animation walkAnim;
  Animation runAnim;
  Animation attackAnim;

  // Core AI State
  BotState state;
  BotState previousState;
  float stateTimer;

  // AI Behavior Config
  float chaseRange;
  float attackRange;
  float fleeingRange;
  float wanderTime;
  float wanderTimer;
  Vector2 wanderTarget;

  // Patrol behavior
  std::vector<Vector2> patrolWaypoints;
  int currentWaypointIndex;
  float waypointReachDistance;

  // Combat status
  bool isAttacking;
  float attackTimer;
  float attackCooldown;

  // Spawn control
  float spawnDelay;
  float spawnTimer;
  bool isSpawned;

  // Direction/animation logic
  Vector2 lastValidDirection;
  float directionChangeTimer;
  static constexpr float DIRECTION_CHANGE_DELAY = 0.2f;

  // Initialization status
  bool isLoaded;

  // Bot identity
  BotType type;

  // E-SWAT Tactical System Variables
  TacticalRole tacticalRole;
  TacticalPhase tacticalPhase;
  Vector2 tacticalTarget;
  Vector2 circleCenter;
  float circleRadius;
  float circleAngle;
  float tacticalTimer;
  bool waitingForSignal;
  int groupId;
  static int nextGroupId;

  // E-SWAT Constants
  static constexpr float ESWAT_CIRCLE_RADIUS = 200.0f;
  static constexpr float ESWAT_ATTACK_DISTANCE = 80.0f;
  static constexpr float ESWAT_POSITIONING_SPEED = 1.3f;
  static constexpr float ESWAT_ATTACK_SPEED = 1.8f;
  static constexpr float ESWAT_COORDINATION_TIME = 2.0f;

private:
  void LoadTexturesSafe();
};