#include "camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

glm::mat4 OrthographicCamera::get_proj_matrix()
{
    const float half_x = size_x / 2.0;
    const float half_y = size_y / 2.0;
    glm::mat4 proj = glm::ortho(
        center.x - half_x, center.x + half_x,
        center.y - half_y, center.y + half_y,
        -1.0f, 1.0f
    );
    return proj;
}

void OrthographicCamera::update(const CameraUpdateData& data) {
    // TODO: Copied from controlable view, abstract
    constexpr float displacement_per_second = 1.5;

    glm::vec2 movement_vec{0.0, 0.0};
    if (data.keyboard_state.is_down(SDLK_J)) {
        movement_vec.x -= 1.0;
    }
    if (data.keyboard_state.is_down(SDLK_I)){
        movement_vec.y += 1.0;
    }
    if (data.keyboard_state.is_down(SDLK_K)){
        movement_vec.y -= 1.0;
    }
    if (data.keyboard_state.is_down(SDLK_L)){
        movement_vec.x += 1.0;
    }

    if (glm::length(movement_vec) > 0.0) {
        movement_vec = glm::normalize(movement_vec);

        const float displacement = displacement_per_second * data.frame_dt;
        const glm::vec2 displacement_vec = movement_vec * displacement;
        center.x += displacement_vec.x;
        center.y += displacement_vec.y;
    }
}