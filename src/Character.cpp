#include "includes/Character.hpp"
#include <raylib.h>

Character::Character(const std::string &idlePath,
                     const std::string &idleLeftPath,
                     const std::string &walkPath,
                     const std::string &shot,
                     const std::string &jump,
                     float startX,
                     float startY,
                     float characterSpeed)
    : x(startX),
      y(startY),
      speed(characterSpeed),
      direction(Right),
      isWalking(false),
      isJumping(false),
      isOnGround(true),
      isFiring(false),
      jumpVelocity(0.0f),
      gravity(0.8f),
      jumpSpeed(15.0f),
      fireTimer(0.0f),
      fireCooldown(0.3f),
      isLoaded(true)
{
  groundY = startY;

  idleTexture = LoadTexture(idlePath.c_str());
  idleLeftTexture = LoadTexture(idleLeftPath.c_str());
  walkTexture = LoadTexture(walkPath.c_str());

  if (idleTexture.id == 0)
  {
    TraceLog(LOG_ERROR, "Failed to load texture: %s", idlePath.c_str());
    isLoaded = false;
  }
  if (idleLeftTexture.id == 0)
  {
    TraceLog(LOG_ERROR, "Failed to load texture: %s", idleLeftPath.c_str());
    isLoaded = false;
  }
  if (walkTexture.id == 0)
  {
    TraceLog(LOG_ERROR, "Jump texture failed to load: %s", jump.c_str());
    isLoaded = false;
  }

  if (!shot.empty())
  {
    shotTexture = LoadTexture(shot.c_str());
    if (shotTexture.id == 0)
    {
      TraceLog(LOG_ERROR, "Failed to load texture: %s", shot.c_str());
    }
  }
  else
  {
    shotTexture = {0};
  }

  if (!jump.empty())
  {
    jumpTexture = LoadTexture(jump.c_str());
    if (jumpTexture.id == 0)
    {
      TraceLog(LOG_ERROR, "Failed to load texture: %s", jump.c_str());
    }
  }
  else
  {
    jumpTexture = {0};
  }

  width = 128 * 2;
  height = 128 * 2;

  idleRightAnim = {0, 4, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  idleLeftAnim = {0, 4, 0, 0.15f, 0.15f, 1, AnimationType::REPEATING};
  walkAnim = {0, 5, 0, 0.08f, 0.08f, 1, AnimationType::REPEATING};
  jumpAnim = {0, 9, 0, 0.1f, 0.1f, 1, AnimationType::ONESHOT};
  shotAnim = {0, 4, 0, 0.05f, 0.05f, 1, AnimationType::ONESHOT};
}

Character::~Character()
{
  if (isLoaded)
  {
    if (idleTexture.id != 0)
      UnloadTexture(idleTexture);
    if (idleLeftTexture.id != 0)
      UnloadTexture(idleLeftTexture);
    if (walkTexture.id != 0)
      UnloadTexture(walkTexture);
    if (jumpTexture.id != 0)
      UnloadTexture(jumpTexture);
    if (shotTexture.id != 0)
      UnloadTexture(shotTexture);
  }
}

void Character::Update()
{
  UpdateAnimations();
  UpdateJumpAnimation(); // Added: update jump physics
  UpdateShotAnimation(); // Added: update shot timer
}

void Character::HandleInput()
{
  isWalking = false;

  if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
  {
    MoveRight();
  }
  else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
  {
    MoveLeft();
  }
  else
  {
    StopMoving();
  }
  if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
  {
    Jump();
  }

  if (IsKeyDown(KEY_J) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
  {
    Shot();
  }
}

void Character::UpdatePosition(float deltaX)
{
  x += deltaX;
  if (deltaX != 0)
  {
    isWalking = true;
    direction = (deltaX > 0) ? Right : Left;
  }
  else
  {
    isWalking = false;
  }
}

void Character::UpdateAnimations()
{
  if (isFiring)
  {
    Animation_Update(&shotAnim);
  }
  else if (isJumping)
  {
    Animation_Update(&jumpAnim);
  }
  else if (isWalking)
  {
    Animation_Update(&walkAnim);
  }
  else
  {
    if (direction == Right)
    {
      Animation_Update(&idleRightAnim);
    }
    else
    {
      Animation_Update(&idleLeftAnim);
    }
  }
}

void Character::UpdateJumpAnimation()
{
  if (isJumping)
  {
    jumpVelocity += gravity;
    y += jumpVelocity;

    if (y >= groundY)
    {
      y = groundY;
      isJumping = false;
      isOnGround = true;
      jumpVelocity = 0.0f;
    }
  }
}

void Character::UpdateShotAnimation()
{
  if (isFiring)
  {
    fireTimer -= GetFrameTime();
    if (fireTimer <= 0.0f)
    {
      isFiring = false;

      shotAnim.curr = shotAnim.first;
      shotAnim.duration_left = shotAnim.speed;
    }
  }
}

void Character::MoveLeft()
{
  x -= speed;
  direction = Left;
  isWalking = true;
}

void Character::MoveRight()
{
  x += speed;
  direction = Right;
  isWalking = true;
}

void Character::StopMoving()
{
  isWalking = false;
}

void Character::Jump()
{
  if (isOnGround)
  {
    isJumping = true;
    isOnGround = false;
    jumpVelocity = -jumpSpeed;

    jumpAnim.curr = jumpAnim.first;
    jumpAnim.duration_left = jumpAnim.speed;
  }
}

void Character::Shot()
{
  if (!isFiring) // Fixed: changed condition to prevent continuous firing
  {
    isFiring = true;
    fireTimer = fireCooldown;
    shotAnim.curr = shotAnim.first;
    shotAnim.duration_left = shotAnim.speed;
  }
}

void Character::SetPosition(float newX, float newY)
{
  x = newX;
  y = newY;
}

void Character::SetDirection(Direction newDirection)
{
  direction = newDirection;
}

void Character::SetSize(float newWidth, float newHeight)
{
  width = newWidth;
  height = newHeight;
}

void Character::Draw()
{
  if (!isLoaded)
    return;

  Texture2D currentTexture;
  Rectangle source;
  if (isFiring)
  {
    currentTexture = shotTexture;
    source = animation_frame(&shotAnim, 128, 128);

    if (direction == Left)
    {
      source.width = -source.width;
    }
  }
  else if (isJumping && jumpTexture.id != 0)
  {
    currentTexture = jumpTexture;
    source = animation_frame(&jumpAnim, 128, 128);

    // Ensure width is valid before flipping
    if (direction == Left)
    {
      source.width = -128;
    }
    else
    {
      source.width = 128;
    }
  }

  else if (isWalking)
  {
    currentTexture = walkTexture;
    source = animation_frame(&walkAnim, 128, 128);

    // Flip sprite for left direction
    if (direction == Left)
    {
      source.width = -source.width;
    }
  }
  else
  {
    if (direction == Right)
    {
      currentTexture = idleTexture;
      source = animation_frame(&idleRightAnim, 128, 128);
    }
    else
    {
      currentTexture = idleLeftTexture;
      source = animation_frame(&idleLeftAnim, 128, 128);
    }
  }

  Rectangle dest = {x, y, width, height};
  Vector2 origin = {0, 0};

  DrawTexturePro(currentTexture, source, dest, origin, 0.0f, WHITE);
}