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
    // PROCEDURAL BACKGROUND RAIN
    //
    // Decorative only. These particles never enter water physics.
    // ------------------------------------------------------------

    if (world.isRaining())
    {
        const float time = static_cast<float>(GetTime());

        const float rainfall = world.getRainfallMmPerHour();

        const float intensity =
            std::clamp(
                std::log10(1.0f + rainfall) / 4.0f,
                0.0f,
                1.0f
            );

        // Slowly changing wind with occasional stronger gusts.
        const float wind =
            std::sin(time * 0.35f) * 35.0f +
            std::sin(time * 0.91f) * 22.0f +
            std::sin(time * 0.13f) * 65.0f;

        const int width = GetScreenWidth();
        const int height = GetScreenHeight();

        const int count =
            20 + static_cast<int>(intensity * 650.0f);

        for (int i = 0; i < count; ++i)
        {
            const float seed = static_cast<float>(i);

            // Stable pseudo-random values for each particle.
            const float randomA =
                std::abs(std::sin(seed * 127.1f) * 43758.5453f);

            const float randomB =
                std::abs(std::sin(seed * 269.5f) * 23421.631f);

            const float randomC =
                std::abs(std::sin(seed * 73.7f) * 19341.173f);

            const float a = randomA - std::floor(randomA);
            const float b = randomB - std::floor(randomB);
            const float c = randomC - std::floor(randomC);

            // Three apparent depths.
            const int layer = i % 3;

            const float depth =
                layer == 0 ? 0.30f :
                layer == 1 ? 0.60f : 1.0f;

            // Different particles have different fall speeds.
            const float fallSpeed =
                (100.0f + b * 650.0f) *
                (0.55f + intensity * 0.65f);

            const float sidewaysSpeed =
                wind * depth +
                (c - 0.5f) * 45.0f;

            const float x =
                std::fmod(
                    a * width +
                    time * sidewaysSpeed +
                    width * 2.0f,
                    static_cast<float>(width)
                );

            const float y =
                std::fmod(
                    b * height +
                    time * fallSpeed,
                    static_cast<float>(height)
                );

            const float size =
                0.5f + c * (1.0f + depth * 2.0f);

            const float streakLength =
                size * (2.0f + fallSpeed / 100.0f);

            const unsigned char alpha =
                static_cast<unsigned char>(
                    (10.0f + intensity * 65.0f) * depth
                );

            Color colour =
            {
                185,
                215,
                240,
                alpha
            };

            DrawLineEx(
                Vector2{x, y},
                Vector2{
                    x + sidewaysSpeed * 0.025f,
                    y + streakLength
                },
                size * 0.45f,
                colour
            );
        }
    }


    // ------------------------------------------------------------
    // ATMOSPHERIC RAIN LAYERS
    //
    // Decorative rain only. These drops do not affect physics.
    // Drawn in screen space, behind the terrain.
    // ------------------------------------------------------------

    if (world.isRaining())
    {
        const float rainfall =
            world.getRainfallMmPerHour();

        const float intensity =
            std::clamp(
                std::log10(1.0f + rainfall) / 4.0f,
                0.0f,
                1.0f
            );

        const int screenWidth =
            GetScreenWidth();

        const int screenHeight =
            GetScreenHeight();

        const float time =
            static_cast<float>(GetTime());

        const int dropCount =
            30 + static_cast<int>(intensity * 1500.0f);

        for (int i = 0; i < dropCount; ++i)
        {
            const int layer = i % 3;

            const float seed =
                static_cast<float>(i);

            const float randomX =
                std::fmod(
                    std::abs(std::sin(seed * 127.1f) * 43758.5453f),
                    1.0f
                );

            const float randomY =
                std::fmod(
                    std::abs(std::sin(seed * 269.5f) * 23421.631f),
                    1.0f
                );

            const float depth =
                layer == 0 ? 0.30f :
                layer == 1 ? 0.60f : 1.0f;

            const float speed =
                (220.0f + depth * 480.0f) *
                (0.35f + intensity * 0.65f);

            const float x =
                randomX * screenWidth;

            const float y =
                std::fmod(
                    randomY * screenHeight +
                    time * speed +
                    screenHeight,
                    static_cast<float>(screenHeight)
                );

            const float length =
                (3.0f + depth * 9.0f) *
                (0.5f + intensity);

            const unsigned char alpha =
                static_cast<unsigned char>(
                    (12.0f + intensity * 65.0f) *
                    depth
                );

            Color rainColour =
            {
                185,
                215,
                235,
                alpha
            };

            DrawLineEx(
                Vector2{x, y},
                Vector2{
                    x - 1.0f * depth,
                    y + length
                },
                layer == 0 ? 0.7f : 1.0f,
                rainColour
            );
        }
    }


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

    // ------------------------------------------------------------
    // PHYSICAL RAIN DROPS
    //
    // These remain the actual simulated water drops.
    // Their appearance varies without changing their physics.
    // ------------------------------------------------------------

    const World::RainDrop* drops =
        world.getRainDrops();

    int dropCount =
        world.getRainDropCount();

    const float rainTime =
        static_cast<float>(GetTime());

    const float wind =
        std::sin(rainTime * 0.35f) * 35.0f +
        std::sin(rainTime * 0.91f) * 22.0f +
        std::sin(rainTime * 0.13f) * 65.0f;

    for (int i = 0; i < dropCount; ++i)
    {
        const World::RainDrop& drop = drops[i];

        const float seed = static_cast<float>(i);

        const float random =
            std::abs(std::sin(seed * 127.1f) * 43758.5453f);

        const float variation =
            random - std::floor(random);

        const float size =
            0.65f + variation * 1.8f;

        const float length =
            drop.landed
            ? 3.0f + size * 2.0f
            : 6.0f + size * 7.0f;

        const float windDrift =
            wind * (0.015f + variation * 0.035f);

        const unsigned char alpha =
            static_cast<unsigned char>(
                125.0f + variation * 100.0f
            );

        Color colour =
        {
            155,
            215,
            250,
            alpha
        };

        DrawLineEx(
            Vector2{
                drop.x,
                drop.y
            },
            Vector2{
                drop.x + windDrift,
                drop.y + length
            },
            size,
            colour
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
        175;

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

    DrawText(
        TextFormat(
            "Rainfall: %.1f mm/h",
            world.getRainfallMmPerHour()
        ),
        panelX + 10,
        panelY + 154,
        18,
        SKYBLUE
    );
}
