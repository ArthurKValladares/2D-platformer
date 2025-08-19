#pragma once

#include "rect.h"
#include "collision_grid.h"
#include "assets.h"

#include "map_editor/tile_types.h"

#include "renderables/includes.h"

enum class PickupType {
    DoubleJump,
    DoubleDash,
    End
};

struct Pickup {
    Pickup() {}
    Pickup(Rect2D rect, TileType tile_ty, bool is_active);

    Renderable draw() const;

    Rect2D rect;
    PickupType ty;
    TextureSource texture;
    glm::vec3 color;
    bool is_active;
};
