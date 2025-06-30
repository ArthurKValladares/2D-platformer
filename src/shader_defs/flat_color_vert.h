#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#include "../renderer/renderer.h"
#include "../renderer/draw.h"

#include "shader.h"

// TODO: Get these from reflection?
struct FlatColorVertex {
    glm::vec3 in_position;
};

struct FlatColorVert final : VertexShader {    
    FlatColorVert() 
    {}

    ShaderSource source() const {
        return ShaderSource::FlatColorVert;
    }

    uint32_t vertex_num_floats() const {
        return sizeof(FlatColorVertex) / sizeof(float);
    }
    
    int32_t push_descriptor_set_idx() const {
        return -1;
    }

    void append_push_descriptor_sets(std::vector<PushDescriptorSetData>& sets) const {
    }

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}
};