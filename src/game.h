#pragma once

#include "collision_grid.h"
#include "view.h"
#include "camera.h"
#include "animatable.h"
#include "player.h"
#include "ui.h"
#include "button.h"
#include "helpers.h"
#include "enemy.h"
#include "pickup.h"
#include "hazard.h"
#include "particles.h"
#include "load_save.h"
#include "pause_menu.h"

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
    Game(const Window& window, Renderer* renderer, UI& ui);

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

    void update_fn(const UpdateContext& context, double total_elapsed_seconds, double frame_dt);

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
            }
        }

        // TODO: Handle duplication
        for (const Pickup& pickup : pickups) {
            if (!pickup.is_active) continue;

            renderable.push_child(pickup.draw());
        }
        for (const Hazard& hazard : hazards) {
            if (!hazard.is_active) continue;

            renderable.push_child(hazard.draw());
        }
        for (const Enemy& enemy : enemies) {
            if (!enemy.is_alive) continue;
            
            renderable.push_child(enemy.draw());
        }

        for (const ParticleEmitter& emitter : particle_emitters) {
            renderable.push_child(emitter.draw(total_elapsed_time));
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

    RootRenderable draw_ui(UI& ui, Renderer* renderer, double total_elapsed_time) {
        return RootRenderable {
            pause_menu.draw(ui, renderer),
            ui.global_descriptor_set.layout_id,
            ui.global_descriptor_set.set_id
        };
    }

    void draw_imgui(ImguiLog& logger, double total_elapsed_time);

    void setup_enemies() {
        enemies.clear();

        for (const TileEnemy& enemy : maps[map_idx].enemies) {
            Rect2D rect = get_tile_rect(enemy.pos.col, enemy.pos.row, 1, 1);

            enemies.push_back(Enemy(rect, enemy.ty, enemy.is_active));
        }

    }
    void setup_pickups() {
        pickups.clear();

        for (const TilePickup& pickup : maps[map_idx].pickups) {
            Rect2D rect = get_tile_rect(pickup.pos.col, pickup.pos.row, 1, 1);

            pickups.push_back(Pickup(rect, pickup.ty, pickup.is_active));
        }
    }
    void setup_hazards();

    void reset() {
        player.rect.pos = get_tile_rect(maps[map_idx].start.col, maps[map_idx].start.row).pos;
        player.movement_vec = glm::vec2(0.0);

        setup_collision_grid();
        setup_enemies();
        setup_pickups();
        setup_hazards();
    }

    void cleanup(Renderer* renderer) {}

    uint64_t map_idx;
    std::vector<OptimizedMap> maps;
    // Actual Game data, separate from the map definition
    std::vector<Enemy> enemies;
    std::vector<Pickup> pickups;
    std::vector<Hazard> hazards;

    LoadSave load_save;
    ParticleEmitter jump_particle;
    ParticleEmitter dash_particle;
    std::vector<ParticleEmitter> particle_emitters;

    CollisionGrid collision_grid;

    Player player;

    OrthographicCamera camera;

    GlobalDescriptorSetData global_set_data;

    bool should_show_ui;
    PauseMenu pause_menu;
};