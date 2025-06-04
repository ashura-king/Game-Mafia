#pragma once
#include <iostream>
#include "raylib.h"

class Button
{
private:
  Texture2D normalTexture;
  Texture2D hoverTexture;
  Texture2D clickTexture;
  Vector2 position;
  float scale;
  bool isPressed;
  bool hasHoverTexture;
  bool hasClickTexture;

public:
  // Constructor with manual position (normal texture only)
  Button(const char *file, Vector2 pos, float scale);

  // Constructor with manual position and hover texture
  Button(const char *normalFile, const char *hoverFile, Vector2 pos, float scale);

  // Constructor with manual position and all textures
  Button(const char *normalFile, const char *hoverFile, const char *clickFile, Vector2 pos, float scale);

  // Constructor for centered button (normal texture only)
  Button(const char *file, float scale, bool centered = true, float yOffset = 0.0f);

  // Constructor for centered button with hover texture
  Button(const char *normalFile, const char *hoverFile, float scale, bool centered = true, float yOffset = 0.0f);

  // Constructor for centered button with all textures
  Button(const char *normalFile, const char *hoverFile, const char *clickFile, float scale, bool centered = true, float yOffset = 0.0f);

  ~Button();
  void Draw();
  void Update();    // Update button state
  bool IsClicked(); // Check if button was clicked
  bool IsHovered(); // Check if mouse is over button

  // Helper method to calculate centered position
  static Vector2 GetCenteredPosition(const char *file, float scale);
};