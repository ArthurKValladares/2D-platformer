#pragma once

#include "rect.h"
#include "collision_grid.h"
#include "assets.h"

#include "map_editor/tile_types.h"

#include "renderables/includes.h"

enum class HazardType {
    Spike,
    MovingSpike,
    SawShooter,
    Saw
};

struct Hazard {
    Hazard() {}
    Hazard(Rect2D rect, TileType tile_ty, bool is_active, glm::vec2 dir);

    Renderable draw() const;

    Rect2D rect;
    HazardType ty;
    TextureSource texture;
    glm::vec3 color;
    bool is_active;
    glm::vec2 dir;

    // TODO: type specific stuff, make into a union later.
    // saw-shooter
    double firing_delay;
    double last_fired;
    // saw/moving spike
    float speed;
    // moving spike exclusive
    bool is_moving;
};