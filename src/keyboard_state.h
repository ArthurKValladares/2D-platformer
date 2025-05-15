#pragma once

#include <unordered_map>
#include <unordered_set>
#include <iostream>

#include <SDL3/SDL.h>

#include "glm/vec2.hpp"

#include "window.h"
#include "util.h"

struct KeyState {
    void step() {
        ++steps;
    }

    uint32_t steps;
};

struct KeyboardState {
    bool is_down(SDL_Keycode key) const {
        return keys_down.contains(key);
    }
    
    bool was_just_pressed(SDL_Keycode key) const {
        if (!is_down(key)) return false;
        return keys_down.at(key).steps == 1;
    }

    bool was_just_released(SDL_Keycode key) const {
        return just_released.contains(key);
    }

    void reset() {
        just_released.clear();
    }

    void process_sdl_event(const SDL_KeyboardEvent& event) {
        const SDL_Keycode key = event.key;
        if (event.type == SDL_EVENT_KEY_DOWN) {
            KeyState& state = keys_down[key];
            state.step();
        } else if (event.type == SDL_EVENT_KEY_UP) {
            keys_down.erase(key);
            just_released.insert(key);
        }
    }

    std::unordered_map<SDL_Keycode, KeyState> keys_down;
    std::unordered_set<SDL_Keycode> just_released;
};

struct MouseState {
    MouseState()
    {}

    bool is_down(SDL_MouseButtonFlags button) const {
        return keys_down.contains(button);
    }

    void process_button_event(const SDL_MouseButtonEvent& event) {
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            KeyState& state = keys_down[event.button];
            state.step();
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            keys_down.erase(event.button);
            just_released.insert(event.button);
        }
    }

    void process_motion_event(const SDL_MouseMotionEvent& event) {
        if (event.x >= 0.0 && event.y >= 0.0) {
            pos = glm::vec2(event.x, event.y);
        }
    }

    void set_window_size(float width, float height) {
        w_width = width;
        w_height = height;
    }

    glm::vec2 world_space_pos() const {
        const float n_x = pos.x / w_width;
        const float n_y = pos.y / w_height;

        const float w_x = lerp(-1.0f, 1.0f, n_x);
        const float w_y = lerp(-1.0f, 1.0f, n_y);

        return glm::vec2(w_x, w_y);
    }

    float w_width, w_height;

    glm::vec2 pos;

    std::unordered_map<SDL_MouseButtonFlags, KeyState> keys_down;
    std::unordered_set<SDL_MouseButtonFlags> just_released;
};
