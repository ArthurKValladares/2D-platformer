#include "player.h"


void Player::jump(const CollisionGrid& collision_grid, double total_elapsed_time) {
    // TODO: Kinda sloppy, duplicated code with the update below, need to separate it out.
    // and handle jumping in Game and not player
    is_mid_jump = true;
    last_jump = total_elapsed_time;

    const glm::vec2 displacement_vec = glm::vec2(0.0, gravity_force * jump_force_scale);

    std::vector<CollisionGrid::CollisionData> collisions;
    const glm::vec2 non_colliding_disp = collision_grid.get_collisions(rect, displacement_vec, &collisions);

    rect.pos += non_colliding_disp;

    movement_vec = non_colliding_disp;
}

void Player::update(const KeyboardState& keyboard_state, const CollisionGrid& collision_grid, double frame_dt, double total_elapsed_time) {
    constexpr float displacement_per_second = 5.0;

    // NOTE: We handle the y axis separately here since jumping is a special case due to gravity
    glm::vec2 displacement_vec = keyboard_state.displacement_vector(displacement_per_second, frame_dt, SDLK_A, SDLK_D, SDLK_UNKNOWN, SDLK_UNKNOWN);
    displacement_vec.y = movement_vec.y;

    if (!is_mid_jump && keyboard_state.was_just_pressed(SDLK_SPACE) && ((total_elapsed_time - last_jump) > jump_delay)) {
        displacement_vec.y = gravity_force * jump_force_scale;
        is_mid_jump = true;
        last_jump = total_elapsed_time;
    }
    if (!already_dashed && keyboard_state.was_just_pressed(SDLK_LSHIFT) && ((total_elapsed_time - last_dash) > dash_delay)) {
        const float dir = displacement_vec.x >= 0.0 
            ? 1.0
            : -1.0;
        displacement_vec.x += dash_force * dir;
        already_dashed = true;
        last_dash = total_elapsed_time;
    }
    displacement_vec.y -= gravity_force;

    std::vector<CollisionGrid::CollisionData> collisions;
    const glm::vec2 non_colliding_disp = collision_grid.get_collisions(rect, displacement_vec, &collisions);

    rect.pos += non_colliding_disp;
    
    // TODO: Might need to add more data to the CollisionData struct to better determine if we've hit the ground or not
    // right now we are just resetting collisions in general
    const auto it = std::find_if(collisions.begin(), collisions.end(), [&](CollisionGrid::CollisionData data) {
        return data.rect.max_y() == rect.min_y();
    });
    if (it != collisions.end()) {
        is_mid_jump = false;
        already_dashed = false;
    }

    movement_vec = non_colliding_disp;
}