#pragma once

#pragma once

#include "shader.h"

#include "../assets.h"
#include "../renderer/draw.h"
#include "../renderer/renderer.h"

#include <vector>

#include <glm/vec4.hpp>


struct FlatColorFrag final : FragmentShader {
    FlatColorFrag() {}
    FlatColorFrag(glm::vec4 color)
        :  color(color)
    {}

    ShaderSource source() const {
        return ShaderSource::FlatColorFrag;
    }
    
    int32_t push_descriptor_set_idx() const {
        return 1;
    }

    void append_push_descriptor_sets(std::vector<PushDescriptorSetData>& sets) const {}

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {
        pcs.push_back(PushConstantData {
            .stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .offset = 0,
            .size = sizeof(glm::vec4),
            .p_data = &color
        });
    }

    glm::vec4 color;
};