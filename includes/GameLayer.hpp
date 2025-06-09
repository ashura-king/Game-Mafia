#ifndef GAMELAYER_HPP
#define GAMELAYER_HPP

#include "raylib.h"

class Gamelayer
{
public:
  Gamelayer(const char *file, float y, float scal);
  ~Gamelayer();

  void UpdateLayer(float playerSpeed); // pass in player speed
  void Drawlayer();

private:
  Texture2D texture;
  float yOffset;
  float scale;
  float scrollX; // current scroll position
};

#endif
