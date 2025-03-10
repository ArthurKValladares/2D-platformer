#include "window.h"

#include "util.h"

#include <SDL3/SDL_vulkan.h>

constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 800;

Window::Window() {
    chk_sdl(SDL_Init(SDL_INIT_VIDEO));

    const SDL_WindowFlags window_flags = (SDL_WindowFlags) SDL_WINDOW_VULKAN;
    raw = SDL_CreateWindow("2D-Platformer", SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);
    if(raw == NULL) {
        SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
        exit(-1);
    }
    surface = SDL_GetWindowSurface(raw);
}

Window::~Window() {
    SDL_DestroySurface(surface);
    SDL_DestroyWindow(raw);
    SDL_Quit();
}

Size2Di32 Window::get_size() const {
    Size2Di32 size;
    SDL_GetWindowSize(raw, &size.width, &size.height);
    return size;
}