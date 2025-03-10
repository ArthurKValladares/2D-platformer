#pragma once

#include <SDL3/SDL.h>

#include "size.h"

struct Window {
    Window();
    ~Window();

    Size2Di32 get_size() const;

    SDL_Window* raw = nullptr;
    SDL_Surface* surface = nullptr;
};