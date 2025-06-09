#include "raylib.h"
#include "includes/Player.hpp"

Player::Player()
{
  speed = 0.0f;
}

void Player::Update()
{
  speed = 0.0f;
  if (IsKeyDown(KEY_RIGHT))
    speed = 5.0f;
  else if (IsKeyDown(KEY_LEFT))
    speed = -5.0f;
}

float Player::GetSpeed() const
{
  return speed;
}
