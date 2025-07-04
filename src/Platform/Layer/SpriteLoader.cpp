#include "Layer/SpriteLoader.hpp"
#include "includes/Controller.hpp"
#include <raylib.h>

static void AddLayer(std::vector<Layer *> &list, const std::string &file, float speed, float yOffSet, float scale)
{
  if (FileExists(file.c_str()))
    list.push_back(new Layer(file.c_str(), speed, yOffSet, scale));
};
void SpriteLoader::LoadMenuLayer(std::vector<Layer *> &layers, float scale)
{
  AddLayer(layers, "resource/texture/Sky_pale.png", 0.1f, 0, scale);
  AddLayer(layers, "resource/texture/back.png", 0.5f, 0, scale);
  AddLayer(layers, "resource/texture/Houses3_pale.png", 1.0f, 70, scale);
  AddLayer(layers, "resource/texture/houses1.png", 1.0f, 70, scale);
  AddLayer(layers, "resource/texture/minishop&callbox.png", 1.0f, 80, scale);
  AddLayer(layers, "resource/texture/road&lamps.png", 1.0f, 75, scale);
};
void SpriteLoader::LoadIntroLayer(std::vector<Layer *> &layers, float scale)
{
  AddLayer(layers, "resource/texture/sky.png", 0.1f, 0, scale);
  AddLayer(layers, "resource/texture/houses3.png", 0.5f, 0, scale);
  AddLayer(layers, "resource/texture/night2.png", 1.0f, 70, scale);
  AddLayer(layers, "resource/texture/night.png", 1.0f, 75, scale);
  AddLayer(layers, "resource/texture/road.png", 1.0f, 75, scale);
  AddLayer(layers, "resource/texture/crosswalk.png", 1.0f, 70, scale);
};
void SpriteLoader::LoadMainGameLayer(Controller *controller)
{
  controller->AddGamelayer("resource/texture/PMSky.png");
  controller->AddGamelayer("resource/texture/PMBack.png");
  controller->AddGamelayer("resource/texture/Middle.png");
  controller->AddGamelayer("resource/texture/Front.png");
  controller->AddGamelayer("resource/texture/MiddleDesign.png");
  controller->AddGamelayer("resource/texture/FrontDesign.png");
  controller->AddGamelayer("resource/texture/PMroad.png");
};