#define VOLK_IMPLEMENTATION
#include "volk.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int argc, char *argv[]) {
    //
    // Init SDL
    //
    constexpr int SCREEN_WIDTH = 1200;
    constexpr int SCREEN_HEIGHT = 800;

    SDL_Window* window = nullptr;
    SDL_Surface* screen_surface = nullptr;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("2D-Platformer", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if(window == NULL) {
        SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
        return -1;
    }
    screen_surface = SDL_GetWindowSurface(window);

    //
    // Main loop
    //
    SDL_Event e;
    SDL_zero(e);

    bool quit = false;
    while (!quit) {
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT ) {
                quit = true;
            } else if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_ESCAPE) {
                    quit = true;
                }
            }
        }
    }

    //
    // Cleanup SDL
    //
    SDL_DestroySurface(screen_surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
}