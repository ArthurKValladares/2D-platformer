#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "../renderer/draw.h"

#include "shader.h"

struct TriangleDataVertex {
    glm::vec3 in_position;
    glm::vec3 in_color;
};

struct TriangleDataVert final : VertexShader {
    TriangleDataVert() {}

    ShaderSource source() const {
        return ShaderSource::TriangleDataVert;
    }

    uint32_t vertex_num_floats() const {
        return sizeof(TriangleDataVert) / sizeof(float);
    }
    
    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}
};