#include "player.h"


void Player::update(const KeyboardState& keyboard_state, const CollisionGrid& collision_grid, double frame_dt, double total_elapsed_time) {
    constexpr float displacement_per_second = 5.0;

    // NOTE: We handle the y axis separately here since jumping is a special case due to gravity
    glm::vec2 displacement_vec = keyboard_state.displacement_vector(displacement_per_second, frame_dt, SDLK_A, SDLK_D, SDLK_UNKNOWN, SDLK_UNKNOWN);
    displacement_vec.y = movement_vec.y;

    if (!is_mid_jump && keyboard_state.was_just_pressed(SDLK_SPACE) && ((total_elapsed_time - last_jump) > jump_delay)) {
        displacement_vec.y = gravity_force * jump_force_scale;
        is_mid_jump = true;
    }
    displacement_vec.y -= gravity_force;

    std::vector<CollisionGrid::CollisionData> collisions;
    const glm::vec2 non_colliding_disp = collision_grid.get_collisions(rect, displacement_vec, &collisions);

    rect.pos += non_colliding_disp;
    
    // TODO: Might need to add more data to the CollisionData struct to better determine if we've hit the ground or not
    const auto it = std::find_if(collisions.begin(), collisions.end(), [&](CollisionGrid::CollisionData data) {
        return data.rect.max_y() == rect.min_y();
    });
    if (it != collisions.end() && is_mid_jump) {
        is_mid_jump = false;
        last_jump = total_elapsed_time;
    }

    movement_vec = non_colliding_disp;
}