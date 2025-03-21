#pragma once

#include "../rect.h"
#include "../shader_defs/shader_pairs.h"

#include "shared.h"
#include "renderable.h"

#include <glm/gtc/matrix_transform.hpp>

struct ControllableQuadDraw final : RenderableInterface{
    ControllableQuadDraw(Rect2D rect, TextureSource texture)
        : rect(rect)
        , pos(rect.center())
        , shader_pair(
            TriangleTransformVert(glm::mat4(1.0)),
            TriangleFrag(texture)
        )
    {}

    bool is_empty() const {
        return rect.is_zero_sized();
    }

    void update(const ViewUpdateData& data) {
        // TODO: need a dt thing
        const auto displacement = 0.01;
        if (data.keyboard_state.is_down(SDLK_A)) {
            pos.x -= displacement;
        }
        if (data.keyboard_state.is_down(SDLK_W)){
            pos.y += displacement;
        }
        if (data.keyboard_state.is_down(SDLK_S)){
            pos.y -= displacement;
        }
        if (data.keyboard_state.is_down(SDLK_D)){
            pos.x += displacement;
        }

        TriangleTransformVert* triangle_transform_vert = dynamic_cast<TriangleTransformVert*>(shader_pair.vertex.get());

        triangle_transform_vert->render_matrix = 
            glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0));
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
};