// Gamelayer.hpp
#pragma once
#include <raylib.h>

class Gamelayer
{
public:
    Gamelayer(const char *file, float y, float scal, float parallaxFactor = 1.0f);
    ~Gamelayer();

    void UpdateLayer(float cameraDelta);
    void Drawlayer();
    float GetTextureWidth() const { return texture.width; }

private:
    Texture2D texture;
    float yOffset;
    float scale;
    float scrollX;
    float parallaxSpeed;
};
