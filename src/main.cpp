#include <SDL3/SDL_main.h>
#include "imgui.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <math.h> 
#include <algorithm>

#include "renderer/renderer.h"

#include "assets.h"
#include "window.h"
#include "util.h"
#include "image.h"
#include "keyboard_state.h"
#include "particle_editor.h"
#include "map_editor.h"
#include "global_descriptor_set.h"
#include "view.h"
#include "game.h"

#include "renderables/includes.h"

#include "map_editor/map.h"

// TODO: engine architecture
// descriptor set number 0 will be used for engine-global resources
// descriptor set number 1 will be used for per-object resources (using push descriptors)

struct App {
    App() 
        : app_start(std::chrono::steady_clock::now())
        , keyboard_state(KeyboardState())
        , mouse_state(MouseState())
        , window(Window())
        , renderer(window)
    {
        mouse_state.set_window_size(window.get_size().width, window.get_size().height);

        open_tab_idx = 0;
        tab_items[0] = std::make_unique<Game>(window, &renderer);
        tab_items[1] = std::make_unique<ParticleEditor>(window, &renderer);
        tab_items[2] = std::make_unique<MapEditor>(window, &renderer);
    }

    void update(double total_elapsed_seconds, double frame_dt) {
        tab_items[open_tab_idx]->update_fn(keyboard_state, mouse_state, total_elapsed_seconds, frame_dt);

        // Setup imgui
        renderer.set_imgui_fn([&]() {
            ImGui::BeginTabBar("##tabs");

            uint32_t idx = 0;
            for (const std::unique_ptr<View>& tab : tab_items) {
                if (ImGui::BeginTabItem(tab->name())) {
                    tab->draw_imgui(logger, total_elapsed_seconds);
                    ImGui::EndTabItem();

                    open_tab_idx = idx;
                }
                ++idx;
            }

            ImGui::EndTabBar();
        });
    }

    void render(double total_elapsed_seconds, double frame_dt) {
        RootRenderable draw_root = tab_items[open_tab_idx]->draw_fn(&renderer, total_elapsed_seconds);
        RootRenderable ui_root;
        if (tab_items[open_tab_idx]->should_draw_ui()) {
            ui_root = tab_items[open_tab_idx]->draw_ui(&renderer, total_elapsed_seconds);
        }

        ViewDrawData draw_data;
        draw_root.append_draw_data(&renderer, draw_data);
        ui_root.append_draw_data(&renderer, draw_data);

        renderer.wait_for_and_reset_curr_fence();
        draw_data.upload_vertex_index_data(&renderer);

        renderer.render(logger, window, draw_data.draws, frame_dt);
    }

    void render_loop() {
        last_frame = std::chrono::steady_clock::now();

        SDL_Event e;
        SDL_zero(e);

        bool quit = false;
        while (!quit) {
            const std::chrono::steady_clock::time_point frame_start = std::chrono::steady_clock::now();
            const std::chrono::duration<double>         elapsed_seconds = frame_start - app_start;
            const std::chrono::duration<double>         frame_dt = frame_start - last_frame;
            last_frame = frame_start;
    
            keyboard_state.reset();
            while(SDL_PollEvent(&e)) {
                renderer.process_sdl_event(&e);
                keyboard_state.process_sdl_event(e.key);
                mouse_state.process_button_event(e.button);
                mouse_state.process_motion_event(e.motion);

                if (e.type == SDL_EVENT_QUIT ) {
                    quit = true;
                } else if (e.type = SDL_EVENT_WINDOW_RESIZED) {
                    renderer.resize_swapchain(window);
                }
            }

            const double elapsed_secounds_count = elapsed_seconds.count();
            const double frame_dt_count = frame_dt.count();
            update(elapsed_secounds_count, frame_dt_count);
            render(elapsed_secounds_count, frame_dt_count);
        }
    }

    void cleanup() {
        for (std::unique_ptr<View>& tab : tab_items) {
            tab->cleanup(&renderer);
        }
    }

    ImguiLog logger;

    std::chrono::steady_clock::time_point app_start;
    KeyboardState keyboard_state;
    MouseState mouse_state;
    Window window;
    Renderer renderer;
    std::chrono::steady_clock::time_point last_frame;

    uint32_t open_tab_idx;
    std::array<std::unique_ptr<View>, 3> tab_items;
};

int main(int argc, char *argv[]) {
    App app = App();
    
    app.render_loop();

    app.cleanup();

    return 0;
}