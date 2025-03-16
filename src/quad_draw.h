#pragma once

#include "rect.h"
#include "shader_defs/shader_pairs.h"

struct QuadDraw {
    bool is_empty() const {
        return rect.is_zero_sized();
    }

    Rect2D rect;
    ShaderPair shaders;
};


