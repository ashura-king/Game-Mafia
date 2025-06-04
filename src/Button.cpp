#include "Button.h"

// Constructor with manual position (normal texture only)
Button::Button(const char *file, Vector2 pos, float scale)
{
  normalTexture = LoadTexture(file);
  position = pos;
  this->scale = scale;
  isPressed = false;
  hasHoverTexture = false;
  hasClickTexture = false;
}

// Constructor with manual position and hover texture
Button::Button(const char *normalFile, const char *hoverFile, Vector2 pos, float scale)
{
  normalTexture = LoadTexture(normalFile);
  hoverTexture = LoadTexture(hoverFile);
  position = pos;
  this->scale = scale;
  isPressed = false;
  hasHoverTexture = true;
  hasClickTexture = false;
}

// Constructor with manual position and all textures
Button::Button(const char *normalFile, const char *hoverFile, const char *clickFile, Vector2 pos, float scale)
{
  normalTexture = LoadTexture(normalFile);
  hoverTexture = LoadTexture(hoverFile);
  clickTexture = LoadTexture(clickFile);
  position = pos;
  this->scale = scale;
  isPressed = false;
  hasHoverTexture = true;
  hasClickTexture = true;
}

// Constructor for centered button (normal texture only)
Button::Button(const char *file, float scale, bool centered, float yOffset)
{
  normalTexture = LoadTexture(file);
  this->scale = scale;
  isPressed = false;
  hasHoverTexture = false;
  hasClickTexture = false;

  if (centered)
  {
    // Calculate centered position, moved down a bit
    position.x = (GetScreenWidth() - (normalTexture.width * scale)) / 2.0f;
    position.y = (GetScreenHeight() - (normalTexture.height * scale)) / 2.0f + yOffset;
  }
  else
  {
    position = {0, 0}; // Default position if not centered
  }
}

// Constructor for centered button with hover texture
Button::Button(const char *normalFile, const char *hoverFile, float scale, bool centered, float yOffset)
{
  normalTexture = LoadTexture(normalFile);
  hoverTexture = LoadTexture(hoverFile);
  this->scale = scale;
  isPressed = false;
  hasHoverTexture = true;
  hasClickTexture = false;

  if (centered)
  {
    position.x = (GetScreenWidth() - (normalTexture.width * scale)) / 2.0f;
    position.y = (GetScreenHeight() - (normalTexture.height * scale)) / 2.0f + yOffset;
  }
  else
  {
    position = {0, 0};
  }
}

// Constructor for centered button with all textures
Button::Button(const char *normalFile, const char *hoverFile, const char *clickFile, float scale, bool centered, float yOffset)
{
  normalTexture = LoadTexture(normalFile);
  hoverTexture = LoadTexture(hoverFile);
  clickTexture = LoadTexture(clickFile);
  this->scale = scale;
  isPressed = false;
  hasHoverTexture = true;
  hasClickTexture = true;

  if (centered)
  {
    position.x = (GetScreenWidth() - (normalTexture.width * scale)) / 2.0f;
    position.y = (GetScreenHeight() - (normalTexture.height * scale)) / 2.0f + yOffset;
  }
  else
  {
    position = {0, 0};
  }
}

Button::~Button()
{
  UnloadTexture(normalTexture);
  if (hasHoverTexture)
  {
    UnloadTexture(hoverTexture);
  }
  if (hasClickTexture)
  {
    UnloadTexture(clickTexture);
  }
}

void Button::Draw()
{
  Texture2D textureToUse = normalTexture; // Default to normal texture

  // Priority: Click state > Hover state > Normal state
  if (isPressed && hasClickTexture)
  {
    textureToUse = clickTexture;
  }
  else if (IsHovered() && hasHoverTexture)
  {
    textureToUse = hoverTexture;
  }

  DrawTextureEx(textureToUse, position, 0.0f, scale, WHITE);
}

void Button::Update()
{
  // Reset pressed state if mouse button is released
  if (isPressed && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
  {
    isPressed = false;
  }
}

bool Button::IsClicked()
{
  Vector2 mousePos = GetMousePosition();
  Rectangle buttonRect = {
      position.x,
      position.y,
      normalTexture.width * scale,
      normalTexture.height * scale};

  // Check if mouse is over button and was clicked
  if (CheckCollisionPointRec(mousePos, buttonRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
  {
    isPressed = true;
    return true;
  }
  return false;
}

bool Button::IsHovered()
{
  Vector2 mousePos = GetMousePosition();
  Rectangle buttonRect = {
      position.x,
      position.y,
      normalTexture.width * scale,
      normalTexture.height * scale};

  return CheckCollisionPointRec(mousePos, buttonRect);
}

// Static helper method to calculate centered position
Vector2 Button::GetCenteredPosition(const char *file, float scale)
{
  Texture2D tempTexture = LoadTexture(file);
  Vector2 centerPos = {
      (GetScreenWidth() - (tempTexture.width * scale)) / 2.0f,
      (GetScreenHeight() - (tempTexture.height * scale)) / 2.0f};
  UnloadTexture(tempTexture);
  return centerPos;
}