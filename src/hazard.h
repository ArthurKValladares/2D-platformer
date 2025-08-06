#pragma once

#include "rect.h"
#include "collision_grid.h"
#include "assets.h"

#include "map_editor/tile_types.h"

#include "renderables/includes.h"

enum class HazardType {
    Spike,
    SawShooter,
    Saw
};

struct Hazard {
    Hazard() {}
    Hazard(Rect2D rect, TileType tile_ty, bool is_active);

    Renderable draw() const;

    Rect2D rect;
    HazardType ty;
    TextureSource texture;
    glm::vec3 color;
    bool is_active;

    // TODO: type specific stuff, make into a union later.
    // saw-shooter
    double firing_delay;
    double last_fired;
    glm::vec2 shooting_dir;
    // saw
    glm::vec2 movement_dir;
    float speed;
};