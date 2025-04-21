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
#include "camera.h"
#include "animatable.h"
#include "particles.h"

#include "renderables/includes.h"

#include "map_editor/map.h"

// TODO: engine architecture
// descriptor set number 0 will be used for engine-global resources
// descriptor set number 1 will be used for per-object resources (using push descriptors)

#define TILE_SIZE 1.0
#define PLAYER_SCALE 0.9
#define PLAYER_SIZE TILE_SIZE * PLAYER_SCALE

namespace {
    float get_camera_size(const MapLayout& map) {
        return map.tiles[0].size() * TILE_SIZE;
    }
};

struct GlobalDescriptorSetData {
    GlobalDescriptorSetData(Renderer& renderer, const OrthographicCamera& camera) 
        : layout_id(renderer.upload_descriptor_set_layout(get_global_set_bindings()))
        , set_id(renderer.upload_descriptor_set(layout_id))
        , shader_data(GlobalShaderData{
            .proj_matrix = camera.get_proj_matrix()
        })
        , buffer_id(renderer.request_buffer(
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_ALLOCATION_CREATE_MAPPED_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU,
            sizeof(GlobalShaderData)
        ))
    {}

    void write_shader_data_to_buffer(Renderer& renderer) {
        Buffer& buffer = renderer.get_buffer(buffer_id);
        buffer.write_to(&shader_data, sizeof(GlobalShaderData));
    }

    DescriptorSetLayoutID layout_id;
    DescriptorSetID set_id;
    GlobalShaderData shader_data;
    BufferID buffer_id;
};

struct App {
    App() 
        : app_start(std::chrono::steady_clock::now())
        , keyboard_state(KeyboardState())
        , window(Window())
        , renderer(window)
        , global_set_data(GlobalDescriptorSetData(renderer, camera))
        , map(MapLayout("assets/maps/test_map.map"))
        , player_rect(Rect2D(glm::vec2(map.start.col * TILE_SIZE, map.start.row * TILE_SIZE), glm::vec2(PLAYER_SIZE, PLAYER_SIZE)))
        , player_sprite(3.0, 0.0, {TextureSource::Test1, TextureSource::Test2, TextureSource::Test3, TextureSource::Test4})
        , camera(OrthographicCamera(
            player_rect.center(),
            get_camera_size(map),
            get_camera_size(map)
        ))
    {
        global_set_data.write_shader_data_to_buffer(renderer);
        update_global_set(&renderer, global_set_data.buffer_id, global_set_data.set_id);
    }

    Renderable build_root_renderable(KeyboardState& keyboard_state, double total_elapsed_seconds, double frame_dt) {
        Renderable renderable;
        const uint64_t max_row = map.tiles.size();
        const uint64_t max_col = map.tiles[0].size();
        for (uint64_t row = 0; row < max_row; ++row) {
            for (uint64_t col = 0; col < max_col; ++col) {
                const Rect2D rect = Rect2D(glm::vec2(col * TILE_SIZE, row * TILE_SIZE), glm::vec2(TILE_SIZE, TILE_SIZE));

                glm::vec3 color;
                if (rect.intersects(player_rect)) {
                    color = glm::vec3(1.0, 0.0, 1.0);
                } else {
                    color = tile_type_to_color(map.tiles[row][col]);
                }

                renderable.push_child(ColoredQuad(
                    &renderer,
                    rect,
                    TextureSource::Test1,
                    color,
                    global_set_data.buffer_id
                ));
            }
        }

        renderable.push_child(MovingQuad(
            &renderer,
            player_rect,
            glm::vec2(0.0, 0.0),
            player_sprite.texture_at(total_elapsed_seconds),
            global_set_data.buffer_id
        ));
       
        return renderable;
    }

    void update(const KeyboardState& keyboard_state, double total_elapse_seconds, double frame_dt) {
        // Update player movement
        constexpr float displacement_per_second = 5.0;
        glm::vec2 movement_vec{0.0, 0.0};
        if (keyboard_state.is_down(SDLK_A)) {
            movement_vec.x -= 1.0;
        }
        if (keyboard_state.is_down(SDLK_W)){
            movement_vec.y += 1.0;
        }
        if (keyboard_state.is_down(SDLK_S)){
            movement_vec.y -= 1.0;
        }
        if (keyboard_state.is_down(SDLK_D)){
            movement_vec.x += 1.0;
        }
        if (glm::length(movement_vec) > 0.0) {
            movement_vec = glm::normalize(movement_vec);

            const float displacement = displacement_per_second * frame_dt;
            const glm::vec2 displacement_vec = movement_vec * displacement;

            Rect2D new_player_rect = player_rect;
            new_player_rect.pos = player_rect.pos + displacement_vec;

            // Test collision to see if movement is allowed
            // TODO: A hierarcy for this
            const uint64_t curr_col = new_player_rect.pos.x / TILE_SIZE;
            const uint64_t curr_row = new_player_rect.pos.y / TILE_SIZE;

            const uint64_t player_span_x = ceil(new_player_rect.half_size.x / TILE_SIZE);
            const uint64_t player_span_y = ceil(new_player_rect.half_size.y / TILE_SIZE);

            bool has_collided = false;
            const uint64_t max_row = map.tiles.size();
            const uint64_t max_col = map.tiles[0].size();
            for (int64_t row = std::max((int64_t) 0, (int64_t) curr_row - (int64_t) player_span_y); row <= std::min(max_row, curr_row + player_span_y); ++row) {
                for (int64_t col = std::max((int64_t) 0, (int64_t) curr_col - (int64_t) player_span_x); col <= std::min(max_col, curr_col + player_span_x); ++col) {
                    if (map.tiles[row][col] == TileType::Wall) {
                        const Rect2D rect = Rect2D(glm::vec2(col * TILE_SIZE, row * TILE_SIZE), glm::vec2(TILE_SIZE, TILE_SIZE));
                        if (rect.intersects(new_player_rect)) {
                            has_collided = true;
                        }
                    }
                }
            }

            // TODO: Also instead of not setting the position at all, need to see how much I can move before collision
            if (!has_collided) {
                player_rect= new_player_rect;
            }
        }

        // Update camera
        constexpr float camera_zoom_vel = 0.5;
        if (keyboard_state.is_down(SDLK_E)) {
            camera.scale += camera_zoom_vel * frame_dt;
        }
        if (keyboard_state.is_down(SDLK_Q)) {
            camera.scale -= camera_zoom_vel * frame_dt;
            camera.scale = std::max(0.1f, camera.scale);
        }

        camera.center = player_rect.center();

        // Setup imgui
        renderer.set_imgui_fn([&camera = this->camera]() {
            if (ImGui::TreeNode("Camera")) {
                ImGui::Text("Center: (%.3f, %.3f)", camera.center.x, camera.center.y);
                ImGui::Text("Size X: %.3f", camera.size_x);
                ImGui::Text("Size X: %.3f", camera.size_y);
                ImGui::Text("Scale: %.3f", camera.scale);

                ImGui::TreePop();
            }
        });
    }

    void render(double total_elapse_seconds, double frame_dt) {
        global_set_data.shader_data.proj_matrix = camera.get_proj_matrix();
        global_set_data.write_shader_data_to_buffer(renderer);

        Renderable curr_renderable = build_root_renderable(keyboard_state, total_elapse_seconds, frame_dt);
        ViewDrawData data = curr_renderable.get_draw_data(&renderer);
        renderer.wait_for_and_reset_curr_fence();
        data.upload_vertex_index_data(&renderer);

        renderer.render(window, data.draws, frame_dt);
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

                if (e.type == SDL_EVENT_QUIT ) {
                    quit = true;
                } else if (e.type = SDL_EVENT_WINDOW_RESIZED) {
                    renderer.resize_swapchain(window);
                }
            }
            if (keyboard_state.is_down(SDLK_ESCAPE)) {
                quit = true;
            }

            const double elapsed_secounds_count = elapsed_seconds.count();
            const double frame_dt_count = frame_dt.count();
            update(keyboard_state, elapsed_secounds_count, frame_dt_count);
            render(elapsed_secounds_count, frame_dt_count);
        }
    }

    std::chrono::steady_clock::time_point app_start;
    KeyboardState keyboard_state;
    Window window;
    Renderer renderer;
    GlobalDescriptorSetData global_set_data;
    std::chrono::steady_clock::time_point last_frame;

    MapLayout map;

    Rect2D player_rect;
    SpriteAnimation player_sprite;

    OrthographicCamera camera;
};

int main(int argc, char *argv[]) {
    App app = App();
    
    app.render_loop();

    return 0;
}

/*
ParticleEmitter(
    0.0,
    glm::vec2(0.0),
    0.00,
    Degrees(90.0),
    2.0,
    1.0,
    glm::vec2(0.3, 0.3),
    glm::vec3(1.0, 0.0, 0.0),
    glm::vec3(0.0, 0.0, 1.0),
    TextureSource::Particle,
    0.0,
    Degrees(15.0),
    0.5,
    0.25,
    glm::vec2(.125),
    glm::vec3(0.05),
    glm::vec3(0.05)
)
*/