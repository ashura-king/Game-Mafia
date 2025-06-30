#ifndef GUNFIRE_H
#define GUNFIRE_H

#include "raylib.h"

class Gunfire
{
public:
  Gunfire(Texture2D tex, Vector2 pos, float spd, int dir);
  void Update(const Camera2D &camera);
  void Draw();

  bool IsActive() const { return active; }

private:
  Vector2 position;
  float speed;
  int direction;

  bool active;

  Texture2D bulletTexture;

  // Animation
  int frameCount;
  int currentFrame;
  float frameTime;
  float frameTimer;

  float frameWidth;
  float frameHeight;
  Rectangle frameRec;
};

#endif
