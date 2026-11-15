// v17

#include "Renderer.h"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr int TileSize = 48;
    constexpr int WorldOffsetY = 80;

    const Color SkyColor =
    {
        45,
        90,
        145,
        255
    };

    const Color DirtColor =
    {
        105,
        60,
        32,
        255
    };

    const Color ClayColor =
    {
        125,
        82,
        48,
        255
    };

    const Color GrassLowColor =
    {
        105,
        155,
        55,
        255
    };

    const Color GrassHighColor =
    {
        35,
        125,
        45,
        255
    };

    const Color WaterColor =
    {
        55,
        145,
        210,
        220
    };

    const Color WaterHighlight =
    {
        145,
        210,
        240,
        230
    };

    const Color RainColor =
    {
        155,
        215,
        250,
        225
    };
}

Renderer::Renderer()
{
    cameraTarget =
    {
        World::Width * TileSize * 0.5f,
        WorldOffsetY +
            World::Height * TileSize * 0.5f
    };

    zoom = 1.35f;
}

void Renderer::draw(const World& world)
{
    ClearBackground(SkyColor);

    // ------------------------------------------------------------
    // Mouse-wheel zoom.
    // ------------------------------------------------------------

    float wheel =
        GetMouseWheelMove();

    if (wheel != 0.0f)
    {
        zoom +=
            wheel * 0.15f;

        zoom =
            std::clamp(
                zoom,
                0.5f,
                5.0f
            );
    }

    // ------------------------------------------------------------
    // Camera.
    // ------------------------------------------------------------

    camera.target =
        cameraTarget;

    camera.offset =
    {
        GetScreenWidth() * 0.5f,
        GetScreenHeight() * 0.5f
    };

    camera.rotation = 0.0f;
    camera.zoom = zoom;

    // ------------------------------------------------------------
    // Middle mouse drag.
    // ------------------------------------------------------------

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
    {
        Vector2 delta =
            GetMouseDelta();

        cameraTarget.x -=
            delta.x / zoom;

        cameraTarget.y -=
            delta.y / zoom;
    }

    // Rebuild camera after movement.
    camera.target =
        cameraTarget;

    BeginMode2D(camera);

    // ------------------------------------------------------------
    // Terrain.
    // ------------------------------------------------------------

    for (int x = 0;
         x < World::Width;
         ++x)
    {
        int surface =
            world.getSurfaceHeight(x);

        for (int y = 0;
             y < World::Height;
             ++y)
        {
            const Tile& tile =
                world.getTile(x, y);

            if (tile.type == TileType::Air)
                continue;

            int screenX =
                x * TileSize;

            int screenY =
                WorldOffsetY +
                y * TileSize;

            Color tileColor =
                DirtColor;

            if (tile.type == TileType::Clay)
            {
                tileColor =
                    ClayColor;
            }

            // ----------------------------------------------------
            // Fertility shading.
            //
            // Higher fertility = darker soil.
            // Lower fertility = lighter soil.
            // ----------------------------------------------------

            float fertility =
                std::clamp(
                    tile.fertility,
                    0.0f,
                    1.0f
                );

            float fertilityDarkening =
                fertility * 32.0f;

            tileColor.r =
                static_cast<unsigned char>(
                    std::max(
                        0.0f,
                        tileColor.r -
                            fertilityDarkening
                    )
                );

            tileColor.g =
                static_cast<unsigned char>(
                    std::max(
                        0.0f,
                        tileColor.g -
                            fertilityDarkening
                    )
                );

            tileColor.b =
                static_cast<unsigned char>(
                    std::max(
                        0.0f,
                        tileColor.b -
                            fertilityDarkening
                    )
                );

            DrawRectangle(
                screenX,
                screenY,
                TileSize,
                TileSize,
                tileColor
            );

            // ----------------------------------------------------
            // Surface green line.
            //
            // This is ALWAYS drawn on the surface, even when
            // grassAmount is zero.
            //
            // Fertility controls the shade.
            // ----------------------------------------------------

            if (y == surface)
            {
                float f =
                    std::clamp(
                        tile.fertility,
                        0.0f,
                        1.0f
                    );

                Color surfaceGreen;

                surfaceGreen.r =
                    static_cast<unsigned char>(
                        115.0f -
                        f * 75.0f
                    );

                surfaceGreen.g =
                    static_cast<unsigned char>(
                        145.0f +
                        f * 30.0f
                    );

                surfaceGreen.b =
                    static_cast<unsigned char>(
                        55.0f -
                        f * 15.0f
                    );

                DrawRectangle(
                    screenX,
                    screenY - 3,
                    TileSize,
                    4,
                    surfaceGreen
                );
            }

            // ----------------------------------------------------
            // Actual grass growth.
            // ----------------------------------------------------

            if (
                y == surface &&
                tile.grassAmount > 0.0f
            )
            {
                float grass =
                    std::clamp(
                        tile.grassAmount,
                        0.0f,
                        1.0f
                    );

                Color grassColor;

                grassColor.r =
                    static_cast<unsigned char>(
                        105.0f -
                        grass * 70.0f
                    );

                grassColor.g =
                    static_cast<unsigned char>(
                        150.0f +
                        grass * 35.0f
                    );

                grassColor.b =
                    static_cast<unsigned char>(
                        55.0f -
                        grass * 20.0f
                    );

                // ------------------------------------------------
                // Grass thickness increases with growth.
                // ------------------------------------------------

                int grassHeight =
                    2 +
                    static_cast<int>(
                        grass * 15.0f
                    );

                DrawRectangle(
                    screenX,
                    screenY -
                        grassHeight,
                    TileSize,
                    grassHeight,
                    grassColor
                );

                // ------------------------------------------------
                // Individual blades.
                // ------------------------------------------------

                int bladeCount =
                    2 +
                    static_cast<int>(
                        grass * 7.0f
                    );

                for (
                    int blade = 0;
                    blade < bladeCount;
                    ++blade
                )
                {
                    int bladeX =
                        screenX +
                        3 +
                        (
                            blade *
                            (
                                (TileSize - 6) /
                                std::max(
                                    1,
                                    bladeCount - 1
                                )
                            )
                        );

                    int bladeHeight =
                        4 +
                        static_cast<int>(
                            grass * 14.0f
                        );

                    int lean =
                        (blade % 2 == 0)
                        ? -2
                        : 2;

                    DrawLine(
                        bladeX,
                        screenY,
                        bladeX + lean,
                        screenY -
                            bladeHeight,
                        grassColor
                    );
                }
            }

            // ----------------------------------------------------
            // Surface water.
            //
            // surfaceWater is now an unrestricted amount.
            // More water = deeper visible pool.
            // ----------------------------------------------------

            if (
                y == surface &&
                tile.surfaceWater > 0.001f
            )
            {
                float water =
                    tile.surfaceWater;

                // Keep visual depth sensible.
                float visualDepth =
                    std::clamp(
                        water,
                        0.0f,
                        3.0f
                    );

                int waterHeight =
                    2 +
                    static_cast<int>(
                        visualDepth * 8.0f
                    );

                // Water covers the grass from the surface upward.
                DrawRectangle(
                    screenX,
                    screenY -
                        waterHeight,
                    TileSize,
                    waterHeight,
                    WaterColor
                );

                // ------------------------------------------------
                // Water surface highlight.
                // ------------------------------------------------

                DrawRectangle(
                    screenX + 2,
                    screenY -
                        waterHeight +
                        1,
                    TileSize - 4,
                    2,
                    WaterHighlight
                );

                // Deeper water gets another subtle horizontal
                // layer, making the depth visible when zoomed in.
                if (water > 1.0f)
                {
                    DrawLine(
                        screenX + 4,
                        screenY -
                            waterHeight / 2,
                        screenX +
                            TileSize - 5,
                        screenY -
                            waterHeight / 2,
                        WaterHighlight
                    );
                }
            }
        }
    }

    // ------------------------------------------------------------
    // Rain drops.
    // ------------------------------------------------------------

    const World::RainDrop* drops =
        world.getRainDrops();

    int dropCount =
        world.getRainDropCount();

    for (
        int i = 0;
        i < dropCount;
        ++i
    )
    {
        const World::RainDrop& drop =
            drops[i];

        float length =
            drop.landed
            ? 5.0f
            : 11.0f;

        DrawLine(
            static_cast<int>(drop.x),
            static_cast<int>(drop.y),
            static_cast<int>(drop.x),
            static_cast<int>(
                drop.y + length
            ),
            RainColor
        );
    }

    EndMode2D();

    // ------------------------------------------------------------
    // Hover information.
    // ------------------------------------------------------------

    DrawInfoPanel(world);
}

void Renderer::DrawInfoPanel(
    const World& world
)
{
    Vector2 mouse =
        GetMousePosition();

    Camera2D infoCamera{};

    infoCamera.target =
        cameraTarget;

    infoCamera.offset =
    {
        GetScreenWidth() * 0.5f,
        GetScreenHeight() * 0.5f
    };

    infoCamera.rotation = 0.0f;
    infoCamera.zoom = zoom;

    Vector2 worldPosition =
        GetScreenToWorld2D(
            mouse,
            infoCamera
        );

    int tileX =
        static_cast<int>(
            std::floor(
                worldPosition.x /
                TileSize
            )
        );

    int tileY =
        static_cast<int>(
            std::floor(
                (
                    worldPosition.y -
                    WorldOffsetY
                ) /
                TileSize
            )
        );

    if (
        tileX < 0 ||
        tileX >= World::Width ||
        tileY < 0 ||
        tileY >= World::Height
    )
    {
        return;
    }

    const Tile& tile =
        world.getTile(
            tileX,
            tileY
        );

    // ------------------------------------------------------------
    // Highlight hovered tile.
    // ------------------------------------------------------------

    BeginMode2D(infoCamera);

    DrawRectangleLines(
        tileX * TileSize,
        WorldOffsetY +
            tileY * TileSize,
        TileSize,
        TileSize,
        WHITE
    );

    EndMode2D();

    // ------------------------------------------------------------
    // Info panel.
    // ------------------------------------------------------------

    const int panelX =
        15;

    const int panelY =
        55;

    const int panelWidth =
        260;

    const int panelHeight =
        145;

    DrawRectangle(
        panelX,
        panelY,
        panelWidth,
        panelHeight,
        Color{
            20,
            25,
            30,
            225
        }
    );

    DrawRectangleLines(
        panelX,
        panelY,
        panelWidth,
        panelHeight,
        Color{
            180,
            180,
            180,
            255
        }
    );

    const char* typeName =
        "Air";

    if (tile.type == TileType::Dirt)
    {
        typeName =
            "Dirt";
    }
    else if (tile.type == TileType::Clay)
    {
        typeName =
            "Clay";
    }

    DrawText(
        TextFormat(
            "Tile: %s",
            typeName
        ),
        panelX + 10,
        panelY + 10,
        18,
        WHITE
    );

    DrawText(
        TextFormat(
            "Coords: %d, %d",
            tileX,
            tileY
        ),
        panelX + 10,
        panelY + 34,
        18,
        WHITE
    );

    DrawText(
        TextFormat(
            "Fertility: %.2f",
            tile.fertility
        ),
        panelX + 10,
        panelY + 58,
        18,
        WHITE
    );

    DrawText(
        TextFormat(
            "Moisture: %.2f",
            tile.moisture
        ),
        panelX + 10,
        panelY + 82,
        18,
        WHITE
    );

    DrawText(
        TextFormat(
            "Grass: %.2f",
            tile.grassAmount
        ),
        panelX + 10,
        panelY + 106,
        18,
        WHITE
    );

    DrawText(
        TextFormat(
            "Surface water: %.2f",
            tile.surfaceWater
        ),
        panelX + 10,
        panelY + 130,
        18,
        WHITE
    );
}
