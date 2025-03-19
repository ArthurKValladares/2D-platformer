#pragma once

#include "../assets.h"

#include "shared.h"

#include "triangle_vert.h"
#include "triangle_transform_vert.h"
#include "triangle_frag.h"
#include "triangle_color_frag.h"

//
// TODO: A bunch of stuff in thus sub-dir can maybe be auto-generated.
//

struct ShaderPair {
    ShaderPair() {}

    void with_triangle_vert(TriangleVert vert) {
        vertex_ty = ShaderSource::TriangleVert;
        triangle_vert = vert;
    }
    void with_triangle_transform_vert(TriangleTransformVert vert) {
        vertex_ty = ShaderSource::TriangleTransformVert;
        triangle_transform_vert = vert;
    }
    void with_triangle_frag(TriangleFrag frag) {
        fragment_ty = ShaderSource::TriangleFrag;
        triangle_frag = frag;
    }
    void with_triangle_color_frag(TriangleColorFrag frag) {
        fragment_ty = ShaderSource::TriangleColorFrag;
        triangle_color_frag = frag;
    }

    uint32_t vertex_num_floats() const;

    // TODO: This is not quite right, but ok for now
    // I need a better way to get descriptor set data i can use in the rendererer
    DescriptorSetData draw_texture_binding() const;
    TextureSource draw_texture() const;

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const;

    ShaderSource vertex_ty;
    union {
        TriangleVert triangle_vert;
        TriangleTransformVert triangle_transform_vert;
    };
    
    ShaderSource fragment_ty;
    union {
        TriangleFrag triangle_frag;
        TriangleColorFrag triangle_color_frag;
    };
};