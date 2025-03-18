#pragma once

#include "../rect.h"
#include "../shader_defs/shader_pairs.h"

#include "shared.h"

struct QuadDraw {
    QuadDraw(Rect2D rect, TextureSource texture)
        : rect(rect)
    {
        ShaderPair pair;
        pair.with_triangle_vert(TriangleVert());
        pair.with_triangle_frag(TriangleFrag(texture));
        this->shaders = pair;
    }
    bool is_empty() const {
        return rect.is_zero_sized();
    }

    void update(const ViewUpdateData& _data) {}
    
    Rect2D rect;
    ShaderPair shaders;
};


