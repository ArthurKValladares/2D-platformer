#pragma once

#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "keyboard_state.h"
#include "rect.h"

struct OrthographicCamera {
    OrthographicCamera() {}
    OrthographicCamera(glm::vec2 pos, float size_x, float size_y, float scale = 1.0)
        : size_x(size_x)
        , size_y(size_y)
        , scale(sqrt(scale))
        , center(pos)
    {}

    glm::vec2 get_size() const {
        return glm::vec2(scale * size_x, scale * size_y);
    }
    Rect2D get_rect() const {
        return Rect2D(center, get_size());
    }

    glm::mat4 get_proj_matrix() const;

    float size_x;
    float size_y;
    float scale;
    glm::vec2 center;
};