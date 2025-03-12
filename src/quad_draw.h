#pragma once

#include "rect.h"
#include "asset_manager.h"

struct QuadDraw {
    bool is_empty() const {
        return rect.is_zero_sized();
    }

    Rect2D rect;
    TextureSource texture;
};


