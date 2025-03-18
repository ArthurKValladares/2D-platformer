#pragma once

#include "../rect.h"
#include "../shader_defs/shader_pairs.h"

#include "shared.h"

#include <glm/gtc/matrix_transform.hpp>

struct MovingQuadDraw {
    MovingQuadDraw(Rect2D rect, TextureSource texture)
        : rect(rect)
    {
        ShaderPair pair;
        pair.with_triangle_transform_vert(TriangleTransformVert(TriangleTransformPC{
            glm::mat4(1.0)
        }));
        pair.with_triangle_frag(TriangleFrag(texture));
        this->shaders = pair;
    }
    bool is_empty() const {
        return rect.is_zero_sized();
    }

    void update(const ViewUpdateData& data) {
        const double offset = sin(data.elapsed_seconds) * 0.1;
        shaders.triangle_transform_vert.push_constant.render_matrix = 
            glm::translate(glm::mat4(1.0f), glm::vec3(offset, 0.0, 0.0));
    }
    
    Rect2D rect;
    ShaderPair shaders;
};