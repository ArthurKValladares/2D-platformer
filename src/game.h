#pragma once

#include "collision_grid.h"
#include "view.h"
#include "camera.h"
#include "animatable.h"
#include "player.h"
#include "ui.h"
#include "button.h"
#include "helpers.h"

#include "map_editor/map.h"

#include "renderer/renderer.h"
#include "renderables/includes.h"

#define PLAYER_SCALE 0.9
#define PLAYER_SIZE TILE_SIZE * PLAYER_SCALE

namespace {
    Rect2D get_tile_rect(uint32_t x_start, uint32_t y_start, uint32_t tile_width = 1, uint32_t tile_height = 1) {
        return Rect2D::from_bottom_left_and_size(
            glm::vec2(x_start * TILE_SIZE, y_start * TILE_SIZE),
            glm::vec2(tile_width * TILE_SIZE, tile_height * TILE_SIZE)
        );
    }
};

struct Game final : View {
    Game(const Window& window, Renderer* renderer)
        : map_idx(0)
        , collision_grid(CollisionGrid(TILE_SIZE * 2.0, TILE_SIZE * 2.0))
        , global_set_data(GlobalDescriptorSetData(renderer, camera))
        , should_show_ui(false)
        , ui(UI(renderer, window))
    {
        // maps
        for (uint32_t i = 0; i < (uint32_t) MapSource::Count; ++i) {
            const MapLayout map = MapLayout(map_path(static_cast<MapSource>(i)));
            maps.push_back(map.optimize());
        }

        // player
        player = Player(
            get_tile_rect(maps[map_idx].start.col, maps[map_idx].start.row), 
            SpriteAnimation(0.75, 0.0, {
                TextureSource::Go1,
                TextureSource::Go2,
                TextureSource::Go3,
                TextureSource::Go4,
                TextureSource::Go6,
                TextureSource::Go7,
                TextureSource::Go8
            })
        );

        // camera
        const Size2Di32 window_size = window.get_size();
        const float scale = static_cast<float>(maps[map_idx].width * TILE_SIZE) / window_size.width;
        camera = OrthographicCamera(
            player.rect.center(),
            glm::vec2(window_size.width, window_size.height),
            scale
        );
        camera.static_area_scale = glm::vec2(0.25, 0.25);

        // button
        const char* p_text = "Button";
        const float text_scale = 1.0;
        TextSize first_line_size = ui.get_text_size(p_text, text_scale);

        const float padding = first_line_size.y * 0.5;
        const float size_x = first_line_size.x + padding;
        const float size_y = first_line_size.y + padding;

        quit_button = Button(
            Rect2D(glm::vec2(0.0), glm::vec2(size_x, size_y)),
            glm::vec4(128.0 / 255.0, 128.0 / 255.0, 128.0 / 255.0, 1.0),
            glm::vec4(199.0 / 255.0, 199.0 / 255.0, 199.0 / 255.0, 1.0),
            p_text,
            text_scale,
            glm::vec4(1.0),
            []() { send_quit_event(); }
        );

        // collision grid
        setup_collision_grid();
        
        // global ds data
        global_set_data.write_shader_data_to_buffer(renderer);
        update_global_set(renderer, global_set_data.buffer_id, global_set_data.set_id);
    }

    void setup_collision_grid() {
        collision_grid.cells.clear();
        for (const MergedTile& tile : maps[map_idx].tiles) {
            const TileType ty = tile.ty;
            if (ty == TileType::Wall) {
                const Rect2D rect = get_tile_rect(tile.x_offset, tile.y_offset, tile.width, tile.height);
                collision_grid.insert_rect(rect);
            }
        }
    }

    const char* name() const {
        return "App";
    }

    void update_fn(const Window& window, const KeyboardState& keyboard_state, const MouseState& mouse_state, double total_elapsed_seconds, double frame_dt) {
        player.update(keyboard_state, collision_grid, frame_dt, total_elapsed_seconds);
        camera.update(keyboard_state, frame_dt, total_elapsed_seconds);
        quit_button.update(mouse_state, get_screen_pos(window, mouse_state, ui.camera));

        if (keyboard_state.was_just_pressed(SDLK_ESCAPE)) {
            should_show_ui = !should_show_ui;
        }

        // Test if game is won
        const Rect2D end_rect = get_tile_rect(maps[map_idx].end.col, maps[map_idx].end.row);
        if (player.rect.intersects(end_rect)) {
            map_idx = (map_idx + 1) % maps.size();
            setup_collision_grid();

            player.rect.pos = glm::vec2(maps[map_idx].start.col * TILE_SIZE, maps[map_idx].start.row * TILE_SIZE);
        }

        camera.mark_move_to(player.rect.center(), total_elapsed_seconds);
    }

    RootRenderable draw_fn(Renderer* renderer, double total_elapsed_time) {
        Renderable renderable;

        global_set_data.shader_data.proj_matrix = camera.get_proj_matrix();
        global_set_data.write_shader_data_to_buffer(renderer);

        const Rect2D camera_rect = camera.rect();

        const OptimizedMap& opt_map = maps[map_idx];
        for (uint32_t t = 0; t < opt_map.tiles.size(); ++t) {
            const MergedTile tile = opt_map.tiles[t];

            Rect2D rect = get_tile_rect(tile.x_offset, tile.y_offset, tile.width, tile.height);
            rect.max_uv = glm::vec2(tile.width, tile.height);

            if (rect.intersects(camera_rect)) {
                const TileType ty = tile.ty;

                renderable.push_child(colored_quad(
                    rect,
                    texture_id(tile_type_to_texture(ty)),
                    tile_type_to_color(ty)
                ));

                const TextureSource item_tex = tile_type_to_item_texture(ty);
                if (item_tex != TextureSource::Count) {
                    renderable.push_child(colored_quad(
                        rect,
                        texture_id(item_tex),
                        tile_type_to_color(ty)
                    ));
                }
            }
        }

        player.add_to_renderable(&renderable, total_elapsed_time);
        camera.add_to_renderable(&renderable);

        return RootRenderable {
            renderable,
            global_set_data.layout_id,
            global_set_data.set_id
        };
    }

    bool should_draw_ui() const {
        return should_show_ui;
    }

    RootRenderable draw_ui(Renderer* renderer, double total_elapsed_time) {
        Renderable renderable;

        renderable.push_child(quit_button.draw(renderer, ui));

        // TODO: can just pass the GlobalDescriptorSetData struct instead, maybe even just a raw ptr
        return RootRenderable {
            renderable,
            ui.global_descriptor_set.layout_id,
            ui.global_descriptor_set.set_id
        };
    }

    void draw_imgui(ImguiLog& logger, double total_elapsed_time) {
        if (ImGui::TreeNode("Player")) {
            player.draw_imgui();
            
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Camera")) {
            camera.draw_imgui();
            
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("UI")) {
            ui.draw_imgui();
            
            ImGui::TreePop();
        }
    }

    void cleanup(Renderer* renderer) {}

    uint64_t map_idx;
    std::vector<OptimizedMap> maps;

    CollisionGrid collision_grid;

    Player player;

    OrthographicCamera camera;

    GlobalDescriptorSetData global_set_data;

    bool should_show_ui;
    UI ui;
    Button quit_button;
};