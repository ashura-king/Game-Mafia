#include "includes/Gunfire.hpp"

Gunfire::Gunfire(Texture2D tex, Vector2 pos, float spd, int dir)
    : position(pos), speed(spd), direction(dir), active(true),
      bulletTexture(tex), frameCount(3), currentFrame(0),
      frameTime(0.1f), frameTimer(0.0f)
{
  frameWidth = static_cast<float>(tex.width) / frameCount;
  frameHeight = static_cast<float>(tex.height);
  frameRec = {0.0f, 0.0f, frameWidth, frameHeight};
}

void Gunfire::Update(const Camera2D &camera)
{
  // Move
  position.x += speed * direction;

  // Animate
  frameTimer += GetFrameTime();
  if (frameTimer >= frameTime)
  {
    frameTimer = 0.0f;
    currentFrame = (currentFrame + 1) % frameCount;
    frameRec.x = currentFrame * frameWidth;
  }

  // Camera bounds check for world scrolling
  Vector2 camMin = {
      camera.target.x - camera.offset.x / camera.zoom,
      camera.target.y - camera.offset.y / camera.zoom};
  Vector2 camMax = {
      camera.target.x + camera.offset.x / camera.zoom,
      camera.target.y + camera.offset.y / camera.zoom};

  if (position.x < camMin.x - frameWidth || position.x > camMax.x + frameWidth)
  {
    active = false;
  }
}

void Gunfire::Draw()
{
  if (!active)
    return;

  Rectangle drawRec = frameRec;
  if (direction == -1)
  {
    drawRec.width = -frameRec.width;
    drawRec.x += frameRec.width;
  }

  DrawTextureRec(bulletTexture, drawRec, position, WHITE);
}
