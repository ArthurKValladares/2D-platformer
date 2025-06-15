#pragma once

#include "collision_grid.h"
#include "view.h"
#include "camera.h"
#include "animatable.h"

#include "map_editor/map.h"

#include "renderer/renderer.h"
#include "renderables/includes.h"

#define PLAYER_SCALE 0.9
#define PLAYER_SIZE TILE_SIZE * PLAYER_SCALE

namespace {
    Rect2D get_tile_rect(uint32_t x_start, uint32_t y_start, uint32_t tile_width = 1, uint32_t tile_height = 1) {
        return Rect2D::from_top_left_and_size(
            glm::vec2(x_start * TILE_SIZE, y_start * TILE_SIZE),
            glm::vec2(tile_width * TILE_SIZE, tile_height * TILE_SIZE)
        );
    }
};

struct Game final : View {
    Game(const Window& window, Renderer* renderer)
        : map_idx(0)
        , collision_grid(CollisionGrid(TILE_SIZE * 2.0, TILE_SIZE * 2.0))
        , player_sprite(0.75, 0.0, {
            TextureSource::Go1,
            TextureSource::Go2,
            TextureSource::Go3,
            TextureSource::Go4,
            TextureSource::Go6,
            TextureSource::Go7,
            TextureSource::Go8
        })
        , player_moving_right(true)
        , global_set_data(GlobalDescriptorSetData(renderer, camera))
    {
        for (uint32_t i = 0; i < (uint32_t) MapSource::Count; ++i) {
            const MapLayout map = MapLayout(map_path(static_cast<MapSource>(i)));
            maps.push_back(map.optimize());
        }

        player_rect = get_tile_rect(maps[map_idx].start.col, maps[map_idx].start.row);
        const Size2Di32 window_size = window.get_size();
        const float scale = static_cast<float>(maps[map_idx].width * TILE_SIZE) / window_size.width;
        camera = OrthographicCamera(
            player_rect.center(),
            window_size.width,
            window_size.height,
            scale
        );

        setup_collision_grid();

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

    void update_fn(const KeyboardState& keyboard_state, const MouseState& _mouse_state, double total_elapsed_seconds, double frame_dt) {
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

            std::vector<CollisionGrid::CollisionData> collisions;
            const glm::vec2 non_colliding_disp = collision_grid.get_collisions(player_rect, displacement_vec, &collisions);
            player_rect.pos += non_colliding_disp;
            player_moving_right = non_colliding_disp.x >= 0.0;
            
            const Rect2D end_rect = get_tile_rect(maps[map_idx].end.col, maps[map_idx].end.row);
            if (player_rect.intersects(end_rect)) {
                map_idx = (map_idx + 1) % maps.size();
                setup_collision_grid();

                player_rect.pos = glm::vec2(maps[map_idx].start.col * TILE_SIZE, maps[map_idx].start.row * TILE_SIZE);
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

    ViewDrawData draw_fn(Renderer* renderer, Renderable* renderable, double total_elapsed_time) {
        global_set_data.shader_data.proj_matrix = camera.get_proj_matrix();
        global_set_data.write_shader_data_to_buffer(renderer);

        const Rect2D camera_rect = camera.get_rect();

        const OptimizedMap& opt_map = maps[map_idx];
        for (uint32_t t = 0; t < opt_map.tiles.size(); ++t) {
            const MergedTile tile = opt_map.tiles[t];

            Rect2D rect = get_tile_rect(tile.x_offset, tile.y_offset, tile.width, tile.height);
            rect.max_uv = glm::vec2(tile.width, tile.height);

            if (rect.intersects(camera_rect)) {
                const TileType ty = tile.ty;

                renderable->push_child(ColoredQuad(
                    renderer,
                    rect,
                    tile_type_to_texture(ty),
                    tile_type_to_color(ty),
                    global_set_data.buffer_id
                ));

                const TextureSource item_tex = tile_type_to_item_texture(ty);
                if (item_tex != TextureSource::Count) {
                    renderable->push_child(ColoredQuad(
                        renderer,
                        rect,
                        item_tex,
                        tile_type_to_color(ty),
                        global_set_data.buffer_id
                    ));
                }
            }
        }

        Rect2D player_draw_rect = player_rect;
        if (player_moving_right) {
            player_draw_rect.max_uv = glm::vec2(-1.0, 1.0);
        }
        renderable->push_child(MovingQuad(
            renderer,
            player_draw_rect,
            glm::vec2(0.0, 0.0),
            player_sprite.texture_at(total_elapsed_time),
            global_set_data.buffer_id
        ));

        return renderable->get_draw_data(renderer, global_set_data.layout_id, global_set_data.set_id);
    }

    void draw_imgui(ImguiLog& logger, double total_elapsed_time) {
        if (ImGui::TreeNode("Camera")) {
            ImGui::Text("Center: (%.3f, %.3f)", camera.center.x, camera.center.y);
            ImGui::Text("Size X: %.3f", camera.size_x);
            ImGui::Text("Size X: %.3f", camera.size_y);
            ImGui::Text("Scale: %.3f", camera.scale);

            ImGui::TreePop();
        }
    }

    void cleanup(Renderer* renderer) {}

    uint64_t map_idx;
    std::vector<OptimizedMap> maps;

    CollisionGrid collision_grid;

    Rect2D player_rect;
    SpriteAnimation player_sprite;
    bool player_moving_right;

    OrthographicCamera camera;

    GlobalDescriptorSetData global_set_data;
};