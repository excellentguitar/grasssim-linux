# grasssim — v18 Update

## Proper Surface Water Physics

v18 is the first step toward making surface water behave like actual water rather than a fixed-height puddle attached to a tile.

### What Changed

The surface water system has been changed to treat `surfaceWater` as a **volume/depth** rather than simply a visual puddle amount.

Water now has a calculated surface level based on:

```text
terrain height + water depth
```

This allows the simulation to distinguish between shallow water on low ground and deeper water on higher ground.

### Gravity

Water now compares the surface level of neighbouring tiles.

When one tile has a higher water surface than its neighbour, water transfers toward the lower surface.

This allows water to:

* flow downhill
* spread sideways
* accumulate in valleys
* become deeper as more water arrives
* eventually move across previously dry terrain
* conserve water while it moves

### Rain

Rain continues to add water to the surface when the soil becomes saturated.

Rainfall is no longer limited by a total rainfall quota, so stronger rain can continue adding water for as long as rainfall is enabled and the rain-drop buffer has capacity.

### Soil Interaction

Surface water continues to slowly infiltrate into the soil.

The existing soil-moisture system remains separate from surface-water flow:

```text
Rain
 ↓
Surface soil moisture
 ↓
Surface water
 ↓
Gravity-driven flow
 ↓
Pooling
```

### Current Status

v18 is a **first implementation of volume-based water physics**.

The main things being tested are:

* Does water move downhill correctly?
* Does water accumulate in low areas?
* Does adding more rain make pools deeper?
* Does water spread naturally across flat terrain?
* Does water remain conserved while moving?
* Does surface water continue soaking into the soil?

### Next Improvements

Once the basic gravity system is stable, the next water improvements will be:

1. Better visual representation of water depth.
2. More realistic pooling behaviour.
3. Water spilling over terrain edges.
4. Faster flow through steep slopes.
5. Slower flow on shallow slopes.
6. Improved absorption into saturated soil.
7. Runoff and small streams.
8. Evaporation.

The goal is to establish convincing water behaviour before adding more complex environmental systems such as trees, nutrients, and larger plant interactions.
