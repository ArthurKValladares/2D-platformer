#include "game.h"
#include "particle_loader.h"

namespace {
    struct SawShooterSettings {
        float firing_delay;
        float speed;
    };
    SawShooterSettings saw_shooter_settings {
        .firing_delay = 3.0,
        .speed = 2.0
    };

    struct MovingSpikeSettings {
        float speed;
    };
    MovingSpikeSettings moving_spike_settings {
        .speed = 2.0
    };
};

Game::Game(const Window& window, Renderer* renderer)
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


    // Load particles
    load_save.load(nullptr, PARTICLES_DIR, PARTICLES_EXTENSION, "test_2", [&](nlohmann::json& root) {
        load_particle_fn(jump_particle, root, 0.0);
    });
}

void Game::update_fn(const UpdateContext& context, double total_elapsed_seconds, double frame_dt) {
    if (context.keyboard_state.was_just_pressed(SDLK_R)) {
        reset();
    }
    if (context.keyboard_state.was_just_pressed(SDLK_ESCAPE)) {
        should_show_ui = !should_show_ui;
    }

    const glm::vec2 player_bottom_pos = player.rect.bottom_center(); 
    player.update(context.keyboard_state, collision_grid, frame_dt, total_elapsed_seconds);
    // TODO: Also sloppy, need a big cleanup pass, doing too much in player update function
    if (player.is_mid_jump && player.last_jump == total_elapsed_seconds) {
        ParticleEmitter new_jump_particle = jump_particle;
        new_jump_particle.start_time = total_elapsed_seconds;
        new_jump_particle.center = player_bottom_pos;
        particle_emitters.push_back(std::move(new_jump_particle));
    }
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
                    player.is_mid_jump = false;
                    player.last_jump = -player.jump_delay;

                    pickup.is_active = false;

                    break;
                }
                case PickupType::DoubleDash: {
                    player.already_dashed = false;
                    player.last_dash = -player.dash_delay;

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
                        // NOTE: Empty means there is nothing in between the hazard and the player, i.e. the hazard can "see" the player
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

                    Hazard new_hazard = Hazard();
                    new_hazard.rect = hazard.rect;
                    new_hazard.ty = HazardType::Saw;
                    new_hazard.texture = TextureSource::Saw;
                    new_hazard.color = glm::vec3(1.0);
                    new_hazard.is_active = true;
                    new_hazard.is_child = true;
                    new_hazard.dir = hazard.dir;
                    new_hazard.speed = saw_shooter_settings.speed;

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

                        ParticleEmitter new_jump_particle = jump_particle;
                        new_jump_particle.start_time = total_elapsed_seconds;
                        new_jump_particle.center = player_bottom_pos;
                        particle_emitters.push_back(std::move(new_jump_particle));
                        
                        player.jump(collision_grid, total_elapsed_seconds);
                    } else {
                        reset();
                    }
                    break;
                }
                case EnemyType::Spiky: {
                    reset();
                    break;
                }
            }
        }
    }

    for (ParticleEmitter& emitter : particle_emitters) {
        emitter.update(total_elapsed_seconds);
    }
    particle_emitters.erase(
        std::remove_if(
            particle_emitters.begin(), 
            particle_emitters.end(),
            [&](const ParticleEmitter& emitter) { 
                return !emitter.is_alive_at(total_elapsed_seconds) && emitter.all_particles_dead();
            }
        ),
        particle_emitters.end()
    );

    camera.mark_move_to(player.rect.center(), total_elapsed_seconds);
}

void Game::setup_hazards() {
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
                new_hazard.speed = moving_spike_settings.speed;
                new_hazard.is_moving = false;
                break;
            }
            case TileType::SawShooter: {
                new_hazard.ty = HazardType::SawShooter;
                new_hazard.firing_delay = saw_shooter_settings.firing_delay;
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

void Game::draw_imgui(ImguiLog& logger, double total_elapsed_time) {
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
    if (ImGui::TreeNode("Saw Shooter")) {
        ImGui::DragFloat("firing delay", &saw_shooter_settings.firing_delay);
        ImGui::DragFloat("speed", &saw_shooter_settings.speed);

        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Moving Spike")) {
        ImGui::DragFloat("speed", &moving_spike_settings.speed);

        ImGui::TreePop();
    }
}