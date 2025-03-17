#pragma once

#include "shared.h"

#include "../assets.h"
#include "../renderer/draw.h"

#include <vector>

struct TriangleFrag {
    TriangleFrag() {}
    TriangleFrag(TextureSource texture)
        :  texture_binding(texture)
    {}

    DescriptorSetData draw_texture_binding() const {
        return DescriptorSetData{
            .set = 0,
            .binding = 0
        };
    }

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}

    TextureSource texture_binding;
};