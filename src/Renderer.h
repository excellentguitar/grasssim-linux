// v16

#pragma once

#include "World.h"

#include <raylib.h>

class Renderer
{
public:
    Renderer();

    void draw(const World& world);

private:
    void DrawInfoPanel(const World& world);

    Camera2D camera{};

    Vector2 cameraTarget{};

    float zoom = 1.0f;
};