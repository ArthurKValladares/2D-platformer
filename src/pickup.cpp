#include "pickup.h"

Pickup::Pickup(Rect2D rect, TileType tile_ty, bool is_active)
    : rect(rect)
    , texture(tile_type_to_item_texture(tile_ty))
    , color(tile_type_to_color(tile_ty))
    , is_active(is_active)
{
    assert(texture != TextureSource::Count);

    switch (tile_ty) {
        case TileType::DoubleJump: {
            ty = PickupType::DoubleJump;
            break;
        }
        case TileType::End: {
            ty = PickupType::End;
            break;
        }
        default: {
            assert(false && "Tile is not an pickup");
            break;
        }
    }
}

Renderable Pickup::draw() const {
    return colored_quad(
        rect,
        texture_id(texture),
        color
    );
}