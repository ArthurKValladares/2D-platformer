#pragma once

#include <cstdint>
#include <vector>

#include <glm/mat4x4.hpp>

#include "../renderer/draw.h"
#include "../renderer/renderer.h"
#include "../assets.h"

struct GlobalData {
    glm::mat4 proj_matrix;
};

// TODO: same optimization question as renderable
struct VertexShader{
    virtual ShaderSource source() const = 0;

    virtual void append_descriptor_sets(std::vector<DescriptorSetData>& sets) const = 0;
    virtual void append_push_constant_data(std::vector<PushConstantData>& pcs) const = 0;

    virtual uint32_t vertex_num_floats() const = 0;
};

struct FragmentShader{
    virtual ShaderSource source() const = 0;

    virtual void append_descriptor_sets(std::vector<DescriptorSetData>& sets) const = 0;
    virtual void append_push_constant_data(std::vector<PushConstantData>& pcs) const = 0;
};