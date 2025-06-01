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
#include "particle_editor.h"
#include "map_editor.h"
#include "collision_grid.h"
#include "global_descriptor_set.h"

#include "renderables/includes.h"

#include "map_editor/map.h"

// TODO: engine architecture
// descriptor set number 0 will be used for engine-global resources
// descriptor set number 1 will be used for per-object resources (using push descriptors)

#define PLAYER_SCALE 0.9
#define PLAYER_SIZE TILE_SIZE * PLAYER_SCALE

namespace {
    float get_camera_size(const MapLayout& map) {
        return map.tiles[0].size() * TILE_SIZE;
    }
};

struct TabItem {
    const char* name;
    std::function<void(double)> imgui_fn;
    std::function<void(double, double)> update_fn;
    std::function<ViewDrawData(Renderable*, double)> draw_fn;
};

struct App {
    App() 
        : app_start(std::chrono::steady_clock::now())
        , keyboard_state(KeyboardState())
        , mouse_state(MouseState())
        , window(Window())
        , renderer(window)
        , global_set_data(GlobalDescriptorSetData(&renderer, camera))
        , map_idx(0)
        , maps({MapLayout("assets/maps/test_map.map"), MapLayout("assets/maps/test_map_2.map")})
        , collision_grid(CollisionGrid(TILE_SIZE * 2.0, TILE_SIZE * 2.0))
        , opt_collision_grid(CollisionGrid(TILE_SIZE * 2.0, TILE_SIZE * 2.0))
        , player_rect(Rect2D::from_top_left_and_size(
            glm::vec2(maps[map_idx].start.col * TILE_SIZE, maps[map_idx].start.row * TILE_SIZE),
            glm::vec2(PLAYER_SIZE, PLAYER_SIZE)
        ))
        , player_sprite(3.0, 0.0, {TextureSource::Test1, TextureSource::Test2, TextureSource::Test3, TextureSource::Test4})
        , camera(OrthographicCamera(
            player_rect.center(),
            get_camera_size(maps[map_idx]),
            get_camera_size(maps[map_idx])
        ))
        , particle_editor(&renderer)
        , map_editor(&renderer)
        , draw_optimized_grid(false)
    {
        for (const MapLayout& map : maps) {
            opt_maps.push_back(map.optimize());
        }

        mouse_state.set_window_size(window.get_size().width, window.get_size().height);

        global_set_data.write_shader_data_to_buffer(&renderer);
        update_global_set(&renderer, global_set_data.buffer_id, global_set_data.set_id);

        setup_collision_grid();

        open_tab_idx = 0;
        tab_items[0] = TabItem {
            .name = "App",
            .imgui_fn = [&](double total_elapsed_seconds) {
                if (ImGui::TreeNode("Camera")) {
                    ImGui::Text("Center: (%.3f, %.3f)", camera.center.x, camera.center.y);
                    ImGui::Text("Size X: %.3f", camera.size_x);
                    ImGui::Text("Size X: %.3f", camera.size_y);
                    ImGui::Text("Scale: %.3f", camera.scale);

                    ImGui::TreePop();
                }
                ImGui::Checkbox("Optimized Map", &draw_optimized_grid);
            },
            .update_fn = [&](double total_elapsed_seconds, double frame_dt) {
                app_update(keyboard_state, total_elapsed_seconds, frame_dt);
            },
            .draw_fn = [&](Renderable* renderable, double total_elapsed_seconds) {
                // TODO: The "Game/App" stuff should be in its own struct like the editors
                global_set_data.shader_data.proj_matrix = camera.get_proj_matrix();
                global_set_data.write_shader_data_to_buffer(&renderer);

                const Rect2D camera_rect = camera.get_rect();

                if (draw_optimized_grid) {
                    const OptimizedMap& opt_map = opt_maps[map_idx];

                    for (uint32_t t = 0; t < opt_map.tiles.size(); ++t) {
                        const MergedTile tile = opt_map.tiles[t];

                        const Rect2D rect = Rect2D::from_top_left_and_size(
                            glm::vec2(tile.x_offset * TILE_SIZE, tile.y_offset * TILE_SIZE),
                            glm::vec2(tile.width * TILE_SIZE, tile.height * TILE_SIZE)
                        );

                        if (rect.intersects(camera_rect)) {
                            const TileType ty = tile.ty;

                            renderable->push_child(ColoredQuad(
                                &renderer,
                                rect,
                                tile_type_to_texture(ty),
                                tile_type_to_color(ty),
                                global_set_data.buffer_id
                            ));
                        }
                    }
                } else {
                    for (uint32_t row = 0; row < maps[map_idx].tiles.size(); ++row) {
                        for (uint32_t col = 0; col < maps[map_idx].tiles[row].size(); ++col) {
                            const Rect2D rect = Rect2D::from_top_left_and_size(
                                glm::vec2(col * TILE_SIZE, row * TILE_SIZE),
                                glm::vec2(TILE_SIZE, TILE_SIZE)
                            );

                            if (rect.intersects(camera_rect)) {
                                const TileType ty = maps[map_idx].tiles[row][col];

                                renderable->push_child(ColoredQuad(
                                    &renderer,
                                    rect,
                                    tile_type_to_texture(ty),
                                    tile_type_to_color(ty),
                                    global_set_data.buffer_id
                                ));
                            }
                        }
                    }
                }

                renderable->push_child(MovingQuad(
                    &renderer,
                    player_rect,
                    glm::vec2(0.0, 0.0),
                    player_sprite.texture_at(total_elapsed_seconds),
                    global_set_data.buffer_id
                ));

                return renderable->get_draw_data(&renderer, global_set_data.layout_id, global_set_data.set_id);
            }
        };
        tab_items[1] = TabItem{
            .name = "Particle Editor",
            .imgui_fn = [&](double total_elapsed_seconds) {
                particle_editor.imgui_node(&renderer, total_elapsed_seconds);
            },
            .update_fn = [&](double total_elapsed_seconds, double frame_dt) {
                particle_editor.update(keyboard_state, total_elapsed_seconds, frame_dt);
            },
            .draw_fn = [&](Renderable* renderable, double total_elapsed_seconds) {
                particle_editor.add_to_renderable(&renderer, renderable, total_elapsed_seconds);

                return renderable->get_draw_data(&renderer, particle_editor.global_set_data.layout_id, particle_editor.global_set_data.set_id);
            }
        };
        tab_items[2] = TabItem{
            .name = "Map Editor",
            .imgui_fn = [&](double total_elapsed_seconds) {
                map_editor.imgui_node(&renderer);
            },
            .update_fn = [&](double total_elapsed_seconds, double frame_dt) {
                map_editor.update(keyboard_state, mouse_state, total_elapsed_seconds, frame_dt);
            },
            .draw_fn = [&](Renderable* renderable, double total_elapsed_seconds) {
                map_editor.add_to_renderable(&renderer, renderable);

                return renderable->get_draw_data(&renderer, map_editor.global_set_data.layout_id, map_editor.global_set_data.set_id);
            }
        };
    }

    void setup_collision_grid() {
        collision_grid.cells.clear();

        for (uint32_t row = 0; row < maps[map_idx].tiles.size(); ++row) {
            for (uint32_t col = 0; col < maps[map_idx].tiles[row].size(); ++col) {
                const TileType ty = maps[map_idx].tiles[row][col];
                const Rect2D rect = Rect2D::from_top_left_and_size(
                    glm::vec2(col * TILE_SIZE, row * TILE_SIZE),
                    glm::vec2(TILE_SIZE, TILE_SIZE)
                );
                if (ty != TileType::Path && ty != TileType::Start) {
                    collision_grid.insert_rect(rect, ty);
                }
            }
        }

        opt_collision_grid.cells.clear();
        for (const MergedTile& tile : opt_maps[map_idx].tiles) {
            const TileType ty = tile.ty;
            const Rect2D rect = Rect2D::from_top_left_and_size(
                glm::vec2(tile.x_offset * TILE_SIZE, tile.y_offset * TILE_SIZE),
                glm::vec2(tile.width * TILE_SIZE, tile.height * TILE_SIZE)
            );
            if (ty != TileType::Path && ty != TileType::Start) {
                opt_collision_grid.insert_rect(rect, ty);
            }
        }
    }

    void app_update(const KeyboardState& keyboard_state, double total_elapsed_seconds, double frame_dt) {
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

            glm::vec2 non_colliding_disp;
            std::vector<CollisionGrid::CollisionData> collisions;
            if (draw_optimized_grid) {
                non_colliding_disp = collision_grid.get_collisions(player_rect, displacement_vec, &collisions);
            } else {
                non_colliding_disp = opt_collision_grid.get_collisions(player_rect, displacement_vec, &collisions);
            }
            player_rect.pos += non_colliding_disp;

            for (const CollisionGrid::CollisionData& item : collisions) {
                if (item.ty == TileType::End) {
                    map_idx = (map_idx + 1) % maps.size();
                    setup_collision_grid();

                    player_rect.pos = glm::vec2(maps[map_idx].start.col * TILE_SIZE, maps[map_idx].start.row * TILE_SIZE);
                    break;
                }
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
    }

    void update(const KeyboardState& keyboard_state, double total_elapsed_seconds, double frame_dt) {
        tab_items[open_tab_idx].update_fn(total_elapsed_seconds, frame_dt);

        // Setup imgui
        renderer.set_imgui_fn([&]() {
            ImGui::BeginTabBar("##tabs");

            uint32_t idx = 0;
            for (TabItem& tab : tab_items) {
                if (ImGui::BeginTabItem(tab.name)) {
                    tab.imgui_fn(total_elapsed_seconds);
                    ImGui::EndTabItem();

                    open_tab_idx = idx;
                }
                ++idx;
            }

            ImGui::EndTabBar();
        });
    }

    void render(double total_elapsed_seconds, double frame_dt) {
        Renderable renderable;

        ViewDrawData data = tab_items[open_tab_idx].draw_fn(&renderable, total_elapsed_seconds);

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
                mouse_state.process_button_event(e.button);
                mouse_state.process_motion_event(e.motion);

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

    void cleanup() {
        map_editor.cleanup(&renderer);
    }

    std::chrono::steady_clock::time_point app_start;
    KeyboardState keyboard_state;
    MouseState mouse_state;
    Window window;
    Renderer renderer;
    GlobalDescriptorSetData global_set_data;
    std::chrono::steady_clock::time_point last_frame;

    uint64_t map_idx;
    std::vector<MapLayout> maps;
    std::vector<OptimizedMap> opt_maps;

    CollisionGrid collision_grid;
    CollisionGrid opt_collision_grid;

    Rect2D player_rect;
    SpriteAnimation player_sprite;

    OrthographicCamera camera;

    uint32_t open_tab_idx;
    std::array<TabItem, 3> tab_items;
    ParticleEditor particle_editor;
    MapEditor map_editor;

    // TODO: Better way to do debug stuff like this
    bool draw_optimized_grid;
};

int main(int argc, char *argv[]) {
    App app = App();
    
    app.render_loop();

    app.cleanup();

    return 0;
}