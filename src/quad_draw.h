#pragma once

#include "rect.h"
#include "assets.h"

struct QuadDraw {
    bool is_empty() const {
        return rect.is_zero_sized();
    }

    Rect2D rect;
    TextureSource texture;
    ShaderSource vertex_shader;
    ShaderSource fragment_shader;
};


