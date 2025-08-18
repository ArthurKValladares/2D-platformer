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
        const TextSize first_line_size = ui.get_text_size(p_text, text_scale);
        const float padding = first_line_size.y * 0.5;
        const float size_x = first_line_size.x + padding;
        const float size_y = first_line_size.y + padding;
        const glm::vec2 button_size = glm::vec2(size_x, size_y);
        const float button_distance = size_y + padding * 2;

        useless_button_0 = Button(
            Rect2D(glm::vec2(0.0) - glm::vec2(0.0, button_distance), button_size),
            glm::vec4(128.0 / 255.0, 128.0 / 255.0, 128.0 / 255.0, 1.0),
            glm::vec4(199.0 / 255.0, 199.0 / 255.0, 199.0 / 255.0, 1.0),
            p_text,
            text_scale,
            glm::vec4(1.0),
            []() {}
        );
        quit_button = Button(
            Rect2D(glm::vec2(0.0), button_size),
            glm::vec4(128.0 / 255.0, 128.0 / 255.0, 128.0 / 255.0, 1.0),
            glm::vec4(199.0 / 255.0, 199.0 / 255.0, 199.0 / 255.0, 1.0),
            "Quit",
            text_scale,
            glm::vec4(1.0),
            []() { send_quit_event(); }
        );
        useless_button_1 = Button(
            Rect2D(glm::vec2(0.0) + glm::vec2(0.0, button_distance), button_size),
            glm::vec4(128.0 / 255.0, 128.0 / 255.0, 128.0 / 255.0, 1.0),
            glm::vec4(199.0 / 255.0, 199.0 / 255.0, 199.0 / 255.0, 1.0),
            p_text,
            text_scale,
            glm::vec4(1.0),
            []() {}
        );

        setup_collision_grid();
        setup_enemies();
        setup_pickups();
        setup_hazards();

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

    void update_fn(const UpdateContext& context, double total_elapsed_seconds, double frame_dt) {
        if (context.keyboard_state.was_just_pressed(SDLK_R)) {
            reset();
        }
        if (context.keyboard_state.was_just_pressed(SDLK_ESCAPE)) {
            should_show_ui = !should_show_ui;
        }

        player.update(context.keyboard_state, collision_grid, frame_dt, total_elapsed_seconds);
        camera.update(context.keyboard_state, frame_dt, total_elapsed_seconds);
        useless_button_0.update(context.mouse_state, get_screen_pos(context.window, context.mouse_state, ui.camera));
        quit_button.update(context.mouse_state, get_screen_pos(context.window, context.mouse_state, ui.camera));
        useless_button_1.update(context.mouse_state, get_screen_pos(context.window, context.mouse_state, ui.camera));

        // TODO: Handle duplication
        for (Pickup& pickup : pickups) {
            if (!pickup.is_active) continue;

            if (player.rect.intersects(pickup.rect)) {
                switch (pickup.ty) {
                    case PickupType::End: {
                        map_idx = (map_idx + 1) % maps.size();
                        reset();

                        break;
                    }
                    case PickupType::DoubleJump: {
                        player.last_jump = -player.jump_delay;
                        player.is_mid_jump = false;

                        pickup.is_active = false;

                        break;
                    }
                }
            }
        }
        for (Hazard& hazard : hazards) {
            if (!hazard.is_active) continue;

            switch (hazard.ty) {
                case HazardType::Spike: {
                    if (player.rect.intersects(hazard.rect)) {
                        reset();
                    }

                    break;
                }
                case HazardType::MovingSpike: {
                    std::vector<CollisionGrid::CollisionData> collisions;
                    collision_grid.get_collisions(hazard.rect, glm::vec2(0.0), &collisions);
                    if (!collisions.empty()) {
                        hazard.is_active = false;
                    }

                    if (!hazard.is_moving) {
                        const float distance = glm::distance(player.rect.pos, hazard.rect.pos);

                        const bool should_check_in_x_plane =
                            (hazard.dir.x == 1.0  && (player.rect.pos.x > hazard.rect.pos.x) && player.rect.intersects_y_plane(hazard.rect)) ||
                            (hazard.dir.x == -1.0 && (player.rect.pos.x < hazard.rect.pos.x) && player.rect.intersects_y_plane(hazard.rect));

                        const bool should_check_in_y_plane = 
                            (hazard.dir.y == 1.0  && (player.rect.pos.y > hazard.rect.pos.y) && player.rect.intersects_x_plane(hazard.rect)) ||
                            (hazard.dir.y == -1.0 && (player.rect.pos.y < hazard.rect.pos.y) && player.rect.intersects_x_plane(hazard.rect));

                        if (should_check_in_x_plane || should_check_in_y_plane) {
                            glm::vec2 ray = should_check_in_x_plane
                                ? glm::vec2(hazard.dir.x * distance, 0.0)
                                : glm::vec2(0.0, hazard.dir.y * distance);
                            collision_grid.get_collisions(hazard.rect, ray, &collisions);
                            // TODO: Empty means there is nothing in between the hazard and the player, i.e. the hazard can "see" the player
                            if (collisions.empty()) {
                                hazard.is_moving = true;
                            }
                        }
                    } else {
                        const glm::vec2 disp_vec = hazard.dir * glm::vec2(hazard.speed * frame_dt);
                        hazard.rect.pos += disp_vec;
                    }

                    if (player.rect.intersects(hazard.rect)) {
                        reset();
                    }

                    break;
                }
                case HazardType::SawShooter: {
                    if ((total_elapsed_seconds - hazard.last_fired) > hazard.firing_delay) {
                        hazard.last_fired = total_elapsed_seconds;

                        // TODO: a bunch of hard-coded stuff
                        Hazard new_hazard = Hazard();
                        new_hazard.rect = hazard.rect;
                        new_hazard.ty = HazardType::Saw;
                        new_hazard.texture = TextureSource::Saw;
                        new_hazard.color = glm::vec3(1.0);
                        new_hazard.is_active = true;
                        new_hazard.is_child = true;
                        new_hazard.dir = hazard.dir;
                        new_hazard.speed = 2.0;

                        hazards.push_back(new_hazard);
                    }

                    break;
                }
                case HazardType::Saw: {
                    std::vector<CollisionGrid::CollisionData> collisions;
                    collision_grid.get_collisions(hazard.rect, glm::vec2(0.0), &collisions);
                    if (!collisions.empty()) {
                        hazard.is_active = false;
                    }

                    const glm::vec2 disp_vec = hazard.dir * glm::vec2(hazard.speed * frame_dt);
                    hazard.rect.pos += disp_vec;

                    if (player.rect.intersects(hazard.rect)) {
                        reset();
                    }

                    break;
                }
            }
        }
        hazards.erase(
            std::remove_if(
                hazards.begin(), 
                hazards.end(),
                [](const Hazard& hazard) { 
                    return hazard.is_child && !hazard.is_active;
                }
            ),
            hazards.end()
        );
        for (Enemy& enemy : enemies) {
            if (!enemy.is_alive) continue;

            enemy.update(collision_grid, frame_dt, total_elapsed_seconds);

            const glm::vec2 intersection = player.rect.intersection_vector(enemy.rect); 
            if (intersection != glm::vec2(0.0)) {
                switch (enemy.ty) {
                    case EnemyType::Basic: {
                        if (intersection.y < 0.0) {
                            enemy.is_alive = false;
                            player.jump(collision_grid, total_elapsed_seconds);
                        } else {
                            reset();
                        }
                        break;
                    }
                }
            }
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

        renderable.push_child(useless_button_0.draw(renderer, ui));
        renderable.push_child(quit_button.draw(renderer, ui));
        renderable.push_child(useless_button_1.draw(renderer, ui));

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
    void setup_hazards() {
        hazards.clear();

        for (const TileHazard& hazard : maps[map_idx].hazards) {
            Rect2D rect = get_tile_rect(hazard.pos.col, hazard.pos.row, 1, 1);

            Hazard new_hazard = Hazard();
            new_hazard.rect = rect;
            new_hazard.texture = tile_type_to_item_texture(hazard.ty);
            new_hazard.color = tile_type_to_item_color(hazard.ty);
            new_hazard.is_active = hazard.is_active;
            new_hazard.is_child = false;
            new_hazard.dir = hazard.dir;
            switch (hazard.ty) {
                case TileType::Spike: {
                    new_hazard.ty = HazardType::Spike;
                    break;
                }
                case TileType::MovingSpike: {
                    new_hazard.ty = HazardType::MovingSpike;
                    new_hazard.speed = 2.0;
                    new_hazard.is_moving = false;
                    break;
                }
                case TileType::SawShooter: {
                    new_hazard.ty = HazardType::SawShooter;
                    new_hazard.firing_delay = 3.0;
                    new_hazard.last_fired = 0.0;
                    break;
                }
                default: {
                    assert(false && "Tile is not a hazard");
                    break;
                }
            }

            hazards.push_back(new_hazard);
        }
    }

    void reset() {
        player.rect.pos = get_tile_rect(maps[map_idx].start.col, maps[map_idx].start.row).pos;
        player.movement_vec = glm::vec2(0.0);

        setup_collision_grid();
        setup_enemies();
        setup_pickups();
        setup_hazards();

        // TODO: Not really a full reset, need to separate the map definition from the game definition
        for (TileHazard& hazard : maps[map_idx].hazards) {
            hazard.is_active = true;
        }
    }

    void cleanup(Renderer* renderer) {}

    uint64_t map_idx;
    std::vector<OptimizedMap> maps;
    // Actual Game data, separate from the map definition
    std::vector<Enemy> enemies;
    std::vector<Pickup> pickups;
    std::vector<Hazard> hazards;

    CollisionGrid collision_grid;

    Player player;

    OrthographicCamera camera;

    GlobalDescriptorSetData global_set_data;

    bool should_show_ui;
    UI ui;

    // TODO: UI "page" abstraction
    Button useless_button_0;
    Button quit_button;
    Button useless_button_1;
};