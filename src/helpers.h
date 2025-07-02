#pragma once

#include "camera.h"
#include "window.h"
#include "keyboard_state.h"
#include "camera.h"

inline glm::vec2 world_space_pos(const Window& window, const MouseState& mouse_state) {
    const Size2Di32 w_size = window.get_size();

    const float n_x = mouse_state.pos.x / w_size.width;
    const float n_y = mouse_state.pos.y / w_size.height;

    const float w_x = lerp(-1.0f, 1.0f, n_x);
    const float w_y = lerp(-1.0f, 1.0f, n_y);

    return glm::vec2(w_x, w_y);
}

inline glm::vec2 get_screen_pos(const Window& window, const MouseState& mouse_state, const OrthographicCamera& camera) {
    glm::vec2 ws_pos = world_space_pos(window, mouse_state);
    ws_pos.y = -ws_pos.y;
    const glm::vec2 half_size = camera.get_size() / glm::vec2(2.0);
    return camera.center + (ws_pos * half_size);
}