#pragma once

#include <glm/vec3.hpp>

#include "rect.h"

enum class EnemyType {
    Basic
};

struct Enemy {
    Rect2D rect;
    EnemyType ty;
    bool is_alive;
    TextureSource tex;
    glm::vec3 color;
};