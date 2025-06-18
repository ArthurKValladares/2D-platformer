#pragma once

#include "../rect.h"
#include "../shader_defs/shader_pairs.h"

#include "renderable.h"

#include <glm/gtc/matrix_transform.hpp>

// TODO: A lot of repetition in these renderables, fix it up
struct FlatColorQuad final : RenderableInterface {
    FlatColorQuad(Renderer* renderer, Rect2D rect, glm::vec4 color, BufferID global_data_buffer)
        : rect(rect)
        , shader_pair(FlatColorVert(renderer, global_data_buffer), FlatColorFrag(color))
    {}

    bool is_empty() const {
        return rect.is_zero_sized();
    }
    
    const ShaderPair& shaders() const {
        return shader_pair;
    }
    ShaderPair& shaders() {
        return shader_pair;
    }

    uint64_t vertex_data(std::vector<float>& vertex_buffer) {
        return rect.vertex_data(vertex_buffer, false);    
    }

    uint64_t index_data(uint32_t vertex_offset, std::vector<uint32_t>& index_buffer) {
        return rect.index_data(vertex_offset, index_buffer);
    }

    Rect2D rect;
    ShaderPair shader_pair;
};