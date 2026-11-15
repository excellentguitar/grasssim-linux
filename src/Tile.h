// v18

#pragma once

enum class TileType
{
    Air,
    Dirt,
    Clay
};

struct Tile
{
    TileType type = TileType::Air;

    float fertility = 0.5f;
    float moisture = 0.0f;
    float grassAmount = 0.0f;

    // Surface water volume.
    // Unlike moisture, this is not limited to 0..1.
    float surfaceWater = 0.0f;
};