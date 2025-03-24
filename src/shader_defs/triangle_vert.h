#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#include "../renderer/renderer.h"
#include "../renderer/draw.h"

#include "shader.h"

struct TriangleVertex {
    glm::vec3 in_position;
    glm::vec3 in_color;
};

struct TriangleVert final : VertexShader {
    struct UniformData {
        glm::mat4 proj_matrix;
    };
    
    TriangleVert(Renderer* renderer, glm::mat4 proj_matrix) 
        : uniform_data(UniformData{
            .proj_matrix = proj_matrix
        })
    {
        buffer_id = renderer->request_buffer(
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_ALLOCATION_CREATE_MAPPED_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU,
            sizeof(UniformData)
        );

        // TODO: Update buffer
        Buffer& buffer = renderer->get_buffer(buffer_id);
        buffer.write_to(&uniform_data, sizeof(UniformData));
    }

    ShaderSource source() const {
        return ShaderSource::TriangleVert;
    }

    uint32_t vertex_num_floats() const {
        return sizeof(TriangleVertex) / sizeof(float);
    }
    
    void append_descriptor_sets(std::vector<DescriptorSetData>& sets) const {
        sets.push_back(DescriptorSetData{
            .set = 0,
            .binding = 0,
            .ty = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .buffer_id = buffer_id,
        });
    }

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}

    UniformData uniform_data;
    BufferID buffer_id;
};