#pragma once

#include "../rect.h"
#include "../shader_defs/shader_pairs.h"

#include "renderable.h"

struct DataQuad final : RenderableInterface {
    DataQuad(Renderer* renderer, Rect2D rect, TextureSource texture, BufferID global_data_buffer, glm::vec2 offset, glm::vec3 color)
        : rect(rect)
        , shader_pair(
            TriangleDataVert(renderer, global_data_buffer, glm::translate(glm::mat4(1.0f), glm::vec3(offset.x, offset.y, 0.0)), color),
            TriangleDataFrag(texture)
        )
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
        return rect.vertex_data(vertex_buffer);    
    }

    uint64_t index_data(uint32_t vertex_offset, std::vector<uint32_t>& index_buffer) {
        return rect.index_data(vertex_offset, index_buffer);
    }

    Rect2D rect;
    ShaderPair shader_pair;
};


