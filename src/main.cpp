#include <SDL3/SDL_main.h>

#include <iostream>
#include <fstream>
#include <filesystem>

#include "renderer/renderer.h"

#include "window.h"
#include "util.h"

int main(int argc, char *argv[]) {
    Window window = Window();
    Renderer renderer(window);
    
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
            } else if (e.type = SDL_EVENT_WINDOW_RESIZED) {
                renderer.resize_swapchain(window);
            }
        }

        renderer.render(window);
    }

    return 0;
}