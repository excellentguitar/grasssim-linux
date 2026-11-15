// v18

#pragma once

#include "Tile.h"

class World
{
public:
    static constexpr int Width = 60;
    static constexpr int Height = 30;

    static constexpr int MaxRainDrops = 2000;

    World();

    void update(float deltaTime);

    const Tile& getTile(int x, int y) const;

    int getSurfaceHeight(int x) const;

    struct RainDrop
    {
        float x = 0.0f;
        float y = 0.0f;
        float targetY = 0.0f;
        float water = 0.08f;
        bool landed = false;
    };

    const RainDrop* getRainDrops() const;
    int getRainDropCount() const;

    bool isRaining() const;
    float getRainIntensity() const;
    float getRainfallMmPerHour() const;

private:
    Tile tiles[Width][Height];

    int surfaceHeight[Width];

    RainDrop rainDrops[MaxRainDrops];
    int rainDropCount = 0;

    bool raining = true;
    float rainIntensity = 1.0f;

    void updateRain(float deltaTime);
    void updateWater(float deltaTime);
    void updateSurfaceWater(float deltaTime);

    void spawnRainDrop();
};