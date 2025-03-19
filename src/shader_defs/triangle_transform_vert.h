#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#include "../renderer/draw.h"

struct TriangleTransformVertex {
    glm::vec3 in_position;
    glm::vec3 in_color;
};


struct TriangleTransformVert {
    TriangleTransformVert() {}
    TriangleTransformVert(glm::mat4 render_matrix)
        : render_matrix(render_matrix)
    {}
    
    uint32_t vertex_num_floats() const {
        return sizeof(TriangleTransformVertex) / sizeof(float);
    }

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {
        pcs.push_back(PushConstantData {
            .stage_flags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(glm::mat4),
            .p_data = &render_matrix
        });
    }

    glm::mat4 render_matrix;
};