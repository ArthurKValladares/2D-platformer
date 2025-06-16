#pragma once

#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "keyboard_state.h"
#include "rect.h"

#include "renderer/renderer.h"
#include "renderables/includes.h"

struct OrthographicCamera {
    OrthographicCamera() {}
    OrthographicCamera(glm::vec2 pos, glm::vec2 size, float scale = 1.0, double damping_time = 0.125, glm::vec2 static_area_scale = glm::vec2(0.0))
        : size(size)
        , sqrt_scale(sqrt(scale))
        , center(pos)
        , damping_time(damping_time)
        , move_to(center)
        , static_area_scale(static_area_scale)
    {}

    void mark_move_to(glm::vec2 pos, double total_elapsed_time) {
        // NOTE: We divide by 2 since static area is around the center so we onlt care about half the size
        const glm::vec2 static_area_size = (static_area_scale / glm::vec2(2.0)) * get_size();
        const glm::vec2 distance = glm::abs(pos - center);
        if (distance.x > static_area_size.x || distance.y > static_area_size.y) {
            move_to = pos;
            started_next_movement = total_elapsed_time;
        }        
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
        return (sqrt_scale * sqrt_scale) * size;
    }
    Rect2D get_rect() const {
        return Rect2D(center, get_size());
    }

    glm::mat4 get_proj_matrix() const;

    void add_to_renderable(Renderer* renderer, Renderable* renderable, BufferID global_data_buffer) {
        Rect2D static_area_rect = Rect2D(center, static_area_scale * get_size());

        // TODO: Just a way to draw a quad with no texture and just color + alpha
        renderable->push_child(ColoredQuad(
            renderer,
            static_area_rect,
            TextureSource::Path,
            glm::vec3(1.0, 0.0, 0.0),
            global_data_buffer
        ));
    }

    glm::vec2 size;
    float sqrt_scale;

    glm::vec2 center;

    double damping_time;
    double started_next_movement;
    glm::vec2 move_to;

    glm::vec2 static_area_scale;
};