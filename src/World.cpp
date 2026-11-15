// v18

#include "World.h"

#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace
{
    constexpr int TileSize = 48;
    constexpr int WorldOffsetY = 80;

    // ------------------------------------------------------------
    // Water physics
    //
    // surfaceWater is a volume expressed as tile-height units.
    //
    // 0.25 = shallow water
    // 1.00 = one tile of water depth
    // 2.00 = two tiles of water depth
    //
    // The actual water surface is:
    //
    // terrain elevation + water depth
    //
    // This lets gravity compare water LEVELS rather than simply
    // comparing water amounts.
    // ------------------------------------------------------------

    constexpr float WaterFlowRate = 0.85f;
    constexpr float MinimumFlowDifference = 0.015f;

    // How quickly standing surface water enters the soil.
    constexpr float SurfaceAbsorptionRate = 0.035f;

    // Rain absorption into the surface soil.
    constexpr float RainAbsorptionRate = 0.30f;
}

World::World()
{
    // ------------------------------------------------------------
    // Generate rolling surface terrain.
    // ------------------------------------------------------------

    for (int x = 0; x < Width; ++x)
    {
        float terrain =
            8.0f
            + 2.0f * std::sin(x * 0.20f)
            + 1.0f * std::sin(x * 0.47f)
            + 0.6f * std::sin(x * 0.91f);

        surfaceHeight[x] =
            static_cast<int>(
                std::round(terrain)
            );
    }

    // ------------------------------------------------------------
    // Generate world.
    // ------------------------------------------------------------

    for (int x = 0; x < Width; ++x)
    {
        for (int y = 0; y < Height; ++y)
        {
            Tile& tile =
                tiles[x][y];

            tile.surfaceWater = 0.0f;

            if (y < surfaceHeight[x])
            {
                tile.type = TileType::Air;
                tile.fertility = 0.0f;
                tile.moisture = 0.0f;
                tile.grassAmount = 0.0f;
            }
            else if (y < surfaceHeight[x] + 6)
            {
                tile.type = TileType::Dirt;
                tile.fertility = 0.5f;
                tile.moisture = 0.05f;
                tile.grassAmount = 0.0f;
            }
            else
            {
                tile.type = TileType::Clay;
                tile.fertility = 0.25f;
                tile.moisture = 0.05f;
                tile.grassAmount = 0.0f;
            }
        }
    }

    // ------------------------------------------------------------
    // Vary fertility along the surface.
    // ------------------------------------------------------------

    for (int x = 0; x < Width; ++x)
    {
        float fertility =
            0.5f
            + 0.30f * std::sin(x * 0.18f)
            + 0.15f * std::sin(x * 0.43f)
            + 0.08f * std::sin(x * 0.93f);

        fertility =
            std::clamp(
                fertility,
                0.0f,
                1.0f
            );

        int surface =
            surfaceHeight[x];

        for (int depth = 0; depth < 6; ++depth)
        {
            int y =
                surface + depth;

            if (y >= Height)
                break;

            tiles[x][y].fertility =
                fertility *
                (1.0f - depth * 0.12f);
        }
    }
}

void World::update(float deltaTime)
{
    // ------------------------------------------------------------
    // Rain controls.
    //
    // R = toggle rain
    // 4 = reduce rain
    // 5 = increase rain
    // ------------------------------------------------------------

    int key =
        GetKeyPressed();

    if (key == 'R' || key == 'r')
    {
        raining =
            !raining;
    }
    else if (key == '4')
    {
        rainIntensity = std::max(0.1f, rainIntensity * 0.5f);
    }
    else if (key == '5')
    {
        rainIntensity = std::min(10000.0f, rainIntensity * 2.0f);
    }

    // ------------------------------------------------------------
    // Simulation order.
    //
    // Rain creates water.
    // Soil moisture moves downward.
    // Surface water then flows under gravity.
    // Surface water also slowly infiltrates soil.
    // ------------------------------------------------------------

    updateRain(deltaTime);

    updateWater(deltaTime);

    updateSurfaceWater(deltaTime);

    // ------------------------------------------------------------
    // Grass growth.
    // ------------------------------------------------------------

    for (int x = 0;
         x < Width;
         ++x)
    {
        int surface =
            surfaceHeight[x];

        Tile& tile =
            tiles[x][surface];

        if (tile.type != TileType::Dirt)
            continue;

        float moistureFactor =
            std::clamp(
                tile.moisture * 3.0f,
                0.0f,
                1.0f
            );

        float growth =
            tile.fertility *
            moistureFactor;

        tile.grassAmount +=
            deltaTime *
            0.10f *
            growth;

        tile.grassAmount =
            std::clamp(
                tile.grassAmount,
                0.0f,
                1.0f
            );
    }
}

void World::updateRain(float deltaTime)
{
    // ------------------------------------------------------------
    // Spawn rain.
    //
    // There is no total-rain quota. Rain continues indefinitely
    // while enabled, subject only to the active drop buffer.
    // ------------------------------------------------------------

    if (raining)
    {
        float spawnRate =
            rainIntensity * 0.70f;

        static float spawnAccumulator =
            0.0f;

        spawnAccumulator +=
            deltaTime * spawnRate;

        while (
            spawnAccumulator >= 1.0f &&
            rainDropCount < MaxRainDrops
        )
        {
            spawnRainDrop();

            spawnAccumulator -=
                1.0f;
        }
    }

    // ------------------------------------------------------------
    // Update rain drops.
    // ------------------------------------------------------------

    for (int i = 0;
         i < rainDropCount;)
    {
        RainDrop& drop =
            rainDrops[i];

        // --------------------------------------------------------
        // Falling.
        // --------------------------------------------------------

        if (!drop.landed)
        {
            drop.y +=
                850.0f *
                deltaTime;

            if (drop.y >= drop.targetY)
            {
                drop.y =
                    drop.targetY;

                drop.landed =
                    true;
            }

            ++i;
            continue;
        }

        // --------------------------------------------------------
        // Find surface tile.
        // --------------------------------------------------------

        int tileX =
            static_cast<int>(
                drop.x /
                static_cast<float>(TileSize)
            );

        if (
            tileX < 0 ||
            tileX >= Width
        )
        {
            --rainDropCount;

            rainDrops[i] =
                rainDrops[
                    rainDropCount
                ];

            continue;
        }

        int surface =
            surfaceHeight[tileX];

        Tile& surfaceTile =
            tiles[tileX][surface];

        // --------------------------------------------------------
        // Rain initially tries to enter the surface soil.
        // --------------------------------------------------------

        float amount =
            RainAbsorptionRate *
            deltaTime;

        amount =
            std::min(
                amount,
                drop.water
            );

        float capacity =
            1.0f -
            surfaceTile.moisture;

        amount =
            std::min(
                amount,
                capacity
            );

        surfaceTile.moisture +=
            amount;

        drop.water -=
            amount;

        // --------------------------------------------------------
        // Once the soil is saturated, remaining rain becomes
        // actual surface water volume.
        // --------------------------------------------------------

        if (
            surfaceTile.moisture >= 0.98f &&
            drop.water > 0.0f
        )
        {
            surfaceTile.surfaceWater +=
                drop.water;

            drop.water =
                0.0f;
        }

        // --------------------------------------------------------
        // Small amount of excess rain can seep into the soil below
        // while the surface is becoming saturated.
        // --------------------------------------------------------

        if (
            surfaceTile.moisture >= 0.90f &&
            drop.water > 0.0f
        )
        {
            int belowY =
                surface + 1;

            if (belowY < Height)
            {
                Tile& below =
                    tiles[tileX][belowY];

                if (below.type != TileType::Air)
                {
                    float belowCapacity =
                        1.0f -
                        below.moisture;

                    float seep =
                        0.02f *
                        deltaTime;

                    seep =
                        std::min(
                            seep,
                            belowCapacity
                        );

                    seep =
                        std::min(
                            seep,
                            drop.water
                        );

                    below.moisture +=
                        seep;

                    drop.water -=
                        seep;
                }
            }
        }

        // --------------------------------------------------------
        // Remove empty drop.
        // --------------------------------------------------------

        if (drop.water <= 0.001f)
        {
            --rainDropCount;

            rainDrops[i] =
                rainDrops[
                    rainDropCount
                ];

            continue;
        }

        ++i;
    }
}

void World::updateWater(float deltaTime)
{
    // ------------------------------------------------------------
    // Soil moisture moves downward through the soil.
    //
    // This remains separate from surface-water gravity.
    // ------------------------------------------------------------

    const float transferRate =
        0.020f *
        deltaTime;

    for (int x = 0;
         x < Width;
         ++x)
    {
        for (int y = 0;
             y < Height - 1;
             ++y)
        {
            Tile& current =
                tiles[x][y];

            Tile& below =
                tiles[x][y + 1];

            if (
                current.type == TileType::Air ||
                below.type == TileType::Air
            )
            {
                continue;
            }

            float difference =
                current.moisture -
                below.moisture;

            if (difference <= 0.15f)
                continue;

            float amount =
                std::min(
                    transferRate,
                    difference * 0.15f
                );

            float capacity =
                1.0f -
                below.moisture;

            amount =
                std::min(
                    amount,
                    capacity
                );

            current.moisture -=
                amount;

            below.moisture +=
                amount;
        }
    }
}

void World::updateSurfaceWater(float deltaTime)
{
    // ============================================================
    // PART 1
    // Surface water infiltrates into the soil.
    // ============================================================

    for (int x = 0;
         x < Width;
         ++x)
    {
        int surface =
            surfaceHeight[x];

        Tile& tile =
            tiles[x][surface];

        if (tile.surfaceWater <= 0.0f)
            continue;

        float capacity =
            1.0f -
            tile.moisture;

        if (capacity <= 0.0f)
            continue;

        float amount =
            SurfaceAbsorptionRate *
            deltaTime;

        amount =
            std::min(
                amount,
                tile.surfaceWater
            );

        amount =
            std::min(
                amount,
                capacity
            );

        tile.surfaceWater -=
            amount;

        tile.moisture +=
            amount;
    }

    // ============================================================
    // PART 2
    //
    // Gravity-driven water flow.
    //
    // This is the important change in v18.
    //
    // We calculate the absolute water surface elevation:
    //
    //     elevation = -terrainHeight + waterDepth
    //
    // A tile with a lower terrain surface starts with a lower
    // elevation. As water builds up, its water level rises.
    //
    // Example:
    //
    //   HIGH GROUND
    //
    //       ~~~
    //       ███
    //
    //             ~~~~~~~~~
    //             █████████
    //             LOW GROUND
    //
    // Water therefore flows whenever the water surface on one
    // tile is higher than the water surface on its neighbour.
    // ============================================================

    float transfer[Width] = {};

    for (int x = 0;
         x < Width;
         ++x)
    {
        int surface =
            surfaceHeight[x];

        Tile& current =
            tiles[x][surface];

        if (current.surfaceWater <= 0.0001f)
            continue;

        // --------------------------------------------------------
        // Current water surface elevation.
        //
        // More positive = physically higher.
        // --------------------------------------------------------

        float currentLevel =
            -static_cast<float>(surface)
            + current.surfaceWater;

        // --------------------------------------------------------
        // Examine left and right neighbours.
        // --------------------------------------------------------

        for (int direction : {-1, 1})
        {
            int neighbourX =
                x + direction;

            if (
                neighbourX < 0 ||
                neighbourX >= Width
            )
            {
                continue;
            }

            int neighbourSurface =
                surfaceHeight[neighbourX];

            Tile& destination =
                tiles[
                    neighbourX
                ][
                    neighbourSurface
                ];

            // ----------------------------------------------------
            // Neighbour water surface elevation.
            // ----------------------------------------------------

            float neighbourLevel =
                -static_cast<float>(
                    neighbourSurface
                )
                + destination.surfaceWater;

            float difference =
                currentLevel -
                neighbourLevel;

            // ----------------------------------------------------
            // No meaningful height difference.
            // ----------------------------------------------------

            if (
                difference <=
                MinimumFlowDifference
            )
            {
                continue;
            }

            // ----------------------------------------------------
            // Flow is proportional to the difference in water
            // surface levels.
            //
            // This automatically handles:
            //
            // - downhill flow
            // - flat terrain
            // - deep pools
            // - water spreading sideways
            // - water climbing slightly when the source pool
            //   becomes deep enough
            // ----------------------------------------------------

            float amount =
                difference *
                WaterFlowRate *
                deltaTime;

            // ----------------------------------------------------
            // Never transfer more water than actually exists.
            // ----------------------------------------------------

            amount =
                std::min(
                    amount,
                    current.surfaceWater
                );

            transfer[x] -=
                amount;

            transfer[neighbourX] +=
                amount;
        }
    }

    // ============================================================
    // PART 3
    // Apply all transfers simultaneously.
    //
    // Doing this after calculating all flows prevents the result
    // from depending heavily on which tile happened to be
    // processed first.
    // ============================================================

    for (int x = 0;
         x < Width;
         ++x)
    {
        int surface =
            surfaceHeight[x];

        Tile& tile =
            tiles[x][surface];

        tile.surfaceWater +=
            transfer[x];

        if (tile.surfaceWater < 0.0f)
        {
            tile.surfaceWater =
                0.0f;
        }
    }

    // ============================================================
    // PART 4
    //
    // Prevent tiny floating-point leftovers from creating endless
    // microscopic flows.
    // ============================================================

    for (int x = 0;
         x < Width;
         ++x)
    {
        int surface =
            surfaceHeight[x];

        Tile& tile =
            tiles[x][surface];

        if (tile.surfaceWater < 0.0001f)
        {
            tile.surfaceWater =
                0.0f;
        }
    }
}

void World::spawnRainDrop()
{
    if (rainDropCount >= MaxRainDrops)
        return;

    int x =
        std::rand() %
        Width;

    int surface =
        surfaceHeight[x];

    RainDrop& drop =
        rainDrops[
            rainDropCount++
        ];

    drop.x =
        x * TileSize +
        8.0f +
        static_cast<float>(
            std::rand() %
            32
        );

    drop.y =
        -50.0f;

    // ------------------------------------------------------------
    // The renderer's world starts at WorldOffsetY.
    //
    // Therefore the top of the surface tile is:
    //
    // WorldOffsetY + surface * TileSize
    //
    // Rain lands exactly on the grass surface.
    // ------------------------------------------------------------

    drop.targetY =
        static_cast<float>(
            WorldOffsetY +
            surface * TileSize
        );

    // Use the amount already defined by World.h.
    drop.water =
        0.025f;

    drop.landed =
        false;
}

const Tile& World::getTile(
    int x,
    int y
) const
{
    return tiles[x][y];
}

int World::getSurfaceHeight(
    int x
) const
{
    if (
        x < 0 ||
        x >= Width
    )
    {
        return 0;
    }

    return surfaceHeight[x];
}

const World::RainDrop* World::getRainDrops() const
{
    return rainDrops;
}

int World::getRainDropCount() const
{
    return rainDropCount;
}

bool World::isRaining() const
{
    return raining;
}

float World::getRainIntensity() const
{
    return rainIntensity;
}

float World::getRainfallMmPerHour() const
{
    return rainIntensity;
}
