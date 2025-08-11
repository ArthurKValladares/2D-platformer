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

static RenderableRect colored_quad(Rect2D rect, TextureID texture, glm::vec3 color) {
    return RenderableRect(
        rect,
        ShaderPair(
            TriangleVert(),
            TriangleColorFrag(texture, color)
        )
    );
}

static RenderableRect data_quad(Renderer* renderer, Rect2D rect, TextureID texture, glm::vec2 offset, glm::vec3 color) {
    return RenderableRect(
        rect,
        ShaderPair(
            TriangleDataVert(renderer, glm::translate(glm::mat4(1.0f), glm::vec3(offset.x, offset.y, 0.0)), color),
            TriangleDataFrag(texture)
        )
    );
}

static RenderableRect flat_color_quad(Rect2D rect, glm::vec4 color) {
    return RenderableRect(
        rect,
        ShaderPair(
            FlatColorVert(),
            FlatColorFrag(color)
        ),
        false
    );
}

static RenderableRect moving_quad(Rect2D rect, glm::vec2 offset, float rotation_radians, TextureID texture) {
    const glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0), rotation_radians, glm::vec3(0, 0, 1));

    return RenderableRect(
        rect,
        ShaderPair(
            TriangleTransformVert(glm::translate(glm::mat4(1.0), glm::vec3(offset.x, offset.y, 0.0)) * rotate_mat),
            TriangleFrag(texture)
        )
    );
}

static RenderableRect outline_quad(Rect2D rect, TextureID texture, OutlinePushConstantData pc_data) {
    return RenderableRect(
        rect,
        ShaderPair(
            TriangleVert(),
            OutlineFrag(texture, pc_data)
        )
    );
}

static RenderableRect quad(Rect2D rect, TextureID texture) {
    return RenderableRect(
        rect,
        ShaderPair(
            TriangleVert(),
            TriangleFrag(texture)
        )
    );
}

static RenderableRect font(Rect2D rect, TextureID texture, glm::vec3 color) {
    return RenderableRect(
        rect,
        ShaderPair(
            TriangleVert(),
            FontFrag(texture, color)
        )
    );
}
