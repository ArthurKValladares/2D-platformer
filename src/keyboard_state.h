#pragma once

#include <unordered_map>
#include <unordered_set>
#include <iostream>

#include <SDL3/SDL.h>

#include "glm/vec2.hpp"

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

    void process_button_event(const SDL_MouseButtonEvent& event) {
        std::cout << "key: " << event.button << std::endl;
    }

    void process_motion_event(const SDL_MouseMotionEvent& event) {
        std::cout << "x: " << event.x << " y: " << event.y << std::endl;
        std::cout << "x: " << event.xrel << " y: " << event.yrel << std::endl;
    }

    glm::vec2 pos;

    std::unordered_map<uint8_t, KeyState> keys_down;
    std::unordered_set<uint8_t> just_released;
};
