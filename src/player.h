#pragma once

#include "rect.h"
#include "animatable.h"
#include "keyboard_state.h"
#include "collision_grid.h"

#include "renderer/renderer.h"
#include "renderables/includes.h"

struct Player {
    Player()
    {}
    Player(Rect2D rect, SpriteAnimation sprite)
        : rect(rect)
        , sprite(sprite)
        , movement_vec(glm::vec2(0.0))
        , is_mid_jump(false)
        , last_jump(0.0)
    {}

    void update(const KeyboardState& keyboard_state, const CollisionGrid& collision_grid, double frame_dt, double total_elapsed_time);

    void add_to_renderable(Renderer* renderer, Renderable* renderable, double total_elapsed_time, BufferID global_data_buffer) {
        Rect2D draw_rect = rect;
        if (movement_vec.x >= 0.0) {
            draw_rect.max_uv = glm::vec2(-1.0, 1.0);
        }

        renderable->push_child(moving_quad(
            renderer,
            draw_rect,
            glm::vec2(0.0, 0.0),
            sprite.texture_at(total_elapsed_time),
            global_data_buffer
        ));
    }

    Rect2D rect;
    SpriteAnimation sprite;
    glm::vec2 movement_vec;
    bool is_mid_jump;
    double last_jump;
};