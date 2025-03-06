#define VOLK_IMPLEMENTATION
#include "volk.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int argc, char *argv[]) {
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

    volkInitialize();
    
    VmaAllocator allocator = {};
    VmaAllocatorCreateInfo allocator_ci = {};
    vmaCreateAllocator(&allocator_ci, &allocator);
}