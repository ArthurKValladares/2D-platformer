#include "hazard.h"

#include <glm/gtx/vector_angle.hpp>

Hazard::Hazard(Rect2D rect, TileType tile_ty, bool is_active, glm::vec2 dir)
    : rect(rect)
    , texture(tile_type_to_item_texture(tile_ty))
    , color(tile_type_to_color(tile_ty))
    , is_active(is_active)
    , dir(dir)
{
    assert(texture != TextureSource::Count);

    switch (tile_ty) {
        case TileType::Spike: {
            ty = HazardType::Spike;
            break;
        }
        case TileType::MovingSpike: {
            ty = HazardType::MovingSpike;

            speed = 2.0;
            is_moving = false;

            break;
        }
        case TileType::SawShooter: {
            ty = HazardType::SawShooter;

            // TODO: This is bad, I probably need separate constructors for each type, but this will do for now.
            // Also, shooting dir needs to not be hard-coded, get it automatically from wall the shooter is attached to,
            // just like i will do for the moving spike
            firing_delay = 3.0;
            last_fired = 0.0;

            break;
        }
        default: {
            assert(false && "Tile is not a hazard");
            break;
        }
    }
}

Renderable Hazard::draw() const {
    const glm::vec2 default_hazard_angle = glm::vec2(0.0, 1.0);
    const float rotation = glm::angle(dir, default_hazard_angle);

    const glm::vec2 translation = rect.pos;
    return moving_quad(
        Rect2D(glm::vec2(0.0), rect.size()),
        translation,
        rotation,
        texture_id(texture)
        //color
    );
}