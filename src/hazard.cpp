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