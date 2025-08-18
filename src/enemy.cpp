#include "enemy.h"

Enemy::Enemy(Rect2D rect, TileType tile_ty, bool is_alive)
    : rect(rect)
    , tex(tile_type_to_item_texture(tile_ty))
    , color(tile_type_to_item_color(tile_ty))
    , is_alive(is_alive)
    , moving_left(true)
{
    assert(tex != TextureSource::Count);

    switch (tile_ty) {
        case TileType::BasicEnemy: {
            ty = EnemyType::Basic;
            break;
        }
        default: {
            assert(false && "Tile is not an enemy");
            break;
        }
    }
}

void Enemy::update(const CollisionGrid& collision_grid, double frame_dt, double total_elapsed_time) {
    if (!is_alive) return;
    
    constexpr float displacement_per_second = 3.0;

    const float displacement_val = displacement_per_second * frame_dt;
    glm::vec2 displacement_vec = moving_left 
        ? glm::vec2(-displacement_val, 0.0) 
        : glm::vec2(displacement_val, 0.0);

    std::vector<CollisionGrid::CollisionData> collisions;
    const glm::vec2 non_colliding_disp = collision_grid.get_collisions(rect, displacement_vec, &collisions);

    rect.pos += non_colliding_disp;
    
    const auto it = std::find_if(collisions.begin(), collisions.end(), [&](CollisionGrid::CollisionData data) {
        return (data.rect.min_x() == rect.max_x()) || (data.rect.max_x() == rect.min_x());
    });
    if (it != collisions.end()) {
        moving_left = !moving_left;
    }
}

Renderable Enemy::draw() const {
    return colored_quad(
        rect,
        texture_id(tex),
        color 
    );
}