// v11

#include "Grass.h"

#include <raylib.h>
#include <cmath>

void Grass::draw(float x, float y, float amount)
{
    if (amount <= 0.0f)
        return;

    const int bladeCount =
        1 + static_cast<int>(amount * 9.0f);

    for (int i = 0; i < bladeCount; ++i)
    {
        float seed =
            std::sin(
                x * 12.9898f +
                y * 78.233f +
                i * 37.719f
            );

        seed -= std::floor(seed);

        // Small variation in blade height.
        float height =
            (10.0f + seed * 18.0f) * amount;

        // Slightly different lean for each blade.
        float lean =
            (seed - 0.5f) * 10.0f;

        float spacing =
            48.0f /
            static_cast<float>(bladeCount);

        float bladeX =
            x +
            spacing * (i + 0.5f);

        DrawLineEx(
            Vector2{
                bladeX,
                y + 2.0f
            },
            Vector2{
                bladeX + lean,
                y - height
            },
            2.0f,
            Color{
                45,
                145,
                50,
                255
            }
        );
    }
}