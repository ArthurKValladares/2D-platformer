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

    void append_descriptor_sets(std::vector<DescriptorSetData>& sets) const {
        sets.push_back(DescriptorSetData{
            .set = 0,
            .binding = 1,
            .ty = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .texture_id = texture_id(texture_binding),
        });
    }

    TextureSource draw_texture() const {
        return texture_binding;
    }

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}

    TextureSource texture_binding;
};