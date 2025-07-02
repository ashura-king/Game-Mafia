#include "includes/ButtonSprite.hpp"

Button *ButtonSprite::LoadButton(const std::string &normal, const std::string &hover, const std::string &click, float scale, bool center, float yOffSet)
{
  if (FileExists(normal.c_str()) && FileExists(hover.c_str()) && FileExists(click.c_str()))
  {
    return new Button(normal.c_str(), hover.c_str(), click.c_str(), scale, center, yOffSet);
  }
  else
  {
    return nullptr;
  }
};
Button *ButtonSprite::LoadButton(const std::string &normal, const std::string &hover, const std::string &click, float scale)
{
  if (FileExists(normal.c_str()) && FileExists(hover.c_str()) && FileExists(click.c_str()))
  {
    return new Button(normal.c_str(), hover.c_str(), click.c_str(), scale);
  }
  return nullptr;
};
SettingMenu *ButtonSprite::LoadSettingMenu(const std::string &first, const std::string &hover, const std::string &click, float scale, bool useHover, bool useClick)
{
  if (FileExists(first.c_str()) && FileExists(hover.c_str()) && FileExists(click.c_str()))
  {
    // Ensure arguments match the SettingMenu constructor signature
    return new SettingMenu(first.c_str(), hover.c_str(), click.c_str(), scale, useHover, useClick);
  }
  else
  {
    return nullptr;
  }
};
void ButtonSprite::LoadSprites(Button *&startButton, Button *&exitButton, Button *&yesButton, Button *&noButton, Button *&resumeButton, Button *&backToMenuButton, SettingMenu *&settingIcon, float scale)
{
  startButton = LoadButton("resource/texture/button1.png", "resource/texture/button2.png", "resource/texture/button3.png", scale * 5.0f, true, 70.0f);
  exitButton = LoadButton("resource/texture/exit1.png", "resource/texture/exit2.png", "resource/texture/exit3.png", scale * 5.0f, true, 160.0f);
  yesButton = LoadButton("resource/texture/yes.png", "resource/texture/yes2.png", "resource/texture/yes3.png", 2.5f);
  noButton = LoadButton("resource/texture/no.png", "resource/texture/no2.png", "resource/texture/no3.png", 2.5f);
  settingIcon = LoadSettingMenu("resource/texture/gear.png", "resource/texture/gearHover.png", "resource/texture/gearClick.png", 1.0f, true, true);
  resumeButton = LoadButton("resource/texture/resume1.png", "resource/texture/resume2.png", "resource/texture/resume3.png", 2.5f, true, 0.0f);
  backToMenuButton = LoadButton("resource/texture/menu1.png", "resource/texture/menu2.png", "resource/texture/menu3.png", 2.5f, true, 0.0f);
}
