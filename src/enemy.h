#pragma once

#include <glm/vec3.hpp>

#include "rect.h"
#include "collision_grid.h"
#include "assets.h"

#include "map_editor/tile_types.h"

#include "renderables/includes.h"

enum class EnemyType {
    Basic
};

struct Enemy {
    Enemy() {}
    Enemy(Rect2D rect, TileType tile_ty, bool is_alive);

    void update(const CollisionGrid& collision_grid, double frame_dt, double total_elapsed_time);

    Renderable draw() const;

    Rect2D rect;
    EnemyType ty;
    bool is_alive;
    TextureSource tex;
    glm::vec3 color;
    bool moving_left;
};
