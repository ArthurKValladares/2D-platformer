#pragma once

#include "rect.h"
#include "animatable.h"
#include "keyboard_state.h"
#include "collision_grid.h"

struct Player {
    Player()
    {}
    Player(Rect2D rect, SpriteAnimation sprite)
        : rect(rect)
        , sprite(sprite)
        , moving_right(true)
    {}

    void update(const KeyboardState& keyboard_state, const CollisionGrid& collision_grid, double frame_dt) {
        constexpr float displacement_per_second = 5.0;
        const glm::vec2 displacement_vec = keyboard_state.displacement_vector(displacement_per_second, frame_dt);
        if (glm::length(displacement_vec) != 0.0) {
            std::vector<CollisionGrid::CollisionData> collisions;
            const glm::vec2 non_colliding_disp = collision_grid.get_collisions(rect, displacement_vec, &collisions);
            rect.pos += non_colliding_disp;
            moving_right = non_colliding_disp.x >= 0.0;
        }
    }

    void add_to_renderable(Renderer* renderer, Renderable* renderable, double total_elapsed_time, BufferID global_data_buffer) {
        Rect2D draw_rect = rect;
        if (moving_right) {
            draw_rect.max_uv = glm::vec2(-1.0, 1.0);
        }

        renderable->push_child(MovingQuad(
            renderer,
            draw_rect,
            glm::vec2(0.0, 0.0),
            sprite.texture_at(total_elapsed_time),
            global_data_buffer
        ));
    }

    Rect2D rect;
    SpriteAnimation sprite;
    bool moving_right;
};