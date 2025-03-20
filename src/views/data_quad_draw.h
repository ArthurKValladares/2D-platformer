#pragma once

#include "../rect.h"
#include "../shader_defs/shader_pairs.h"

#include "shared.h"
#include "renderable.h"

struct DataQuadDraw final : RenderableInterface {
    DataQuadDraw(Renderer* renderer, Rect2D rect, TextureSource texture, glm::mat4 render_matrix = glm::mat4(1.0), glm::vec4 color = glm::vec4(1.0))
        : rect(rect)
        , shader_pair(TriangleDataVert(renderer, render_matrix, color), TriangleDataFrag(texture))
    {}

    bool is_empty() const {
        return rect.is_zero_sized();
    }

    void update(const ViewUpdateData& data) {
        TriangleDataVert* triagle_data_vert = dynamic_cast<TriangleDataVert*>(shader_pair.vertex.get());

        const double offset = cos(data.elapsed_seconds) * 0.1;
        triagle_data_vert->uniform_data.render_matrix = 
            glm::translate(glm::mat4(1.0f), glm::vec3(offset, 0.0, 0.0));

        const double r = abs(tan(data.elapsed_seconds));
        const double g = abs(sin(data.elapsed_seconds * 0.5));
        const double b = abs(cos(data.elapsed_seconds * 0.25));
        triagle_data_vert->uniform_data.color = glm::vec4(r, g, b, 1.0);

        triagle_data_vert->update_buffer();
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


