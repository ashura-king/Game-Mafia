#pragma once
#include "Buttons/Button.hpp"
#include "Buttons/SettingMenu.hpp"
#include <raylib.h>
#include <string>

class ButtonSprite
{
public:
  static Button *LoadButton(const std::string &normal, const std::string &hover, const std::string &click, float scale, bool center, float yOffSet);
  static Button *LoadButton(const std::string &normal, const std::string &hover, const std::string &click, float scale);
  static SettingMenu *LoadSettingMenu(const std::string &first, const std::string &hover, const std::string &click, float scale, bool useHover, bool useClick);

  static void LoadSprites(Button *&startButton, Button *&exitButton, Button *&yesButton, Button *&noButton,
                          Button *&resumeButton, Button *&backToMenuButton, SettingMenu *&settingIcon,
                          float scale);
};
