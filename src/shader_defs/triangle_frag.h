#pragma once

#include "../assets.h"
#include "../renderer/draw.h"

#include <vector>

struct TriangleFrag {
    TriangleFrag() {}
    TriangleFrag(TextureSource texture)
        :  texture_binding(texture)
    {}

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}

    TextureSource texture_binding;
};