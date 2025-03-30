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
    TriangleVert(Renderer* renderer, BufferID global_data_buffer) 
        : global_data_buffer(global_data_buffer)
    {}

    ShaderSource source() const {
        return ShaderSource::TriangleVert;
    }

    uint32_t vertex_num_floats() const {
        return sizeof(TriangleVertex) / sizeof(float);
    }
    
    void append_push_descriptor_sets(std::vector<PushDescriptorSetData>& sets) const {
    }

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}

    BufferID global_data_buffer;
};