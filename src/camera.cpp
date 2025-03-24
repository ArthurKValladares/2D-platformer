#include "camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

glm::mat4 OrthographicCamera::get_proj_matrix()
{
    const float half_x = size_x / 2.0;
    const float half_y = size_y / 2.0;
    glm::mat4 proj = glm::ortho(
        -half_x, half_x,
        -half_y, half_y,
        0.0f, 1.0f
    );
    return proj;
}