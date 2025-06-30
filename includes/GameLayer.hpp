#pragma once
#include <raylib.h>

class Gamelayer
{
private:
  Texture2D texture;
  float yOffset;
  float scale;
  float scrollX;
  float parallaxSpeed;

public:
  Gamelayer(const char *file, float y, float scal, float parallaxFactor = 1.0f);
  ~Gamelayer();
  void UpdateLayer(float playerSpeed);
  void Drawlayer();
};