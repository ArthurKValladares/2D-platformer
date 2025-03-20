#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#include "../renderer/renderer.h"
#include "../renderer/draw.h"
#include "../renderer/buffer.h"

#include "shader.h"

struct TriangleDataVertex {
    glm::vec3 in_position;
    glm::vec3 in_color;
};

struct UniformData {
    glm::mat4 render_matrix;
    glm::vec4 color;
};

struct TriangleDataVert final : VertexShader {
    TriangleDataVert(Renderer* renderer, glm::mat4 render_matrix, glm::vec4 color)
        : uniform_data(UniformData{
            .render_matrix = render_matrix,
            .color = color,
        })
    {
        render_matrix_buffer = Buffer(
            renderer->get_allocator(),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_ALLOCATION_CREATE_MAPPED_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU,
            &this->uniform_data,
            sizeof(UniformData)
        );
    }

    ShaderSource source() const {
        return ShaderSource::TriangleDataVert;
    }

    uint32_t vertex_num_floats() const {
        return sizeof(TriangleDataVert) / sizeof(float);
    }
    
    void append_descriptor_sets(std::vector<DescriptorSetData>& sets) const {
        sets.push_back(DescriptorSetData{
            .set = 0,
            .binding = 0,
            .ty = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .buffer = &render_matrix_buffer,
        });
    }
    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}

    // TODO: I'm leaking this for now but its fine, it won't live here in the end
    UniformData uniform_data;
    Buffer render_matrix_buffer;
};