#pragma once

#include <glm/vec3.hpp>

#include "rect.h"
#include "collision_grid.h"
#include "assets.h"

enum class EnemyType {
    Basic
};

struct Enemy {
    void update(const CollisionGrid& collision_grid, double frame_dt, double total_elapsed_time);

    Rect2D rect;
    EnemyType ty;
    bool is_alive;
    TextureSource tex;
    glm::vec3 color;
    bool moving_left;
};
