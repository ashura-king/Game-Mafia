#pragma once
#include "Layer.hpp"
#include "Controller.hpp"

class Controller;
class SpriteLoader
{
public:
  static void LoadMenuLayer(std::vector<Layer *> &layers, float scale);
  static void LoadIntroLayer(std::vector<Layer *> &layers, float scale);
  static void LoadMainGameLayer(Controller *controller);
};