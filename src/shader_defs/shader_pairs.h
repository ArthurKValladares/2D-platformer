#pragma once

#include <memory>

#include "../assets.h"

#include "../renderer/renderer.h"

#include "triangle_vert.h"
#include "triangle_transform_vert.h"
#include "triangle_frag.h"
#include "triangle_color_frag.h"
#include "triangle_data_vert.h"
#include "triangle_data_frag.h"

//
// TODO: A bunch of stuff in thus sub-dir can maybe be auto-generated.
//

struct ShaderPair {
    ShaderPair() {}

    template<class V, class F>
    requires std::is_base_of_v<VertexShader, V> &&
        std::is_base_of_v<FragmentShader, F>
    ShaderPair(V vertex, F fragment)
        : vertex(new V(std::move(vertex)))
        , fragment(new F(std::move(fragment)))
    {}

    void append_descriptor_sets(std::vector<DescriptorSetData>& sets) const;
    void append_push_constant_data(std::vector<PushConstantData>& pcs) const;

    uint32_t vertex_num_floats() const;
    
    std::unique_ptr<VertexShader> vertex;
    std::unique_ptr<FragmentShader> fragment;
};