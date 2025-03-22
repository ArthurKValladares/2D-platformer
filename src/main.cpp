#include <SDL3/SDL_main.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <math.h> 

#include "renderer/renderer.h"

#include "assets.h"
#include "window.h"
#include "util.h"
#include "image.h"
#include "keyboard_state.h"

#include "views/view.h"

int main(int argc, char *argv[]) {
    const std::chrono::steady_clock::time_point app_start = std::chrono::steady_clock::now();

    Window window = Window();

    Renderer renderer(window);
    
    // View-tree
    View root_view = View();
    root_view.push_child(QuadDraw(
        Rect2D(Point2Df32{ -0.5f,  0.5f }, Size2Df32{1.0, 1.0}),
        TextureSource::Test1
    ));
    root_view.push_child(MovingQuadDraw(
        Rect2D(Point2Df32{  0.5f,  0.5f }, Size2Df32{1.0, 1.0}),
        TextureSource::Test2
    )
    );
    root_view.push_child(ColorQuadDraw(
        Rect2D(Point2Df32{ -0.5f, -0.5f }, Size2Df32{1.0, 1.0}),
        TextureSource::Test3
    ));
    root_view.push_child(DataQuadDraw(
        &renderer,
        Rect2D(Point2Df32{  0.5f, -0.5f }, Size2Df32{1.0, 1.0}),
        TextureSource::Test4
    ));
    root_view.push_child(ControllableQuadDraw(
        Rect2D(Point2Df32{ 0.0f,  0.0f }, Size2Df32{0.5, 0.5}),
        0.0,
        {TextureSource::Akv, TextureSource::Test1, TextureSource::Test2, TextureSource::Test3, TextureSource::Test4}
    ));
    ViewDrawData data = root_view.get_draw_data(&renderer);

    KeyboardState keyboard_state;
    std::chrono::steady_clock::time_point last_frame = std::chrono::steady_clock::now();
    SDL_Event e;
    SDL_zero(e);
    bool quit = false;
    while (!quit) {
        const std::chrono::steady_clock::time_point frame_start = std::chrono::steady_clock::now();
        const std::chrono::duration<double>         elapsed_seconds = frame_start - app_start;
        const std::chrono::duration<double>         frame_dt = frame_start - last_frame;
        last_frame = frame_start;

        while(SDL_PollEvent(&e)) {
            renderer.process_sdl_event(&e);
            if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) {
                keyboard_state.on_keyboard_event(e.key);
            } else if (e.type == SDL_EVENT_QUIT ) {
                quit = true;
            } else if (e.type = SDL_EVENT_WINDOW_RESIZED) {
                renderer.resize_swapchain(window);
            }
        }
        if (keyboard_state.is_down(SDLK_ESCAPE)) {
            quit = true;
        }

        root_view.update(ViewUpdateData{
            .renderer = &renderer,
            .total_elapsed_seconds = elapsed_seconds.count(),
            .frame_dt = frame_dt.count(),
            .keyboard_state = keyboard_state,
        });
        
        renderer.setup_imgui_draw(ImguiData{
            .frame_dt = frame_dt.count()
        });

        renderer.render(window, data.draws);
    }

    return 0;
}