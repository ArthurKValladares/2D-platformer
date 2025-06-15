#pragma once

#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "keyboard_state.h"
#include "rect.h"

struct OrthographicCamera {
    OrthographicCamera() {}
    OrthographicCamera(glm::vec2 pos, float size_x, float size_y, float scale = 1.0, double damping_time = 0.125)
        : size_x(size_x)
        , size_y(size_y)
        , scale(sqrt(scale))
        , center(pos)
        , damping_time(damping_time)
        , move_to(center)
    {}

    void mark_move_to(glm::vec2 pos, double total_elapsed_time) {
        move_to = pos;
        started_next_movement = total_elapsed_time; 
    }

    void update(double total_elapsed_time) {
        if (center == move_to) return;

        const double elapsed_damping = (total_elapsed_time - started_next_movement) / damping_time;
        if (elapsed_damping < 1.0) {
            const glm::vec2 new_pos = lerp_vec2(center, move_to, elapsed_damping);
            center = new_pos;
        }
    }

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

    double damping_time;
    double started_next_movement;
    glm::vec2 move_to;
};