#pragma once

#include "../rect.h"
#include "../shader_defs/shader_pairs.h"

#include "renderable.h"
#include "animatable.h"

#include <glm/gtc/matrix_transform.hpp>

struct ControllableQuad final : RenderableInterface {
    ControllableQuad(Renderer* renderer, Rect2D rect, glm::vec2 offset, double start_time, std::vector<TextureSource> textures, BufferID global_data_buffer)
        : rect(rect)
        , pos(rect.center())
        , shader_pair(
            TriangleTransformVert(renderer, glm::translate(glm::mat4(1.0f), glm::vec3(offset.x, offset.y, 0.0)), global_data_buffer),
            TriangleFrag(textures[0])
        )
        , sprite(3.0, start_time, std::move(textures))
    {}

    bool is_empty() const {
        return rect.is_zero_sized();
    }

    void update(const ViewUpdateData& data) {
        TriangleFrag* triangle_frag = dynamic_cast<TriangleFrag*>(shader_pair.fragment.get());
        triangle_frag->texture_binding = sprite.texture_at(data.total_elapsed_seconds);
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

    Point2Df32 pos;
    Rect2D rect;
    ShaderPair shader_pair;
    SpriteAnimation sprite;
};