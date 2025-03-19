#pragma once

#include <cstdint>
#include <vector>


#include "../renderer/draw.h"
#include "../renderer/renderer.h"
#include "../assets.h"

// TODO: same optimization question as renderable
struct VertexShader{
    virtual ShaderSource source() const = 0;

    virtual uint32_t vertex_num_floats() const = 0;

    virtual void append_push_constant_data(std::vector<PushConstantData>& pcs) const = 0;
};

struct FragmentShader{
    virtual ShaderSource source() const = 0;

    virtual DescriptorSetData draw_texture_binding() const = 0;
    virtual TextureSource draw_texture() const = 0;

    virtual void append_push_constant_data(std::vector<PushConstantData>& pcs) const = 0;
};