#include "camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

namespace{
    float scale_formula(float scale) {
        // NOTE: A quadratic scaling keeps the total area we are scaling by consistent since A = dX * dY -> A = dS^2
        return scale * scale;
    }
};

glm::mat4 OrthographicCamera::get_proj_matrix() const
{
    const float actual_scale = scale_formula(scale);

    const float half_x = actual_scale * size_x / 2.0;
    const float half_y = actual_scale * size_y / 2.0;
    glm::mat4 proj = glm::ortho(
        center.x - half_x, center.x + half_x,
        center.y - half_y, center.y + half_y,
        -1.0f, 1.0f
    );
    return proj;
}