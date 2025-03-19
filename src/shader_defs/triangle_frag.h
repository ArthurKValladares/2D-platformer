#pragma once

#include "shared.h"
#include "shader.h"

#include "../assets.h"
#include "../renderer/draw.h"

#include <vector>

struct TriangleFrag final : FragmentShader {
    TriangleFrag() {}
    TriangleFrag(TextureSource texture)
        :  texture_binding(texture)
    {}

    ShaderSource source() const {
        return ShaderSource::TriangleFrag;
    }

    DescriptorSetData draw_texture_binding() const {
        return DescriptorSetData{
            .set = 0,
            .binding = 0
        };
    }

    TextureSource draw_texture() const {
        return texture_binding;
    }

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}

    TextureSource texture_binding;
};