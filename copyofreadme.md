# GrassSim

A small C++ simulation experiment inspired by the vertical world
presentation and environmental simulation of games such as Oxygen Not Included.

The project is being built from the ground up with C++, CMake and raylib.

## Current Status

# grasssim

A small C++/Raylib environmental simulation focused initially on grass, soil, moisture, rainfall, and eventually water movement.

## Current Version

**v17 — Surface water pooling prototype**

### What we've achieved

* Procedurally generated rolling terrain with variable surface height.
* Dirt and clay underground layers.
* Blue sky above the surface.
* Surface grass with variable fertility and growth.
* Soil moisture simulation.
* Rainfall with adjustable intensity.
* Rain can be toggled on and off.
* Rain intensity can be increased and decreased.
* Rain drops visibly fall onto the terrain surface.
* Rain water is absorbed into the surface soil.
* Excess rainfall creates surface water.
* Surface water can spread between neighbouring tiles.
* Camera can be moved by middle-mouse dragging.
* Mouse wheel zoom allows close inspection of individual grass tiles.
* Hovering over a tile displays its type, coordinates, fertility, moisture, grass amount, and surface water.
* Surface grass colour varies according to fertility.

## v17 Water Pooling

The first surface-water system is now working.

Rain no longer disappears when the soil becomes saturated. Instead, excess water remains on the surface and can form visible pools.

The current system allows water to:

* accumulate on the surface
* slowly soak into soil
* spread sideways
* move toward lower terrain

### Current limitation

The water is still effectively attached to the tile it occupies. It does not yet behave like a true volume of water with a calculated water level.

In particular, adding more water to a tile does not currently make the water surface rise correctly, and gravity does not yet produce realistic downhill flow and pooling.

## Next Step — Proper Water Physics

The next version will replace the simple surface-water behaviour with a volume-based system.

The goal is for water to behave approximately like:

```text
          rain
           ↓
        ~~~~~~~
        grass
        ██████
           ↓
        ███████
       ~~~~~~~~~
       ████████
      ~~~~~~~~~~~
      ███████████
```

Water should:

* have a volume rather than simply a fixed visual height
* build vertically when more water accumulates
* calculate its surface level from the amount of water present
* flow downhill according to the water-surface height
* spread across equal-height terrain
* spill over terrain when a pool becomes deep enough
* accumulate naturally in valleys
* continue infiltrating into the soil
* conserve water rather than simply deleting it

The intention is to establish convincing basic water physics before adding more complex environmental systems.

## Longer-Term Plans

Once the basic water system is behaving correctly:

1. Improve grass growth response to moisture and fertility.
2. Add evaporation.
3. Add proper downhill runoff and streams.
4. Improve grass visual variation.
5. Add small exposed rocks and boulders.
6. Add trees and plant growth.
7. Introduce nutrients and soil depletion.
8. Eventually expand the simulation into a larger environmental system.

The project is intentionally being built incrementally so each simulation system can be observed and tested before the next one is added.


### v16 — Surface Water Pooling

**What changed**

- Added surface water to individual tiles.
- Saturated soil now rejects additional rain.
- Excess rain becomes visible surface water.
- Surface water slowly soaks back into the soil.
- Surface water can spread sideways.
- Water preferentially flows toward lower neighbouring terrain.
- Water is conserved rather than simply disappearing.

**How it works**

Rain is absorbed by the surface soil until its moisture reaches saturation.
Additional rainfall becomes surface water. The water then looks for a lower
neighbouring surface and gradually flows there. Surface water can also
slowly be absorbed back into the soil.

**Result**

Rain can now accumulate on the ground instead of immediately disappearing
into the soil. Low areas of the terrain should begin forming visible
puddles during heavier rainfall.

**Not yet implemented**

- Large bodies of water
- Water flowing across flat terrain
- Water spilling over edges
- Underground water
- Evaporation
- Proper fluid depth simulation

**Next**

Improve pooling so water can accumulate naturally in hollows and spread
across equally low terrain rather than only moving directly toward a
lower neighbouring tile.



### v10 — Working Baseline

The current prototype has:

- Vertical slice / side-on world view
- Procedurally generated rolling terrain
- Variable surface height
- Sky/air above the terrain
- Dirt and clay layers
- Thin green surface turf
- Grass that grows over time
- Fertility affecting grass growth
- Fertility variation across the terrain
- Mouse-over tile information panel
- Tile coordinates displayed
- Material, fertility and grass information displayed
- Zooming with the mouse wheel
- Camera panning with middle-mouse drag
- Zooming up to 4x for examining grass growth
- Version number displayed in the application

The current world deliberately remains simple. There are no rocks,
water, buildings, creatures or complex environmental systems yet.

## Controls

| Input | Action |
|---|---|
| Mouse wheel | Zoom |
| Middle mouse drag | Move camera |
| Mouse hover | Inspect tile |

## Technical Structure

The project is intentionally split into small systems:

- `World` — generates and updates the simulation
- `Tile` — stores the state of individual world cells
- `Grass` — handles grass growth and rendering
- `Renderer` — draws the world and user interface
- `main.cpp` — application entry point

## Goals

The eventual aim is to build a deeper environmental simulation where
plants respond to the conditions around them rather than simply being
decorative graphics.

## Planned Development

### Near Term

1. Improve terrain generation
2. Improve soil visual variation
3. Improve grass growth and appearance
4. Add soil moisture
5. Add more meaningful fertility simulation
6. Add sunlight / exposure
7. Add water
8. Allow environmental properties to change over time

### Medium Term

- Multiple plant types
- Plant competition
- Root systems
- Soil nutrient consumption
- Water movement through soil
- Evaporation
- Weather
- Temperature
- Different soil materials
- Small natural rock patches
- Plant death and decomposition

### Long Term

The goal is to move toward a general-purpose vertical environmental
simulation where terrain, soil, water, plants and organisms interact
through simple underlying rules.

The emphasis is on simulation depth rather than graphical complexity.

## Build

CMake is used to generate the Visual Studio project.

Configure:

```powershell
cmake -S . -B build