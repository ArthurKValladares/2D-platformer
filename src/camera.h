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
    OrthographicCamera(glm::vec2 pos, glm::vec2 size, float scale = 1.0, double damping_time = 0.5, glm::vec2 static_area_scale = glm::vec2(0.0))
        : size(size)
        , sqrt_scale(sqrt(scale))
        , center(pos)
        , damping_time(damping_time)
        , move_to(center)
        , static_area_scale(static_area_scale)
        , draw_debug(false)
    {}

    void mark_move_to(glm::vec2 pos, double total_elapsed_time) {
        // NOTE: We divide by 2 since static area is around the center so we only care about half the size
        const glm::vec2 static_area_size = (static_area_scale / glm::vec2(2.0)) * get_size();
        const glm::vec2 distance = pos - center;

        glm::vec2 camera_movement = glm::vec2(0.0);
        if (abs(distance.x) > static_area_size.x) {
            camera_movement.x = distance.x;
        }
        if (abs(distance.y) > static_area_size.y) {
            camera_movement.y = distance.y;
        }

        move_to = center + camera_movement;
        started_next_movement = total_elapsed_time;
    }

    void update(const KeyboardState& keyboard_state, double frame_dt, double total_elapsed_time) {
        constexpr float camera_zoom_vel = 0.5;
        if (keyboard_state.is_down(SDLK_E)) {
            sqrt_scale += camera_zoom_vel * frame_dt;
        }
        if (keyboard_state.is_down(SDLK_Q)) {
            sqrt_scale -= camera_zoom_vel * frame_dt;
            sqrt_scale = std::max(0.01f, sqrt_scale);
        }

        if (center == move_to) return;

        double elapsed_damping = (damping_time == 0.0)
            ? 1.0
            : (total_elapsed_time - started_next_movement) / damping_time;
        elapsed_damping = std::min(1.0, elapsed_damping);

        const glm::vec2 new_pos = lerp_vec2(center, move_to, elapsed_damping);
        center = new_pos;
    }

    glm::vec2 get_size() const {
        return (sqrt_scale * sqrt_scale) * size;
    }
    Rect2D rect() const {
        return Rect2D(center, get_size());
    }

    glm::mat4 get_proj_matrix() const;

    void add_to_renderable(Renderer* renderer, Renderable* renderable, BufferID global_data_buffer) {
        if (draw_debug) {
            Rect2D static_area_rect = Rect2D(center, static_area_scale * get_size());

            renderable->push_child(flat_color_quad(
                renderer,
                static_area_rect,
                glm::vec4(1.0, 0.0, 0.0, 0.5),
                global_data_buffer
            ));
        }
    }

    void draw_imgui() {
        ImGui::Text("Center: (%.3f, %.3f)", center.x, center.y);
        ImGui::Text("Size: (%.3f, %.3f)", size.x, size.y);
        ImGui::InputFloat("Root cale", &sqrt_scale, 0.005, 0.01);
        ImGui::SliderFloat2("Static Area Scale", (float*) &static_area_scale, 0.0f, 1.0f);
        if (ImGui::InputDouble("Damping Time", &damping_time, 0.05, 0.1) && damping_time < 0.0) {
            damping_time = 0.0;
        }

        ImGui::Checkbox("Draw Debug", &draw_debug);
    }

    glm::vec2 size;
    float sqrt_scale;

    glm::vec2 center;

    double damping_time;
    double started_next_movement;
    glm::vec2 move_to;

    glm::vec2 static_area_scale;

    bool draw_debug;
};