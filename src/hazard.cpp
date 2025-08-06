#include "hazard.h"

Hazard::Hazard(Rect2D rect, TileType tile_ty, bool is_active)
    : rect(rect)
    , texture(tile_type_to_item_texture(tile_ty))
    , color(tile_type_to_color(tile_ty))
    , is_active(is_active)
{
    assert(texture != TextureSource::Count);

    switch (tile_ty) {
        case TileType::Spike: {
            ty = HazardType::Spike;
            break;
        }
        case TileType::SawShooter: {
            ty = HazardType::SawShooter;

            // TODO: This is bad, I probably need separate constructors for each type, but this will do for now
            firing_delay = 3.0;
            last_fired = 0.0;
            shooting_dir = glm::vec2(0.0, 1.0);

            break;
        }
        default: {
            assert(false && "Tile is not a hazard");
            break;
        }
    }
}

Renderable Hazard::draw() const {
    return colored_quad(
        rect,
        texture_id(texture),
        color
    );
}