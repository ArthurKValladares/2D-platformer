#pragma once

#pragma once

#include "shader.h"

#include "../assets.h"
#include "../renderer/draw.h"
#include "../renderer/renderer.h"

#include <vector>

#include <glm/vec3.hpp>

struct TriangleColorFrag final : FragmentShader {
    TriangleColorFrag() {}
    TriangleColorFrag(TextureSource texture, glm::vec3 color = glm::vec3(0.0))
        :  texture_binding(texture)
    {}

    ShaderSource source() const {
        return ShaderSource::TriangleColorFrag;
    }
    
    void append_descriptor_sets(std::vector<DescriptorSetData>& sets) const {
        sets.push_back(DescriptorSetData{
            .set = 0,
            .binding = 0,
            .ty = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .texture_id = texture_id(texture_binding),
        });
    }

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {
        pcs.push_back(PushConstantData {
            .stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .offset = 0,
            .size = sizeof(glm::vec3),
            .p_data = &color
        });
    }

    TextureSource texture_binding;
    glm::vec3 color;
};