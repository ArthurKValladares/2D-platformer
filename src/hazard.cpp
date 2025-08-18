#include "hazard.h"

#include <glm/gtx/vector_angle.hpp>

Renderable Hazard::draw() const {
    const glm::vec2 default_hazard_angle = glm::vec2(0.0, 1.0);
    float rotation = glm::angle(dir, default_hazard_angle);
    if (dir.x > 0.0) {
        rotation -= glm::pi<float>();
    }

    const glm::vec2 translation = rect.pos;
    return moving_quad(
        Rect2D(glm::vec2(0.0), rect.size()),
        translation,
        rotation,
        texture_id(texture)
        //color
    );
}