// v12

#include <raylib.h>

#include "src/World.h"
#include "src/Renderer.h"

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 900;

    InitWindow(
        screenWidth,
        screenHeight,
        "grasssim"
    );

    SetTargetFPS(60);

    World world;
    Renderer renderer;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_Q))
            break;

        float deltaTime = GetFrameTime();

        world.update(deltaTime);

        BeginDrawing();

        renderer.draw(world);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
