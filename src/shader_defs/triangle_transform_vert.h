#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#include "../renderer/draw.h"

struct TriangleTransformVertex {
    glm::vec3 in_position;
    glm::vec3 in_color;
};

struct TriangleTransformPC {
    glm::mat4 render_matrix;
};

struct TriangleTransformVert {
    TriangleTransformVert() {}
    TriangleTransformVert(TriangleTransformPC push_constant)
        : push_constant(push_constant)
    {}
    
    uint32_t vertex_num_floats() const {
        return sizeof(TriangleTransformVertex) / sizeof(float);
    }

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {
        pcs.push_back(PushConstantData {
            .stage_flags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(glm::mat4),
            .p_data = &push_constant.render_matrix
        });
    }

    TriangleTransformPC push_constant;
};