#pragma once

#include "selectable.h"

#include <glm/gtc/matrix_transform.hpp>

struct RenderableRect : RenderableInterface {
    RenderableRect(Rect2D rect, ShaderPair shader_pair, bool vertex_uses_uv = true)
        : rect(rect)
        , shader_pair(std::move(shader_pair))
        , vertex_uses_uv(vertex_uses_uv)
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
        return rect.vertex_data(vertex_buffer, vertex_uses_uv);    
    }

    uint64_t index_data(uint32_t vertex_offset, std::vector<uint32_t>& index_buffer) {
        return rect.index_data(vertex_offset, index_buffer);
    }

    Rect2D rect;
    ShaderPair shader_pair;
    bool vertex_uses_uv;
};

static RenderableRect colored_quad(Renderer* renderer, Rect2D rect, TextureID texture, glm::vec3 color, BufferID global_data_buffer) {
    return RenderableRect(
        rect,
        ShaderPair(
            TriangleVert(renderer, global_data_buffer),
            TriangleColorFrag(texture, color)
        )
    );
}

static RenderableRect data_quad(Renderer* renderer, Rect2D rect, TextureID texture, BufferID global_data_buffer, glm::vec2 offset, glm::vec3 color) {
    return RenderableRect(
        rect,
        ShaderPair(
            TriangleDataVert(renderer, global_data_buffer, glm::translate(glm::mat4(1.0f), glm::vec3(offset.x, offset.y, 0.0)), color),
            TriangleDataFrag(texture)
        )
    );
}

static RenderableRect flat_color_quad(Renderer* renderer, Rect2D rect, glm::vec4 color, BufferID global_data_buffer) {
    return RenderableRect(
        rect,
        ShaderPair(
            FlatColorVert(renderer, global_data_buffer),
            FlatColorFrag(color)
        ),
        false
    );
}

static RenderableRect moving_quad(Renderer* renderer, Rect2D rect, glm::vec2 offset, TextureID texture, BufferID global_data_buffer) {
    return RenderableRect(
        rect,
        ShaderPair(
            TriangleTransformVert(renderer, glm::translate(glm::mat4(1.0f), glm::vec3(offset.x, offset.y, 0.0)), global_data_buffer),
            TriangleFrag(texture)
        )
    );
}

static RenderableRect outline_quad(Renderer* renderer, Rect2D rect, TextureID texture, OutlinePushConstantData pc_data, BufferID global_data_buffer) {
    return RenderableRect(
        rect,
        ShaderPair(
            TriangleVert(renderer, global_data_buffer),
            OutlineFrag(texture, pc_data)
        )
    );
}

static RenderableRect quad(Renderer* renderer, Rect2D rect, TextureID texture, BufferID global_data_buffer) {
    return RenderableRect(
        rect,
        ShaderPair(
            TriangleVert(renderer, global_data_buffer),
            TriangleFrag(texture)
        )
    );
}

static RenderableRect font(Renderer* renderer, Rect2D rect, TextureID texture, glm::vec3 color, BufferID global_data_buffer) {
    return RenderableRect(
        rect,
        ShaderPair(
            TriangleVert(renderer, global_data_buffer),
            FontFrag(texture, color)
        )
    );
}
