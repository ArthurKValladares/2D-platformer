#include "camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

glm::mat4 OrthographicCamera::get_proj_matrix() const
{
    const float actual_scale = sqrt_scale * sqrt_scale;

    const float half_x = actual_scale * size.x / 2.0;
    const float half_y = actual_scale * size.y / 2.0;
    glm::mat4 proj = glm::ortho(
        center.x - half_x, center.x + half_x,
        center.y - half_y, center.y + half_y,
        -1.0f, 1.0f
    );
    return proj;
}