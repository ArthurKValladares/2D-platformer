#pragma once

#pragma once

#include "shader.h"

#include "../assets.h"
#include "../renderer/draw.h"
#include "../renderer/renderer.h"

#include <vector>

#include <glm/vec3.hpp>

struct OutlinePushConstantData {
    glm::vec3 color;
    float pad;
    glm::vec3 outline;
    float thickness;
};

struct OutlineFrag final : FragmentShader {
    OutlineFrag() {}
    OutlineFrag(TextureSource texture, OutlinePushConstantData pc_data = OutlinePushConstantData())
        :  texture_binding(texture)
        , pc_data(pc_data)
    {}

    ShaderSource source() const {
        return ShaderSource::OutlineFrag;
    }
    
    int32_t push_descriptor_set_idx() const {
        return 1;
    }

    void append_push_descriptor_sets(std::vector<PushDescriptorSetData>& sets) const {
        sets.push_back(PushDescriptorSetData{
            .binding = 0,
            .ty = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .texture_id = texture_id(texture_binding),
        });
    }

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {
        pcs.push_back(PushConstantData {
            .stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .offset = 0,
            .size = sizeof(OutlinePushConstantData),
            .p_data = &pc_data
        });
    }

    TextureSource texture_binding;
    OutlinePushConstantData pc_data;
};