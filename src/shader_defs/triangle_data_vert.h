#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

#include "../renderer/renderer.h"
#include "../renderer/draw.h"
#include "../renderer/buffer.h"

#include "shader.h"

struct TriangleDataVertex {
    glm::vec3 in_position;
    glm::vec3 in_color;
};

struct TriangleDataVert final : VertexShader {
    struct UniformData {
        glm::mat4 render_matrix;
        glm::vec4 color;
    };
    
    TriangleDataVert(Renderer* renderer, BufferID global_data_buffer, glm::mat4 render_matrix, glm::vec4 color)
        : uniform_data(UniformData{
            .render_matrix = render_matrix,
            .color = color,
        })
        , global_data_buffer(global_data_buffer)
    {
        uniform_buffer = renderer->request_buffer(
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_ALLOCATION_CREATE_MAPPED_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU,
            sizeof(UniformData)
        );
    }

    ShaderSource source() const {
        return ShaderSource::TriangleDataVert;
    }

    uint32_t vertex_num_floats() const {
        return sizeof(TriangleDataVertex) / sizeof(float);
    }
    
    void append_push_descriptor_sets(std::vector<PushDescriptorSetData>& sets) const {
        sets.push_back(PushDescriptorSetData{
            .set = 0,
            .binding = 0,
            .ty = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .buffer_id = global_data_buffer,
        });
        sets.push_back(PushDescriptorSetData{
            .set = 0,
            .binding = 2,
            .ty = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .buffer_id = uniform_buffer,
        });
    }
    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}

    void update_buffer(Renderer* renderer) {
        Buffer& buffer = renderer->get_buffer(uniform_buffer);
        buffer.write_to(&uniform_data, sizeof(UniformData));
    }

    UniformData uniform_data;
    BufferID uniform_buffer;
    BufferID global_data_buffer;
};