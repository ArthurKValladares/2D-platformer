#pragma once

#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "keyboard_state.h"

struct CameraUpdateData {
    double frame_dt;
    const KeyboardState& keyboard_state;
};

struct OrthographicCamera {
    OrthographicCamera(glm::vec2 pos, float size_x, float size_y)
        : size_x(size_x)
        , size_y(size_y)
        , center(pos)
    {}

    glm::mat4 get_proj_matrix() const;

    void update(const CameraUpdateData& data);

    float size_x;
    float size_y;
    glm::vec2 center;
};