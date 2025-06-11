#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <raylib.h>
#include "GameType.hpp"
#include <string>

class Character
{
private:
  // Textures
  Texture2D idleTexture;
  Texture2D idleLeftTexture;
  Texture2D walkTexture;
  Texture2D jumpTexture;
  Texture2D shotTexture;
  Sound gunshotSound;
  bool soundLoaded;

  // Animations
  Animation idleRightAnim;
  Animation idleLeftAnim;
  Animation walkAnim;
  Animation jumpAnim;
  Animation shotAnim;

  // Properties
  float x, y;
  float width, height;
  float speed;
  Direction direction;
  bool isWalking;
  bool isLoaded;
  // Jump Properties
  bool isJumping;
  bool isOnGround;
  float jumpVelocity;
  float gravity;
  float groundY;
  float jumpSpeed;
  // Shot properties
  float fireTimer;
  float fireCooldown;
  bool isFiring;

public:
  // Constructor - FIXED parameter order to match implementation
  Character(const std::string &idlePath,
            const std::string &idleLeftPath,
            const std::string &walkPath,
            const std::string &shot = "",             // Fixed: empty string instead of space
            const std::string &jump = "",             // Fixed: empty string instead of space
            const std::string &gunshotSoundPath = "", // Fixed: moved to correct position
            float startX = 0.0f,
            float startY = 0.0f,
            float characterSpeed = 2.0f);

  // Destructor
  ~Character();

  // Update methods
  void Update();
  void HandleInput();                // For player character
  void UpdatePosition(float deltaX); // Manual position update
  void UpdateAnimations();
  void UpdateJumpAnimation();
  void UpdateShotAnimation();

  // Movement methods
  void MoveLeft();
  void MoveRight();
  void StopMoving();
  void Jump();
  void Shot();
  void SetPosition(float newX, float newY);
  void SetDirection(Direction newDirection);
  void PlayGunshotSound();
  void SetGunshotVolume(float volume); // Fixed: added missing parameter
  bool IsGunshotPlaying() const;

  // Rendering
  void Draw();

  // Getters
  float GetX() const { return x; }
  float GetY() const { return y; }
  float GetWidth() const { return width; }
  float GetHeight() const { return height; }
  Direction GetDirection() const { return direction; }
  bool IsWalking() const { return isWalking; }
  bool IsJumping() const { return isJumping; }
  bool IsFiring() const { return isFiring; }
  bool IsOnGround() const { return isOnGround; }
  bool IsLoaded() const { return isLoaded; }

  // Setters
  void SetSpeed(float newSpeed) { speed = newSpeed; }
  void SetJumpSpeed(float newJumpSpeed) { jumpSpeed = newJumpSpeed; }
  void SetGravity(float newGravity) { gravity = newGravity; }
  void SetGroundY(float newGroundY) { groundY = newGroundY; }
  void SetFireCooldown(float newCooldown) { fireCooldown = newCooldown; }
  void SetSize(float newWidth, float newHeight);
};

#endif