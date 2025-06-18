#include "player.h"

void Player::update(const KeyboardState& keyboard_state, const CollisionGrid& collision_grid, double frame_dt) {
    constexpr float displacement_per_second = 5.0;
    const glm::vec2 displacement_vec = keyboard_state.displacement_vector(displacement_per_second, frame_dt);
    if (glm::length(displacement_vec) != 0.0) {
        std::vector<CollisionGrid::CollisionData> collisions;
        const glm::vec2 non_colliding_disp = collision_grid.get_collisions(rect, displacement_vec, &collisions);
        rect.pos += non_colliding_disp;
        moving_right = non_colliding_disp.x >= 0.0;
    }
}