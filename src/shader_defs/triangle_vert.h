#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "../renderer/draw.h"

#include "shader.h"

struct TriangleVertex {
    glm::vec3 in_position;
    glm::vec3 in_color;
};

struct TriangleVert final : VertexShader {
    TriangleVert() {}

    ShaderSource source() const {
        return ShaderSource::TriangleVert;
    }

    uint32_t vertex_num_floats() const {
        return sizeof(TriangleVertex) / sizeof(float);
    }
    
    void append_descriptor_sets(std::vector<DescriptorSetData>& sets) const {}
    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}
};