#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#include "../renderer/renderer.h"
#include "../renderer/draw.h"

#include "shader.h"

struct TriangleTransformVertex {
    glm::vec3 in_position;
    glm::vec3 in_color;
};

struct TriangleTransformVert final : VertexShader {    
    TriangleTransformVert() {}
    TriangleTransformVert(Renderer* renderer, glm::mat4 render_matrix, BufferID global_data_buffer)
        : render_matrix(render_matrix)
        , global_data_buffer(global_data_buffer)
    {}
    
    ShaderSource source() const {
        return ShaderSource::TriangleTransformVert;
    }

    uint32_t vertex_num_floats() const {
        return sizeof(TriangleTransformVertex) / sizeof(float);
    }

    int32_t push_descriptor_set_idx() const {
        return -1;
    }

    void append_push_descriptor_sets(std::vector<PushDescriptorSetData>& sets) const {
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
    BufferID global_data_buffer;
};