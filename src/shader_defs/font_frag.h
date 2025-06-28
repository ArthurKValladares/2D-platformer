#pragma once

#pragma once

#include "shader.h"

#include "../assets.h"
#include "../renderer/draw.h"
#include "../renderer/renderer.h"

#include <vector>

#include <glm/vec3.hpp>

struct FontFrag final : FragmentShader {
    FontFrag() {}
    FontFrag(TextureID texture, glm::vec3 color = glm::vec3(0.0))
        :  texture_id(texture)
        , color(color)
    {}

    ShaderSource source() const {
        return ShaderSource::FontFrag;
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

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {
        pcs.push_back(PushConstantData {
            .stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .offset = 0,
            .size = sizeof(glm::vec3),
            .p_data = &color
        });
    }

    TextureID texture_id;
    glm::vec3 color;
};