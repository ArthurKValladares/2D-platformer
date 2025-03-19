#pragma once

#include "../rect.h"
#include "../shader_defs/shader_pairs.h"

#include "shared.h"
#include "renderable.h"

#include <glm/gtc/matrix_transform.hpp>

struct ColorQuadDraw final : RenderableInterface{
    ColorQuadDraw(Rect2D rect, TextureSource texture)
        : rect(rect)
        , shader_pair(TriangleVert(), TriangleColorFrag(texture))
    {}

    bool is_empty() const {
        return rect.is_zero_sized();
    }

    void update(const ViewUpdateData& data) {
        TriangleColorFrag* triangle_color_frag = dynamic_cast<TriangleColorFrag*>(shader_pair.fragment.get());

        const double r = abs(sin(data.elapsed_seconds));
        const double g = abs(cos(data.elapsed_seconds * 0.5));
        const double b = abs(tan(data.elapsed_seconds * 0.25));
        triangle_color_frag->color = glm::vec3(r, g, b);
    }
    
    const ShaderPair& shaders() const {
        return shader_pair;
    }

    uint64_t vertex_data(std::vector<float>& vertex_buffer) {
        return rect.vertex_data(vertex_buffer);    
    }

    uint64_t index_data(uint32_t vertex_offset, std::vector<uint32_t>& index_buffer) {
        return rect.index_data(vertex_offset, index_buffer);
    }

    Rect2D rect;
    ShaderPair shader_pair;
};