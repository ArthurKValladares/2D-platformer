#include "shader_pairs.h"

uint32_t ShaderPair::vertex_num_floats() const {
    switch (vertex_ty)
    {
        case ShaderSource::TriangleVert: {
            return triangle_vert.vertex_num_floats();
            break;
        }
        case ShaderSource::TriangleTransformVert: {
            return triangle_transform_vert.vertex_num_floats();
            break;
        }
        default: {
            assert(false);
            return 0;
        }
    }
}

TextureSource ShaderPair::draw_texture() const {
    switch (fragment_ty)
    {
        case ShaderSource::TriangleFrag: {
            return triangle_frag.texture_binding;
        }
        default: {
            assert(false);
            return TextureSource::Count;
        }
    }
}

void ShaderPair::append_push_constant_data(std::vector<PushConstantData>& pcs) const {
    switch (vertex_ty)
    {
        case ShaderSource::TriangleVert: {
            triangle_vert.append_push_constant_data(pcs);
            break;
        }
        case ShaderSource::TriangleTransformVert: {
            triangle_transform_vert.append_push_constant_data(pcs);
            break;
        }
        default: {
            assert(false);
            break;
        }
    }

    switch (fragment_ty)
    {
        case ShaderSource::TriangleFrag: {
            triangle_frag.append_push_constant_data(pcs);
            break;
        }
        default: {
            assert(false);
            break;
        }
    }
}