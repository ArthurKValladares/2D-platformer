#pragma once

#include "../rect.h"
#include "../shader_defs/shader_pairs.h"

#include "shared.h"
#include "renderable.h"

#include <glm/gtc/matrix_transform.hpp>

struct MovingQuadDraw final : RenderableInterface{
    MovingQuadDraw(Rect2D rect, TextureSource texture)
        : rect(rect)
        , shader_pair(
            TriangleTransformVert(glm::mat4(1.0)),
            TriangleFrag(texture)
        )
    {}

    bool is_empty() const {
        return rect.is_zero_sized();
    }

    void update(const ViewUpdateData& data) {
        TriangleTransformVert* triangle_transform_vert = dynamic_cast<TriangleTransformVert*>(shader_pair.vertex.get());

        const double offset = sin(data.elapsed_seconds) * 0.1;
        triangle_transform_vert->render_matrix = 
            glm::translate(glm::mat4(1.0f), glm::vec3(offset, 0.0, 0.0));
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