#include "Buttons/Popup.hpp"
#include "Buttons/Button.hpp"

void Popup::DrawExitPopup(bool &running, bool &showExitPopup, Sound clickSound, Button &yesButton, Button &noButton)
{

  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));

  int boxWidth = 350;
  int boxHeight = 160;
  int boxX = GetScreenWidth() / 2 - boxWidth / 2;
  int boxY = GetScreenHeight() / 2 - boxHeight / 2;

  DrawRectangle(boxX, boxY, boxWidth, boxHeight, DARKGRAY);

  DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, RAYWHITE);

  const char *message = "Do you want to exit?";
  int textWidth = MeasureText(message, 20);
  DrawText(message, boxX + (boxWidth - textWidth) / 2, boxY + 20, 20, RAYWHITE);

  yesButton.SetPosition({(float)(boxX + 40), (float)(boxY + 90)});
  noButton.SetPosition({(float)(boxX + 180), (float)(boxY + 90)});

  yesButton.Update();
  noButton.Update();

  yesButton.Draw();
  noButton.Draw();

  if (yesButton.IsClicked())
  {
    PlaySound(clickSound);
    TraceLog(LOG_INFO, "ESC pressed, closing exit popup.");
    running = false;
  }
  else if (noButton.IsClicked())
  {
    PlaySound(clickSound);
    showExitPopup = false;
  }

  if (IsKeyPressed(KEY_ESCAPE))
  {
    showExitPopup = false;
  }
}
