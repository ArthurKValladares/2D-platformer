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
        , gravity_force(0.005)
        , jump_force_scale(30.0)
        , jump_delay(0.05)
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

    void draw_imgui() {
        if(ImGui::InputFloat("Gravity Force", &gravity_force)) {
            gravity_force = std::max(0.0f, gravity_force);
        }
        if(ImGui::InputFloat("Jump Scale Force", &jump_force_scale, 1.0, 5.0)) {
            jump_force_scale = std::max(0.0f, jump_force_scale);
        }
        if(ImGui::InputFloat("Jump Delay", &jump_delay)) {
            jump_delay = std::max(0.0f, jump_delay);
        }
    }

    Rect2D rect;
    SpriteAnimation sprite;
    glm::vec2 movement_vec;
    bool is_mid_jump;
    double last_jump;

    float gravity_force;
    float jump_force_scale;
    float jump_delay;
};