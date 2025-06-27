#pragma once

#include "shader.h"

#include "../assets.h"
#include "../renderer/draw.h"
#include "../renderer/renderer.h"

#include <vector>

struct TriangleDataFrag final : FragmentShader {
    TriangleDataFrag() {}
    TriangleDataFrag(TextureID texture)
        :  texture_id(texture)
    {}

    ShaderSource source() const {
        return ShaderSource::TriangleDataFrag;
    }

    int32_t push_descriptor_set_idx() const {
        return 1;
    }

    void append_push_descriptor_sets(std::vector<PushDescriptorSetData>& sets) const {
        sets.push_back(PushDescriptorSetData{
            .binding = 0,
            .ty = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .texture_id = texture_id,
        });
    }

    TextureID draw_texture() const {
        return texture_id;
    }

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}

    TextureID texture_id;
};