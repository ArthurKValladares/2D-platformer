#pragma once

#include "../rect.h"
#include "../shader_defs/shader_pairs.h"

#include "shared.h"
#include "renderable.h"

struct DataQuadDraw final : RenderableInterface {
    DataQuadDraw(Renderer* renderer, Rect2D rect, TextureSource texture, glm::mat4 render_matrix = glm::mat4(1.0))
        : rect(rect)
        , shader_pair(TriangleDataVert(renderer, render_matrix), TriangleDataFrag(texture))
    {}

    bool is_empty() const {
        return rect.is_zero_sized();
    }

    void update(const ViewUpdateData& _data) {}
    
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


