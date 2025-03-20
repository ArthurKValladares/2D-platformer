#pragma once

#include "../rect.h"
#include "../shader_defs/shader_pairs.h"

#include "shared.h"
#include "renderable.h"

struct QuadDraw final : RenderableInterface {
    QuadDraw(Rect2D rect, TextureSource texture)
        : rect(rect)
        , shader_pair(TriangleVert(), TriangleFrag(texture))
    {}

    bool is_empty() const {
        return rect.is_zero_sized();
    }

    void update(const ViewUpdateData& _data) {}
    
    const ShaderPair& shaders() const {
        return shader_pair;
    }
    ShaderPair& shaders() {
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


