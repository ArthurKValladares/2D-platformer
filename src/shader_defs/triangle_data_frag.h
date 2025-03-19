#pragma once

#include "shader.h"

#include "../assets.h"
#include "../renderer/draw.h"
#include "../renderer/renderer.h"

#include <vector>

struct TriangleDataFrag final : FragmentShader {
    TriangleDataFrag() {}
    TriangleDataFrag(TextureSource texture)
        :  texture_binding(texture)
    {}

    ShaderSource source() const {
        return ShaderSource::TriangleDataFrag;
    }

    DescriptorSetData draw_texture_binding() const {
        return DescriptorSetData{
            .set = 0,
            .binding = 1
        };
    }

    TextureSource draw_texture() const {
        return texture_binding;
    }

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}

    TextureSource texture_binding;
};